#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "walib.h"
/*
 * This is a regular expression library that implements a subset of Perl RE.
 * Please refer to http://slre.sourceforge.net for detailed description.
 *
 * Usage example (parsing HTTP request):
 *
 * struct slre	slre;
 * struct cap	captures[4 + 1];  // Number of braket pairs + 1
 * ...
 *
 * slre_compile(&slre,"^(GET|POST) (\S+) HTTP/(\S+?)\r\n");
 *
 * if (slre_match(&slre, buf, len, captures)) {
 *	printf("Request line length: %d\n", captures[0].len);
 *	printf("Method: %.*s\n", captures[1].len, captures[1].ptr);
 *	printf("URI: %.*s\n", captures[2].len, captures[2].ptr);
 * }
 */

/*
 * Compiled regular expression
 */
struct slre {
	unsigned char	code[256];
	unsigned char	data[256];
	int		code_size;
	int		data_size;
	int		num_caps;	/* Number of bracket pairs	*/
	int		anchored;	/* Must match from string start	*/
	const char	*err_str;	/* Error string			*/
};

/*
 * Captured substring
 */
struct cap {
	const char	*ptr;		/* Pointer to the substring	*/
	int		len;		/* Substring length		*/
};


enum {END, BRANCH, ANY, EXACT, ANYOF, ANYBUT, OPEN, CLOSE, BOL, EOL,
	STAR, PLUS, STARQ, PLUSQ, QUEST, SPACE, NONSPACE, DIGIT,
	LOWER, UPPER, ALPHA, ALNUM};

static struct {
	const char	*name;
	int		narg;
	const char	*flags;
} opcodes[] = {
	{"END",		0, ""},		/* End of code block or program	*/
	{"BRANCH",	2, "oo"},	/* Alternative operator, "|"	*/
	{"ANY",		0, ""},		/* Match any character, "."	*/
	{"EXACT",	2, "d"},	/* Match exact string		*/
	{"ANYOF",	2, "D"},	/* Match any from set, "[]"	*/
	{"ANYBUT",	2, "D"},	/* Match any but from set, "[^]"*/
	{"OPEN ",	1, "i"},	/* Capture start, "("		*/
	{"CLOSE",	1, "i"},	/* Capture end, ")"		*/
	{"BOL",		0, ""},		/* Beginning of string, "^"	*/
	{"EOL",		0, ""},		/* End of string, "$"		*/
	{"STAR",	1, "o"},	/* Match zero or more times "*"	*/
	{"PLUS",	1, "o"},	/* Match one or more times, "+"	*/
	{"STARQ",	1, "o"},	/* Non-greedy STAR,  "*?"	*/
	{"PLUSQ",	1, "o"},	/* Non-greedy PLUS, "+?"	*/
	{"QUEST",	1, "o"},	/* Match zero or one time, "?"	*/
	{"SPACE",	0, ""},		/* Match whitespace, "\s"	*/
	{"NONSPACE",	0, ""},		/* Match non-space, "\S"	*/
	{"DIGIT",	0, ""}		/* Match digit, "\d"		*/
	,{"LOWER",	0, ""}		/* Match lower, "\l"	*/
	,{"UPPER",	0, ""}		/* Match upper, "\u"	*/
	,{"ALPHA",	0, ""}		/* Match alpha, "\a"	*/
	,{"ALNUM",	0, ""}		/* Match alnum, "\w"	*/
};

/*
 * Commands and operands are all unsigned char (1 byte long). All code offsets
 * are relative to current address, and positive (always point forward). Data
 * offsets are absolute. Commands with operands:
 *
 * BRANCH offset1 offset2
 *	Try to match the code block that follows the BRANCH instruction
 *	(code block ends with END). If no match, try to match code block that
 *	starts at offset1. If either of these match, jump to offset2.
 *
 * EXACT data_offset data_length
 *	Try to match exact string. String is recorded in data section from
 *	data_offset, and has length data_length.
 *
 * OPEN capture_number
 * CLOSE capture_number
 *	If the user have passed 'struct cap' array for captures, OPEN
 *	records the beginning of the matched substring (cap->ptr), CLOSE
 *	sets the length (cap->len) for respective capture_number.
 *
 * STAR code_offset
 * PLUS code_offset
 * QUEST code_offset
 *	*, +, ?, respectively. Try to gobble as much as possible from the
 *	matched buffer, until code block that follows these instructions
 *	matches. When the longest possible string is matched,
 *	jump to code_offset
 *
 * STARQ, PLUSQ are non-greedy versions of STAR and PLUS.
 */

static const char *meta_chars = "|.^$*+?()[\\";

static void
print_character_set(FILE *fp, const unsigned char *p, int len)
{
	int	i;

	for (i = 0; i < len; i++) {
		if (i > 0)
			(void) fputc(',', fp);
		if (p[i] == 0) {
			i++;
			if (p[i] == 0)
				(void) fprintf(fp, "\\x%02x", p[i]);
			else
				(void) fprintf(fp, "%s", opcodes[p[i]].name);
		} else if (isprint(p[i])) {
			(void) fputc(p[i], fp);
		} else {
			(void) fprintf(fp,"\\x%02x", p[i]);
		}
	}
}

void
slre_dump(const struct slre *r, FILE *fp)
{
	int	i, j, ch, op, pc;

	for (pc = 0; pc < r->code_size; pc++) {

		op = r->code[pc];
		(void) fprintf(fp, "%3d %s ", pc, opcodes[op].name);

		for (i = 0; opcodes[op].flags[i] != '\0'; i++)
			switch (opcodes[op].flags[i]) {
			case 'i':
				(void) fprintf(fp, "%d ", r->code[pc + 1]);
				pc++;
				break;
			case 'o':
				(void) fprintf(fp, "%d ",
				    pc + r->code[pc + 1] - i);
				pc++;
				break;
			case 'D':
				print_character_set(fp, r->data +
				    r->code[pc + 1], r->code[pc + 2]);
				pc += 2;
				break;
			case 'd':
				(void) fputc('"', fp);
				for (j = 0; j < r->code[pc + 2]; j++) {
					ch = r->data[r->code[pc + 1] + j];
					if (isprint(ch))
						(void) fputc(ch, fp);
					else
						(void) fprintf(fp,"\\x%02x",ch);
				}
				(void) fputc('"', fp);
				pc += 2;
				break;
			}

		(void) fputc('\n', fp);
	}
}

static void
set_jump_offset(struct slre *r, int pc, int offset)
{
	assert(offset < r->code_size);

	if (r->code_size - offset > 0xff) {
		r->err_str = "Jump offset is too big";
	} else {
		r->code[pc] = (unsigned char) (r->code_size - offset);
	}
}

static void
emit(struct slre *r, int code)
{
	if (r->code_size >= (int) (sizeof(r->code) / sizeof(r->code[0])))
		r->err_str = "RE is too long (code overflow)";
	else
		r->code[r->code_size++] = (unsigned char) code;
}

static void
store_char_in_data(struct slre *r, int ch)
{
	if (r->data_size >= (int) sizeof(r->data))
		r->err_str = "RE is too long (data overflow)";
	else
		r->data[r->data_size++] = ch;
}

static void
exact(struct slre *r, const char **re)
{
	int	old_data_size = r->data_size;

	while (**re != '\0' && (strchr(meta_chars, **re)) == NULL)
		store_char_in_data(r, *(*re)++);

	emit(r, EXACT);
	emit(r, old_data_size);
	emit(r, r->data_size - old_data_size);
}

static int
get_escape_char(const char **re)
{
	int	res;

	switch (*(*re)++) {
	case 'n':	res = '\n';		break;
	case 'r':	res = '\r';		break;
	case 't':	res = '\t';		break;
	case '0':	res = 0;		break;
	case 'S':	res = NONSPACE << 8;	break;
	case 's':	res = SPACE << 8;	break;
	case 'd':	res = DIGIT << 8;	break;
	case 'l':	res = LOWER << 8;	break;
	case 'u':	res = UPPER << 8;	break;
	case 'a':	res = ALPHA << 8;	break;
	case 'w':	res = ALNUM << 8;	break;
	default:	res = (*re)[-1];	break;
	}

	return (res);
}

static void
anyof(struct slre *r, const char **re)
{
	int	esc, old_data_size = r->data_size, op = ANYOF;

	if (**re == '^') {
		op = ANYBUT;
		(*re)++;
	}

	while (**re != '\0')

		switch (*(*re)++) {
		case ']':
			emit(r, op);
			emit(r, old_data_size);
			emit(r, r->data_size - old_data_size);
			return;
			/* NOTREACHED */
			break;
		case '\\':
			esc = get_escape_char(re);
			if ((esc & 0xff) == 0) {
				store_char_in_data(r, 0);
				store_char_in_data(r, esc >> 8);
			} else {
				store_char_in_data(r, esc);
			}
			break;
		default:
			store_char_in_data(r, (*re)[-1]);
			break;
		}

	r->err_str = "No closing ']' bracket";
}

static void
relocate(struct slre *r, int begin, int shift)
{
	emit(r, END);
	memmove(r->code + begin + shift, r->code + begin, r->code_size - begin);
	r->code_size += shift;
}

static void
quantifier(struct slre *r, int prev, int op)
{
	if (r->code[prev] == EXACT && r->code[prev + 2] > 1) {
		r->code[prev + 2]--;
		emit(r, EXACT);
		emit(r, r->code[prev + 1] + r->code[prev + 2]);
		emit(r, 1);
		prev = r->code_size - 3;
	}
	relocate(r, prev, 2);
	r->code[prev] = op;
	set_jump_offset(r, prev + 1, prev);
}

static void
exact_one_char(struct slre *r, int ch)
{
	emit(r, EXACT);
	emit(r, r->data_size);
	emit(r, 1);
	store_char_in_data(r, ch);
}

static void
fixup_branch(struct slre *r, int fixup)
{
	if (fixup > 0) {
		emit(r, END);
		set_jump_offset(r, fixup, fixup - 2);
	}
}

static void
compile(struct slre *r, const char **re)
{
	int	op, esc, branch_start, last_op, fixup, cap_no, level;

	fixup = 0;
	level = r->num_caps;
	branch_start = last_op = r->code_size;

	for (;;)
		switch (*(*re)++) {
		case '\0':
			(*re)--;
			return;
			/* NOTREACHED */
			break;
		case '^':
			emit(r, BOL);
			break;
		case '$':
			emit(r, EOL);
			break;
		case '.':
			last_op = r->code_size;
			emit(r, ANY);
			break;
		case '[':
			last_op = r->code_size;
			anyof(r, re);
			break;
		case '\\':
			last_op = r->code_size;
			esc = get_escape_char(re);
			if (esc & 0xff00) {
				emit(r, esc >> 8);
			} else {
				exact_one_char(r, esc);
			}
			break;
		case '(':
			last_op = r->code_size;
			cap_no = ++r->num_caps;
			emit(r, OPEN);
			emit(r, cap_no);

			compile(r, re);
			if (*(*re)++ != ')') {
				r->err_str = "No closing bracket";
				return;
			}

			emit(r, CLOSE);
			emit(r, cap_no);
			break;
		case ')':
			(*re)--;
			fixup_branch(r, fixup);
			if (level == 0) {
				r->err_str = "Unbalanced brackets";
				return;
			}
			return;
			/* NOTREACHED */
			break;
		case '+':
		case '*':
			op = (*re)[-1] == '*' ? STAR: PLUS;
			if (**re == '?') {
				(*re)++;
				op = op == STAR ? STARQ : PLUSQ;
			}
			quantifier(r, last_op, op);
			break;
		case '?':
			quantifier(r, last_op, QUEST);
			break;
		case '|':
			fixup_branch(r, fixup);
			relocate(r, branch_start, 3);
			r->code[branch_start] = BRANCH;
			set_jump_offset(r, branch_start + 1, branch_start);
			fixup = branch_start + 2;
			r->code[fixup] = 0xff;
			break;
		default:
			(*re)--;
			last_op = r->code_size;
			exact(r, re);
			break;
		}
}

/*
 * Compile regular expression. If success, 1 is returned.
 * If error, 0 is returned and slre.err_str points to the error message.
 */
static int
slre_compile(struct slre *r, const char *re)
{
	r->err_str = NULL;
	r->code_size = r->data_size = r->num_caps = r->anchored = 0;

	if (*re == '^')
		r->anchored++;

	emit(r, OPEN);	/* This will capture what matches full RE */
	emit(r, 0);

	while (*re != '\0')
		compile(r, &re);

	if (r->code[2] == BRANCH)
		fixup_branch(r, 4);

	emit(r, CLOSE);
	emit(r, 0);
	emit(r, END);

	return (r->err_str == NULL ? 1 : 0);
}

static int match(const struct slre *, int,
		const char *, int, int *, struct cap *);

static void
loop_greedy(const struct slre *r, int pc, const char *s, int len, int *ofs)
{
	int	saved_offset, matched_offset;

	saved_offset = matched_offset = *ofs;

	while (match(r, pc + 2, s, len, ofs, NULL)) {
		saved_offset = *ofs;
		if (match(r, pc + r->code[pc + 1], s, len, ofs, NULL))
			matched_offset = saved_offset;
		*ofs = saved_offset;
	}

	*ofs = matched_offset;
}

static void
loop_non_greedy(const struct slre *r, int pc, const char *s,int len, int *ofs)
{
	int	saved_offset = *ofs;

	while (match(r, pc + 2, s, len, ofs, NULL)) {
		saved_offset = *ofs;
		if (match(r, pc + r->code[pc + 1], s, len, ofs, NULL))
			break;
	}

	*ofs = saved_offset;
}

static int
is_any_of(const unsigned char *p, int len, const char *s, int *ofs)
{
	int	i, ch;

	ch = s[*ofs];

	for (i = 0; i < len; i++)
		if (p[i] == ch) {
			(*ofs)++;
			return (1);
		}

	return (0);
}

static int
is_any_but(const unsigned char *p, int len, const char *s, int *ofs)
{
	int	i, ch;

	ch = s[*ofs];

	for (i = 0; i < len; i++)
		if (p[i] == ch)
			return (0);

	(*ofs)++;
	return (1);
}

static int
match(const struct slre *r, int pc, const char *s, int len,
		int *ofs, struct cap *caps)
{
	int	n, saved_offset, res = 1;

	while (res && r->code[pc] != END) {

		assert(pc < r->code_size);
		assert(pc < (int) (sizeof(r->code) / sizeof(r->code[0])));

		switch (r->code[pc]) {
		case BRANCH:
			saved_offset = *ofs;
			res = match(r, pc + 3, s, len, ofs, caps);
			if (res == 0) {
				*ofs = saved_offset;
				res = match(r, pc + r->code[pc + 1],
				    s, len, ofs, caps);
			}
			pc += r->code[pc + 2];
			break;
		case EXACT:
			res = 0;
			n = r->code[pc + 2];	/* String length */
			if (n <= len - *ofs && !memcmp(s + *ofs, r->data +
			    r->code[pc + 1], n)) {
				(*ofs) += n;
				res = 1;
			}
			pc += 3;
			break;
		case QUEST:
			res = 1;
			saved_offset = *ofs;
			if (!match(r, pc + 2, s, len, ofs, caps))
				*ofs = saved_offset;
			pc += r->code[pc + 1];
			break;
		case STAR:
			res = 1;
			loop_greedy(r, pc, s, len, ofs);
			pc += r->code[pc + 1];
			break;
		case STARQ:
			res = 1;
			loop_non_greedy(r, pc, s, len, ofs);
			pc += r->code[pc + 1];
			break;
		case PLUS:
			if ((res = match(r, pc + 2, s, len, ofs, caps)) == 0)
				break;

			loop_greedy(r, pc, s, len, ofs);
			pc += r->code[pc + 1];
			break;
		case PLUSQ:
			if ((res = match(r, pc + 2, s, len, ofs, caps)) == 0)
				break;

			loop_non_greedy(r, pc, s, len, ofs);
			pc += r->code[pc + 1];
			break;
		case SPACE:
			res = 0;
			if (*ofs < len && isspace(((unsigned char *)s)[*ofs])) {
				(*ofs)++;
				res = 1;
			}
			pc++;
			break;
		case NONSPACE:
			res = 0;
			if (*ofs <len && !isspace(((unsigned char *)s)[*ofs])) {
				(*ofs)++;
				res = 1;
			}
			pc++;
			break;
		case DIGIT:
			res = 0;
			if (*ofs < len && isdigit(((unsigned char *)s)[*ofs])) {
				(*ofs)++;
				res = 1;
			}
			pc++;
			break;
		case LOWER:
			res = 0;
			if (*ofs < len && islower(((unsigned char *)s)[*ofs])) {
				(*ofs)++;
				res = 1;
			}
			pc++;
			break;
		case UPPER:
			res = 0;
			if (*ofs < len && isupper(((unsigned char *)s)[*ofs])) {
				(*ofs)++;
				res = 1;
			}
			pc++;
			break;
		case ALPHA:
			res = 0;
			if (*ofs < len && isalpha(((unsigned char *)s)[*ofs])) {
				(*ofs)++;
				res = 1;
			}
			pc++;
			break;
		case ALNUM:
			res = 0;
			if (*ofs < len && isalnum(((unsigned char *)s)[*ofs])) {
				(*ofs)++;
				res = 1;
			}
			pc++;
			break;
		case ANY:
			res = 0;
			if (*ofs < len) {
				(*ofs)++;
				res = 1;
			}
			pc++;
			break;
		case ANYOF:
			res = 0;
			if (*ofs < len)
				res = is_any_of(r->data + r->code[pc + 1],
					r->code[pc + 2], s, ofs);
			pc += 3;
			break;
		case ANYBUT:
			res = 0;
			if (*ofs < len)
				res = is_any_but(r->data + r->code[pc + 1],
					r->code[pc + 2], s, ofs);
			pc += 3;
			break;
		case BOL:
			res = *ofs == 0 ? 1 : 0;
			pc++;
			break;
		case EOL:
			res = *ofs == len ? 1 : 0;
			pc++;
			break;
		case OPEN:
			if (caps != NULL)
				caps[r->code[pc + 1]].ptr = s + *ofs;
			pc += 2;
			break;
		case CLOSE:
			if (caps != NULL)
				caps[r->code[pc + 1]].len = (s + *ofs) -
				    caps[r->code[pc + 1]].ptr;
			pc += 2;
			break;
		case END:
			pc++;
			break;
		default:
			printf("unknown cmd (%d) at %d\n", r->code[pc], pc);
			assert(0);
			break;
		}
	}

	return (res);
}

/*
 * Return 1 if match, 0 if no match.
 * If `captured_substrings' array is not NULL, then it is filled with the
 * values of captured substrings. captured_substrings[0] element is always
 * a full matched substring. The round bracket captures start from
 * captured_substrings[1].
 * It is assumed that the size of captured_substrings array is enough to
 * hold all captures. The caller function must make sure it is! So, the
 * array_size = number_of_round_bracket_pairs + 1
 */
static int
slre_match(const struct slre *r, const char *buf, int len,
		struct cap *caps)
{
	int	i, ofs = 0, res = 0;

	if (r->anchored) {
		res = match(r, 0, buf, len, &ofs, caps);
	} else {
		for (i = 0; i < len && res == 0; i++) {
			ofs = i;
			res = match(r, 0, buf, len, &ofs, caps);
		}
	}

	return (res);
}

int wa_match(char* reg, char* txt, struct stResub *sub) {
  struct slre r;
  int ret = 0;
  if ( slre_compile(&r, reg) ) {
    struct cap *caps = (struct cap *)sub->sub;
    memset(caps, 0, sizeof(struct cap)*REG_MAXSUB);
    if (slre_match(&r, txt, strlen(txt), caps)) {
      int i = 0;
      for (; i<REG_MAXSUB; i++) {
        if (0==caps[i].len) {break;}
      }
      sub->nsub = i;
    } else {
      sub->nsub = 0;
      ret = -1;
    }
  } else {
    if (strstr(r.err_str, "bracket")) {
      ret = -11;
    } else if (strstr(r.err_str, " too ")) {
      ret = -12;
    } else {
      ret = -13;
    }
  }
  return ret;
}
