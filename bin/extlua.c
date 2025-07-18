/*
** $Id: extlua.c,v 1.3 2025/04/21 16:16:58 u0_a157 Exp $
** Standard library for UTF-8 manipulation
** See Copyright Notice in lua.h
*/
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#if LUA_VERSION_NUM > 501
#define luaL_register(L,name,reg)  luaL_newlib(L,reg);lua_setglobal(L,name)
#endif

#if LUA_VERSION_NUM < 503
#define lutf8lib_c
#define LUA_LIB

typedef unsigned int  lua_Unsigned;
/* push the value used to represent failure/error */
#define luaL_pushfail(L)	lua_pushnil(L)

#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>


#define MAXUNICODE	0x10FFFFu
#define MAXUTF		0x7FFFFFFFu
#define MSGInvalid	"invalid UTF-8 code"

/*
** Integer type for decoded UTF-8 values; MAXUTF needs 31 bits.
*/
#if (UINT_MAX >> 30) >= 1
typedef unsigned int utfint;
#else
typedef unsigned long utfint;
#endif


#define iscont(c)	(((c) & 0xC0) == 0x80)
#define iscontp(p)	iscont(*(p))


/* from strlib */
/* translate a relative string position: negative means back from end */
static lua_Integer u_posrelat (lua_Integer pos, size_t len) {
  if (pos >= 0) return pos;
  else if (0u - (size_t)pos > len) return 0;
  else return (lua_Integer)len + pos + 1;
}


/*
** Decode one UTF-8 sequence, returning NULL if byte sequence is
** invalid.  The array 'limits' stores the minimum value for each
** sequence length, to check for overlong representations. Its first
** entry forces an error for non-ascii bytes with no continuation
** bytes (count == 0).
*/
static const char *utf8_decode (const char *s, utfint *val, int strict) {
  static const utfint limits[] =
        {~(utfint)0, 0x80, 0x800, 0x10000u, 0x200000u, 0x4000000u};
  unsigned int c = (unsigned char)s[0];
  utfint res = 0;  /* final result */
  if (c < 0x80)  /* ascii? */
    res = c;
  else {
    int count = 0;  /* to count number of continuation bytes */
    for (; c & 0x40; c <<= 1) {  /* while it needs continuation bytes... */
      unsigned int cc = (unsigned char)s[++count];  /* read next byte */
      if (!iscont(cc))  /* not a continuation byte? */
        return NULL;  /* invalid byte sequence */
      res = (res << 6) | (cc & 0x3F);  /* add lower 6 bits from cont. byte */
    }
    res |= ((utfint)(c & 0x7F) << (count * 5));  /* add first byte */
    if (count > 5 || res > MAXUTF || res < limits[count])
      return NULL;  /* invalid byte sequence */
    s += count;  /* skip continuation bytes read */
  }
  if (strict) {
    /* check for invalid code points; too large or surrogates */
    if (res > MAXUNICODE || (0xD800u <= res && res <= 0xDFFFu))
      return NULL;
  }
  if (val) *val = res;
  return s + 1;  /* +1 to include first byte */
}


/*
** utf8len(s [, i [, j [, lax]]]) --> number of characters that
** start in the range [i,j], or nil + current position if 's' is not
** well formed in that interval
*/
static int utflen (lua_State *L) {
  lua_Integer n = 0;  /* counter for the number of characters */
  size_t len;  /* string length in bytes */
  const char *s = luaL_checklstring(L, 1, &len);
  lua_Integer posi = u_posrelat(luaL_optinteger(L, 2, 1), len);
  lua_Integer posj = u_posrelat(luaL_optinteger(L, 3, -1), len);
  int lax = lua_toboolean(L, 4);
  luaL_argcheck(L, 1 <= posi && --posi <= (lua_Integer)len, 2,
                   "initial position out of bounds");
  luaL_argcheck(L, --posj < (lua_Integer)len, 3,
                   "final position out of bounds");
  while (posi <= posj) {
    const char *s1 = utf8_decode(s + posi, NULL, !lax);
    if (s1 == NULL) {  /* conversion error? */
      luaL_pushfail(L);  /* return fail ... */
      lua_pushinteger(L, posi + 1);  /* ... and current position */
      return 2;
    }
    posi = s1 - s;
    n++;
  }
  lua_pushinteger(L, n);
  return 1;
}


/*
** codepoint(s, [i, [j [, lax]]]) -> returns codepoints for all
** characters that start in the range [i,j]
*/
static int codepoint (lua_State *L) {
  size_t len;
  const char *s = luaL_checklstring(L, 1, &len);
  lua_Integer posi = u_posrelat(luaL_optinteger(L, 2, 1), len);
  lua_Integer pose = u_posrelat(luaL_optinteger(L, 3, posi), len);
  int lax = lua_toboolean(L, 4);
  int n;
  const char *se;
  luaL_argcheck(L, posi >= 1, 2, "out of bounds");
  luaL_argcheck(L, pose <= (lua_Integer)len, 3, "out of bounds");
  if (posi > pose) return 0;  /* empty interval; return no values */
  if (pose - posi >= INT_MAX)  /* (lua_Integer -> int) overflow? */
    return luaL_error(L, "string slice too long");
  n = (int)(pose -  posi) + 1;  /* upper bound for number of returns */
  luaL_checkstack(L, n, "string slice too long");
  n = 0;  /* count the number of returns */
  se = s + pose;  /* string end */
  for (s += posi - 1; s < se;) {
    utfint code;
    s = utf8_decode(s, &code, !lax);
    if (s == NULL)
      return luaL_error(L, MSGInvalid);
    lua_pushinteger(L, code);
    n++;
  }
  return n;
}


void codepoint_to_utf8(lua_Unsigned codepoint, char *utf8) {
    if (codepoint <= 0x7F) {
        utf8[0] = (char)codepoint;
        utf8[1] = '\0';
    } else if (codepoint <= 0x7FF) {
        utf8[0] = (char)(0xC0 | (codepoint >> 6));
        utf8[1] = (char)(0x80 | (codepoint & 0x3F));
        utf8[2] = '\0';
    } else if (codepoint <= 0xFFFF) {
        utf8[0] = (char)(0xE0 | (codepoint >> 12));
        utf8[1] = (char)(0x80 | ((codepoint >> 6) & 0x3F));
        utf8[2] = (char)(0x80 | (codepoint & 0x3F));
        utf8[3] = '\0';
    } else if (codepoint <= 0x10FFFF) {
        utf8[0] = (char)(0xF0 | (codepoint >> 18));
        utf8[1] = (char)(0x80 | ((codepoint >> 12) & 0x3F));
        utf8[2] = (char)(0x80 | ((codepoint >> 6) & 0x3F));
        utf8[3] = (char)(0x80 | (codepoint & 0x3F));
        utf8[4] = '\0';
    }
}

static void pushutfchar (lua_State *L, int arg) {
  char utf8str[5] = {0};
  lua_Unsigned code = (lua_Unsigned)luaL_checkinteger(L, arg);
  luaL_argcheck(L, code <= MAXUTF, arg, "value out of range");
  codepoint_to_utf8(code, utf8str);
  lua_pushstring(L, utf8str);
}


/*
** utfchar(n1, n2, ...)  -> char(n1)..char(n2)...
*/
static int utfchar (lua_State *L) {
  int n = lua_gettop(L);  /* number of arguments */
  if (n == 1)  /* optimize common case of single char */
    pushutfchar(L, 1);
  else {
    int i;
    luaL_Buffer b;
    luaL_buffinit(L, &b);
    for (i = 1; i <= n; i++) {
      pushutfchar(L, i);
      luaL_addvalue(&b);
    }
    luaL_pushresult(&b);
  }
  return 1;
}


/*
** offset(s, n, [i])  -> index where n-th character counting from
**   position 'i' starts; 0 means character at 'i'.
*/
static int byteoffset (lua_State *L) {
  size_t len;
  const char *s = luaL_checklstring(L, 1, &len);
  lua_Integer n  = luaL_checkinteger(L, 2);
  lua_Integer posi = (n >= 0) ? 1 : len + 1;
  posi = u_posrelat(luaL_optinteger(L, 3, posi), len);
  luaL_argcheck(L, 1 <= posi && --posi <= (lua_Integer)len, 3,
                   "position out of bounds");
  if (n == 0) {
    /* find beginning of current byte sequence */
    while (posi > 0 && iscontp(s + posi)) posi--;
  }
  else {
    if (iscontp(s + posi))
      return luaL_error(L, "initial position is a continuation byte");
    if (n < 0) {
       while (n < 0 && posi > 0) {  /* move back */
         do {  /* find beginning of previous character */
           posi--;
         } while (posi > 0 && iscontp(s + posi));
         n++;
       }
     }
     else {
       n--;  /* do not move for 1st character */
       while (n > 0 && posi < (lua_Integer)len) {
         do {  /* find beginning of next character */
           posi++;
         } while (iscontp(s + posi));  /* (cannot pass final '\0') */
         n--;
       }
     }
  }
  if (n == 0)  /* did it find given character? */
    lua_pushinteger(L, posi + 1);
  else  /* no such character */
    luaL_pushfail(L);
  return 1;
}


static int iter_aux (lua_State *L, int strict) {
  size_t len;
  const char *s = luaL_checklstring(L, 1, &len);
  lua_Unsigned n = (lua_Unsigned)lua_tointeger(L, 2);
  if (n < len) {
    while (iscontp(s + n)) n++;  /* go to next character */
  }
  if (n >= len)  /* (also handles original 'n' being negative) */
    return 0;  /* no more codepoints */
  else {
    utfint code;
    const char *next = utf8_decode(s + n, &code, strict);
    if (next == NULL || iscontp(next))
      return luaL_error(L, MSGInvalid);
    lua_pushinteger(L, n + 1);
    lua_pushinteger(L, code);
    return 2;
  }
}


static int iter_auxstrict (lua_State *L) {
  return iter_aux(L, 1);
}

static int iter_auxlax (lua_State *L) {
  return iter_aux(L, 0);
}


static int iter_codes (lua_State *L) {
  int lax = lua_toboolean(L, 2);
  const char *s = luaL_checkstring(L, 1);
  luaL_argcheck(L, !iscontp(s), 1, MSGInvalid);
  lua_pushcfunction(L, lax ? iter_auxlax : iter_auxstrict);
  lua_pushvalue(L, 1);
  lua_pushinteger(L, 0);
  return 3;
}


/* pattern to match a single UTF-8 character */
#define UTF8PATT	"[0-7F|C2-FD][80-BF]*"


static const luaL_Reg utf8_funcs[] = {
  {"offset", byteoffset},
  {"codepoint", codepoint},
  {"char", utfchar},
  {"len", utflen},
  {"codes", iter_codes},
  /* placeholders */
  {"charpattern", NULL},
  {NULL, NULL}
};


LUALIB_API int luaopen_utf8 (lua_State *L) {
  luaL_register(L, "utf8", utf8_funcs);
  lua_pushlstring(L, UTF8PATT, sizeof(UTF8PATT)/sizeof(char) - 1);
  lua_setfield(L, -2, "charpattern");
  return 1;
}
#endif


/********enc********/
#ifdef USE_WALIB
#include "walib.h"

static int md5 (lua_State *L) {
  const char *src = luaL_checkstring(L, 1);
  char dst[33] = {0};
  wa_md5((char*)src, dst);
  lua_pushstring(L, dst);
  return 1;
}

static int sha1 (lua_State *L) {
  const char *src = luaL_checkstring(L, 1);
  char dst[41] = {0};
  wa_sha1((char*)src, dst);
  lua_pushstring(L, dst);
  return 1;
}

static int b64enc (lua_State *L) {
  size_t len;
  const char *src = lua_tolstring(L, 1, &len);
  char* dst = malloc((len/3+1)*4+1);
  wa_base64enc((char*)src, dst);
  lua_pushstring(L, dst);
  free(dst);
  return 1;
}

static int b64dec (lua_State *L) {
  size_t len;
  const char *src = lua_tolstring(L, 1, &len);
  char* dst = malloc((len/4+1)*3+1);
  int ret = wa_base64dec((char*)src, dst);
  if (0==ret) {
    lua_pushstring(L, dst);
  } else {
    lua_pushnil(L);
  }
  free(dst);
  return 1;
}

static const luaL_Reg enc_funcs[] = {
  {"md5", md5},
  {"sha1", sha1},
  {"btoa", b64enc},
  {"atob", b64dec},
  {NULL, NULL}
};

LUALIB_API int luaopen_enc (lua_State *L) {
  luaL_register(L, "enc", enc_funcs);
  return 1;
}
#endif

/********dt********/
#ifdef USE_WALIB
static int datediff (lua_State *L) {
  const char *from_day = luaL_checkstring(L, 1);
  const char *to_day = luaL_checkstring(L, 2);
  lua_pushinteger(L, wa_datediff(from_day, to_day));
  return 1;
}
#endif

#include <dirent.h>
#include <sys/stat.h>

static int _scan_dir_file (lua_State *L, int is_dir) { //dir 1, otherwise 0
  struct dirent *entry;
  struct stat v_st;
  DIR *dirp;
  int i = 1, need_full;
  const char *src_d;
  char full_name[1024];

  src_d = luaL_optstring(L, 1, ".");
  need_full = lua_toboolean(L, 2);
  dirp = opendir(src_d);
  if (dirp==NULL){lua_pushnil(L);return 1;}

  lua_newtable(L);
  while(i){
    entry = readdir(dirp);
    // deal EOD . and ..
    if (entry == NULL) {break;}
    if (!strncmp(entry->d_name, ".", 1) || !strncmp(entry->d_name, "..", 2)) {
      continue;
    }
    stat(entry->d_name, &v_st);
    if ((1==is_dir&&v_st.st_mode&0x4000) || (0==is_dir&&(0==(v_st.st_mode&0x4000)))) {
      if (1==need_full){
        sprintf(full_name, "%s/%s", src_d, entry->d_name);
        lua_pushstring(L, full_name);
      } else {
        lua_pushstring(L, entry->d_name);
      }
      lua_rawseti(L, -2, i++);
    } else {
      continue;
    }
  }
  return 1;
}

static int lsdir (lua_State *L) {
  return _scan_dir_file(L, 1);
}

static int lsfile (lua_State *L) {
  return _scan_dir_file(L, 0);
}

static const luaL_Reg dt_funcs[] = {
#ifdef USE_WALIB
  {"datediff", datediff},
#endif
  {"lsdir", lsdir},
  {"lsfile", lsfile},
  {NULL, NULL}
};


LUALIB_API int luaopen_dt (lua_State *L) {
  luaL_register(L, "dt", dt_funcs);
  return 1;
}

/********bit32********/
static int band (lua_State *L) {
  int i, n=lua_gettop(L), r=0xFFFFFFFF;
  for (i=1; i<=n; i++){r &= lua_tointeger(L, i);}
  lua_pushinteger(L, r);
  return 1;
}

static int bor (lua_State *L) {
  int i, n=lua_gettop(L), r=0;
  for (i=1; i<=n; i++){r |= lua_tointeger(L, i);}
  lua_pushinteger(L, r);
  return 1;
}

static int bxor (lua_State *L) {
  int i, n=lua_gettop(L), r=0;
  for (i=1; i<=n; i++){r ^= lua_tointeger(L, i);}
  lua_pushinteger(L, r);
  return 1;
}

static const luaL_Reg bit32_funcs[] = {
  {"band", band},
  {"bor",  bor},
  {"bxor", bxor},
  {NULL, NULL}
};


LUALIB_API int luaopen_bit32 (lua_State *L) {
  luaL_register(L, "bit32", bit32_funcs);
  return 1;
}

