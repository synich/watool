#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include "walib.h"
#define MAXLINE 256

#define SUPPORT_LUA
#ifdef SUPPORT_LUA
  #include "lua.h"
  #include "lauxlib.h"
  #include "lualib.h"

#ifdef USE_VENDOR
  int luaopen_lsqlite3(lua_State *L);
  //int luaopen_lpeg (lua_State *L);
#endif
#endif

void usage(){
  wa_prtcs("personal busybox %dbit ver260625\nascii\n"
  "dyn2str file -- convert script into C string file\n"
  "hsc helper show cvs\n  mf(list modified file)|ml(number modified line)|rv(repo version)\n"
  "snip [keyword] -- {pb}/pb_d/_pb_snip[0-9]\n"
  "xlispindent file|stdin\n"
#ifdef SUPPORT_LUA
  "el file [luac] -- convert lua to c code\n"
  "lua%d file [argv] or -e expr or -h; set PB_DEBUG see hook\n"
  "lisp file [argv]\n"
#endif
  , (int)(8*sizeof(void*))
#ifdef SUPPORT_LUA
  , LUA_VERSION_NUM
#endif
  );
  exit(0);
}


/******** util ********/
static void _cal_ext_fb2bb(char* cmd){
  int has_bb, ret;
#ifdef _WIN32
  has_bb = system("busybox >NUL 2>NUL");
#else
  has_bb = system("sh busybox 1>/dev/null 2>/dev/null");
#endif
  if (0==has_bb){
    char bbcmd[MAXLINE];
    sprintf(bbcmd, "busybox %s", cmd);
    ret = system(bbcmd);
  } else {ret = system(cmd);}
  if (0!=ret){puts("no busybox or native util found, cmd fail");}
}

int frontcmp(const char* s, const char* target, int most){
  int tlen = strlen(target);
  for(;most<=tlen;most++){
    if (0==strncmp(s,target,most)){return 0;}
  }
  return strncmp(s,target, tlen);
}

/******** show ascii char ********/
void ascii(){
  int i = 32;
  wa_prtcs("Control:\n");
  wa_prtcs("0x00,0: NUL\t0x08,8: Backspace\t0x10,16: DataLnkEscape\t0x18,24: Cancel\n");
  wa_prtcs("0x01,1: SOH\t0x09,9: Tab\t\t0x11,17: DC1-XON\t0x19,25: EndMedium\n");
  wa_prtcs("0x02,2: STX\t0x0A,10: LF\t\t0x12,18: DC2\t\t0x1A,26: Substitute\n");
  wa_prtcs("0x03,3: ETX\t0x0B,11: VTab\t\t0x13,19: DC3-XOFF\t0x1B,27: Escape\n");
  wa_prtcs("0x04,4: EOT\t0x0C,12: ClearScreen\t0x14,20: DC4\t\t0x1C,28: FileSplit\n");
  wa_prtcs("0x05,5: Enquiry\t0x0D,13: CR\t\t0x15,21: NAK\t\t0x1D,29: GrpSplit\n");
  wa_prtcs("0x06,6: ACK\t0x0E,14: ShiftOut\t0x16,22: SYN\t\t0x1E,30: RecSplit\n");
  wa_prtcs("0x07,7: Bell\t0x0F,15: ShiftIn\t0x17,23: EndTransBlk\t0x1F,31: UnitSplit\n");
  wa_prtcs("\nPrintable:\n");
  for (; i<32+24; i++){
    wa_prtcs("0x%x,%d: %c\t\t0x%x,%d: %c\t\t0x%x,%d: %c\t\t0x%x,%d: %c\n",
      i, i, i, i+24, i+24, i+24, i+48, i+48, i+48, i+72, i+72, i+72);
  }
}

/******** help_show_csv ********/
void help_show_csv(int argc, char** argv){
#define MODFILE "mf"
#define MODLINE "ml"
#define REPOVER "rv"
  if (1==argc){
    usage();
  } else {
    if (0==strcmp(argv[1], MODFILE)) {
      system("cvs st | grep Modified");
    } else if (0==strcmp(argv[1], MODLINE)) {
      system("cvs diff | grep \"^>\" | wc -l");
     } else if (0==strcmp(argv[1], REPOVER)) {
      system("cvs st | grep Repository");
   } else {
      puts("Unknown command, see help without argument.");
    }
  }
}


/******** dyn2str ********/
static void convFile(unsigned char* str, int slen, FILE* fout){
  int i=0;
  char buf[8] = {0};
  for (; i<slen; i++) {
    if (0 == i%16){
      fwrite("\n", 1, 1, fout);
    }
    snprintf(buf, sizeof(buf), "%u,", str[i]);
    fwrite(buf, strlen(buf), 1, fout);
  }
}

/* Convert script language source to C style string.
   So you can embed source into binary file, not confuse just combine.*/
void dyn2str(int argc, char *argv[])
{
  FILE *fin = NULL, *fout = NULL;
  char buf[128] = {0};
  char *pos = NULL;
  unsigned char *chin = NULL;
  int insize = 0;

  if (argc <= 1) {
    usage();
  }

  /* if input is script.ext, output file name is script_ext.c */
  snprintf(buf, sizeof(buf), "%s.c", argv[1]);
  pos = strchr(buf, '.');
  *pos = '_'; /*At least has one .c, must available*/

  wa_prtcs("dyn2str to %s\n", buf);
  fin = fopen(argv[1], "r");
  fout = fopen(buf, "w");
  fseek(fin, 0, SEEK_END);
  insize = ftell(fin);
  rewind(fin);

  /* write C string declare */
  snprintf(buf, sizeof(buf), "static const unsigned char s_%s_str[]={", argv[1]);
  chin = (unsigned char*)strchr(buf, '.');
  if (chin) {*chin = '_';} /*Origin file no extname, dont overwrite here*/
  fwrite(buf, strlen(buf), 1, fout);

  /* write script content */
  chin = (unsigned char*)calloc(1, insize + 2);
  insize = fread(chin, sizeof(char), insize, fin);
  convFile(chin, insize, fout);

  /* write C string end mark */
  snprintf(buf, sizeof(buf), "0};\n");
  fwrite(buf, strlen(buf), 1, fout);

  free(chin);
  fclose(fin);
  fclose(fout);
}


/******** xlispindent ********/
int isspaceline(char* buf){
    int i=0;
    for(; i<strlen(buf); ++i){
        char c = buf[i];
        if(' ' != c && '\t' != c && '\n' != c && '\r' != c)
            return 0;
    }
    return 1;
}

typedef struct _lisp_state{
    int lonely;  //num of lonely '('
    int offset;  //use for 'if' or 'cond' etc.
}lisp_state;

/* caculate state of line, return non-space position */
char* cacu_pathen(char* buf, lisp_state* st){
    int left = 0, right = 0, i = 0;
    char *pfirst = NULL, *tmp = NULL;
    while(' '==buf[i] || '\t'==buf[i]) ++i;
    pfirst = &buf[i];
    tmp = pfirst;
    while (*tmp != 0){
        if('('==*tmp){++left;}
        else if(')'==*tmp){++right;}
        ++tmp;
    }
    st->lonely = left-right;
    return pfirst;
}
#define ALLW "                                                                 "
void indent_file(FILE* file){
    char output[8192] = {0};
    lisp_state last_state={0}, line_state={0};
    while (1){
        char line[120] = {0};
        if (NULL == fgets(line, sizeof(line), file)){
            wa_prtcs("%s", output);
            break;
        }
        //if this line is meanless, output all reserve string
        if(isspaceline(line)){
            wa_prtcs("%s\n", output);
            memset(output, 0, sizeof(output));
        }
        else{
            int end = 0;
            char* pfirst = cacu_pathen(line, &line_state);
            //if line is begin with ')", append it to last line
            if(')'==*pfirst){
                char* pend = output + strlen(output);
                while('\r'==*(pend-1) || '\n'==*(pend-1))
                    *(--pend) = 0;
                goto APPEND;
            }
            strcat(output, ALLW);
            end = strlen(output) - strlen(ALLW) + 2 * last_state.lonely;
            output[end] = 0;
APPEND:
            strcat(output, pfirst);
            last_state.lonely += line_state.lonely;
        }
    }
}

void xlispindent(int argc, char** argv){
    if (argc==1) {
        /* If input is from pipe, use stdin as FILE* */
        indent_file(stdin);
    } else {
        int i=1;
        for(; i<argc; ++i){
            FILE* f = fopen(argv[i], "r");
            f? indent_file(f): wa_prtcs("cant open file %s\n", argv[i]);
        }
    }
}


/******** lua ********/
#ifdef SUPPORT_LUA
static void debug_lua(lua_State *L, char* hint_mess){
  if (getenv("PB_DEBUG") != NULL) { wa_debug_lua(L, hint_mess); }
}

void* linit(){
  void* p = (lua_State*)wa_linit();
#ifdef USE_VENDOR
    lua_State* L = (lua_State*)p;
    luaopen_lsqlite3(L);
  #if LUA_VERSION_NUM > 501
    lua_setglobal(L, "sqlite3");
  #endif
    //luaopen_lpeg(L);
  #if LUA_VERSION_NUM > 501
    //lua_setglobal(L, "lpeg");
  #endif
#endif
  return p;
}
#endif

static void _conv2lua(char *fname){
  char trans_name[96];
  char cmd[128];
  //transpiler_lua(fname, trans_name);
  sprintf(cmd, "mv %s %s", trans_name, fname);
  _cal_ext_fb2bb(cmd);
}

static void _lua_expr(lua_State *L, int argc, char** argv){
  char exprbuff[MAXLINE] = {0};
  int val_t, ret;
  if (2==argc){usage();}
  sprintf(exprbuff, "return %s", argv[2]);
  ret = luaL_dostring(L, exprbuff);
  wa_prtcs("run %s\n", ret==0?"ok":"fail");
  wa_debug_lua(L, "Expr");
}

static void _lua_help(lua_State *L){
  lua_getglobal(L, "pb");
  lua_getfield(L, -1, "__doc__");
  puts(lua_tostring(L, -1));
}

#include "lupt/fennel_lua.c"
#ifndef _PB_LUAFN_FENNEL
static void luafn_fennel(lua_State* L){puts("FNL N/A");}
#endif

static void _lua_dofile(lua_State* L, int argc, char** argv){
  int i=1, j=0;
  lua_newtable(L);
  if ('i'==argv[0][1]) {j=1;} // fennel self on arg[0]
  int count = 0;
  for (; i < argc; i++) {
    lua_pushstring(L, argv[i]);
    lua_rawseti(L, -2, j++);
    count++;
  }
  lua_setglobal(L, "arg");
  for (i=1; i<argc; i++) {
    lua_pushstring(L, argv[i]);
  }
  if ('i'==argv[0][1]){ /*lisp*/
    luafn_fennel(L);
  } else {
    wa_ldofile(L, argv[1], count);
  }
}

void run_lua(int argc, char** argv){
#ifdef SUPPORT_LUA
  void *L = linit();
  if (1==argc) {
    char fname[MAXLINE];
    wa_get_exe_path(fname);
    strcat(fname, "init.lua");
    wa_ldofile(L, fname, 0);
  } else {
    if ('i'==argv[0][1]){ /*lisp*/
      _lua_dofile(L, argc, argv);
    } else { /*lua*/
      if (0==strcmp(argv[1], "-e")) {
        _lua_expr(L, argc, argv);
      } else if (0==strcmp(argv[1], "-h")) {
        _lua_help(L);
      } else {
        _lua_dofile(L, argc, argv);
      }
    }
  }
  debug_lua(L, "exit lua");
  wa_lclose(L);
#endif
}


static int lwriter (lua_State *L, const void* p, size_t sz, void* ud) {
  FILE* fp= (FILE*)ud;
  return (fwrite(p, sz, 1, fp)!=1)&&(sz!=0);
}

static int _luac(char *fname){
#ifdef SUPPORT_LUA
  void *L = linit();
  FILE* fp = fopen("luac.out", "wb");
  int ret = 0;
  luaL_loadfile(L, fname);
#if LUA_VERSION_NUM < 503
  ret = lua_dump(L, lwriter, fp);
#else
  ret = lua_dump(L, lwriter, fp, 1);
#endif
  if (0 != ret) {
    wa_prtcs("lua dump %s fail, code: %d. see more with PB_DEBUG\n", fname, ret);
    debug_lua(L, "dump fail");
  }
  fclose(fp);
  wa_lclose(L);
#endif
  return ret;
}

void enc_lua(int argc, char *argv[])
{
  char buf[MAXLINE] = {0}, flname[128]={0};
  char *pos, *fl_pos;
  int i = 0, ret;
  char fndecl[MAXLINE] = {0}, ma_upp[16]={0};
  FILE *fw, *fr;

  if (argc <2){
    usage();
  }

  if (2==argc) {
    ret = _luac(argv[1]);
  } else {
    sprintf(buf, "%s -s %s", argv[2], argv[1]);/*drop debug*/
    ret = system(buf);
  }
  if (0!=ret){return;}

  sprintf(buf, "%s.c", argv[1]);
  pos = strchr(buf, '.');
  if (pos) {*pos = '_';}
  fr = fopen("luac.out", "rb");
  fw = fopen(buf, "w");
  wa_prtcs("encode lua bytecode to %s\n", buf);
  fl_pos = strrchr(buf, '/')?strrchr(buf, '/'):strrchr(buf, '\\');
  if (NULL==fl_pos){strcpy(flname, buf);}
  else {strcpy(flname, fl_pos+1);}
  fl_pos = strchr(flname, '_');
  if (fl_pos){*fl_pos = 0;}

  *pos = 0; // let buf be the lua module name
  for (i=0;i<strlen(flname);i++) {
    ma_upp[i] = toupper(flname[i]);}
  i = 0;
  sprintf(fndecl, "#define _PB_LUAFN_%s\n\n"
    "static void luafn_%s(lua_State* L) {\n"
    "  const unsigned char B1[]={", ma_upp, flname);
  fwrite(fndecl, strlen(fndecl), 1, fw);

  while(1){
#define comma    ","
#define nline    "\n    "
    unsigned char ch;
    char fch[4] = {0};
    int cnt = fread(&ch, 1, 1, fr);
    if (cnt == 0) {break;}
    else if ( (i++)%16 == 0) {fwrite(nline, 5, 1, fw);}
    sprintf(fch, "%u", ch); // convert to string
    fwrite(&fch, strlen(fch), 1, fw);
    fwrite(comma, 1, 1, fw);
    //if (feof(fr)) break;
  }
  /*lua_call keep result and set global, or 1->0 then not set*/
  snprintf(fndecl, MAXLINE, "\n  };\n\n  if (luaL_loadbuffer"
    "(L,(const char*)B1,sizeof(B1),\"buf_%s\")==0)\n"
    "    lua_call(L, 0, LUA_MULTRET);\n"
    "  else wa_debug_lua(L, \"err: buf_%s\");\n"
    "  //lua_setglobal(L, \"%s\");\n"
    "}\n", flname, flname, flname);
  fwrite(fndecl, strlen(fndecl), 1, fw);
  fclose(fr);
  fclose(fw);
}

/******** snip ********/
#include "lupt/snip_lua.c"
#ifndef _PB_LUAFN_SNIP
static void luafn_snip(lua_State* L){puts("SNIP N/A");}
#endif

void lsnip(int argc, char** argv){
  char kwd[MAXLINE]={0};
  char fname[MAXLINE];
  char scname[16];
  char* snip_flag = "snip";
  void *L = linit();
  int i=1;
  wa_get_exe_path(fname);
  sprintf(scname, "_pb_%s", snip_flag);
  strcat(fname, scname);
  for (; i<argc; i++){
    strcat(kwd, argv[i]);strcat(kwd, " ");
  }
  luafn_snip(L);
  lua_pushstring(L, snip_flag);
  lua_pushstring(L, fname);
  lua_pushstring(L, kwd);
  if (0 != lua_pcall(L, 3, LUA_MULTRET, 0)){
    puts("snip fail, see more with PB_DEBUG"); debug_lua(L, "snip");
  }
}

/************** main entry *********************/
/** split with spaces 0:ok -1:fail
 * @param input   string, will be modified
 * - spaces as one space, ignore leading and trailing space
 * - argv[0] is also arg, not like main argv[0]  */
int split_args(char *input, int *argc, char ***argv) {
    if (!input || !argc || !argv) { return -1; }

    *argc = 0;
    *argv = NULL;
    if (input[0] == '\0') { return 0; }

    // first time, count
    int count = 0;
    char *p = input;
    int in_token = 0;
    while (*p) {
        if (!isspace((unsigned char)*p)) {
            if (!in_token) {
                count++;
                in_token = 1;
            }
        } else {
            in_token = 0;
        }
        p++;
    }
    if (count == 0) { return 0; }

    // one more argv end with NULL
    char **result = malloc((count + 1) * sizeof(char *));
    if (!result) { return -1; }
    result[count] = NULL;

    // second time fill pointer
    int idx = 0;
    p = input;
    in_token = 0;
    char *token_start = NULL;

    while (*p) {
        if (!isspace((unsigned char)*p)) {
            if (!in_token) {
                token_start = p;
                in_token = 1;
            }
        } else {
            if (in_token) {
                *p = '\0';
                result[idx++] = token_start;
                in_token = 0;
            }
        }
        p++;
    }

    // last token(not end with space)
    if (in_token) {
        result[idx++] = token_start;
    }
    *argc = count;
    *argv = result;
    return 0;
}

void cli_entry(int argc, char** argv){
  int cmd = (argc==0 ?0 :argv[0][0]);
  switch(cmd){
    case 'a':
      ascii();
      break;
    case 'd':
      dyn2str(argc, argv);
      break;
    case 'e':
      enc_lua(argc, argv);
      break;
    case 'h':
      help_show_csv(argc, argv);
      break;
    case 'l':
      run_lua(argc, argv);
      break;
    case 's':
      lsnip(argc, argv);
      break;
    case 'x':
      xlispindent(argc, argv);
      break;
  default:
      usage();
      break;
  }
}

void pipe_entry(char* pp_txt){
  int prgc;
  char** prgv;
  if (0==split_args(pp_txt, &prgc, &prgv)){
    cli_entry(prgc, prgv);
    free(prgv);
  } else {puts("can not parse args");}
}

#define PB_MAIN
#ifdef PB_MAIN
int main(int argc, char** argv){
  if (argc > 1) { /*call by cli*/
    cli_entry(argc-1, argv+1);
  } else {
    if (!isatty(fileno(stdin))) { /*call by pipe*/
      char pp_txt[256] = {0};
      if (fgets(pp_txt, sizeof(pp_txt), stdin)!=NULL) {
        pipe_entry(pp_txt);
      }
    } else {
      usage();
    }
  }
  return 0;
}
#endif

