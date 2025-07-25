#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#define MAXLINE 256

#ifdef _WIN32
  #include <windows.h>
  #define DIR_SEP "\\"
  #define PATH_SEP ';'
#else
  #include <unistd.h>
  #define DIR_SEP "/"
  #define PATH_SEP ':'
#endif

#define SUPPORT_LUA
#ifdef SUPPORT_LUA
  #include "lua.h"
  #include "lauxlib.h"
  #include "lualib.h"
  int luaopen_utf8(lua_State *L);
  int luaopen_dt(lua_State *L);
  int luaopen_bit32(lua_State *L);
#ifdef USE_WALIB
  int luaopen_enc(lua_State *L);
#endif
#ifdef USE_VENDOR
  int luaopen_lsqlite3(lua_State *L);
  //int luaopen_lpeg (lua_State *L);
#endif
#endif

void usage(){
  printf("personal busybox %dbit ver250723\nascii\n"
  "dyn2str file -- convert script into C string file\n"
  "hsc helper show cvs\n  mf(list modified file)|ml(number modified line)|rv(repo version)\n"
  "snip|comp [keyword]\n"
  "xlispindent file|stdin\n"
#ifdef SUPPORT_LUA
  "el file [luac] -- convert lua to c code\n"
  "lua%d file [argv] or -e expr or -h; set PB_LUA_DEBUG see hook\n"
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

void get_exe_path(char* wd){
#ifdef _WIN32
  char *pos;
  GetModuleFileName(NULL, wd, MAXLINE);
  pos = strstr(wd, ".exe");
  *pos=0;
#else
  int ret = readlink("/proc/self/exe", wd, MAXLINE);
  wd[ret] = 0;
#endif
  strcat(wd, "_d"DIR_SEP);
}

int frontcmp(const char* s, const char* target, int most){
  int tlen = strlen(target);
  for(;most<=tlen;most++){
    if (0==strncmp(s,target,most)){return 0;}
  }
  return strncmp(s,target, tlen);
}

void err_pf(const char* format, ...){
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
}

/******** show ascii char ********/
void ascii(){
  int i = 32;
  for (; i<32+24; i++){
    printf("0x%x,%d: %c\t\t0x%x,%d: %c\t\t0x%x,%d: %c\t\t0x%x,%d: %c\n",
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

  err_pf("dyn2str to %s\n", buf);
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
            printf("%s", output);
            break;
        }
        //if this line is meanless, output all reserve string
        if(isspaceline(line)){
            printf("%s\n", output);
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
            f? indent_file(f): printf("cant open file %s\n", argv[i]);
        }
    }
}


/******** lua ********/
#if LUA_VERSION_NUM == 501
#define lua_rawlen  lua_objlen
#endif
#ifdef SUPPORT_LUA
static void _debug_lua(lua_State *L, char* hint_mess){
  int stk_size = lua_gettop(L), ri=-1, i, val_t;
  err_pf("[DEBUG %s]: elem num is %d\n", hint_mess, stk_size);
  for(i=stk_size;i>=1;i--){
    val_t = lua_type(L, i);
    char *idx_mean = "";
    if (i==stk_size){idx_mean = "\t<- top";} else if (1==i){idx_mean = "\t<- bot";}
    err_pf("  %d or %d: %s", i, ri, lua_typename(L, val_t));
    if (val_t==LUA_TSTRING) {err_pf(" %s", lua_tostring(L, i));}
    else if (val_t==LUA_TNUMBER) {err_pf(" %.2f", lua_tonumber(L, i));}
    else if (val_t==LUA_TTABLE) {int j=1;err_pf(" arrlen %d, key_5:", (int)lua_rawlen(L, i));
      lua_pushnil(L);
      for(;j<=5;j++){if (0==lua_next(L,i)){break;} else {
        val_t = lua_type(L, -2);
        if (val_t==LUA_TSTRING) {err_pf(" %s",lua_tostring(L,-2));}
        else if (val_t==LUA_TNUMBER) {err_pf(" %.0f",lua_tonumber(L,-2));}
        else {err_pf(" %s", lua_typename(L, lua_type(L, -2)));}
        lua_pop(L, 1);/* removes 'value'; keeps 'key' for next iteration */
        } }
      }
    else if (val_t==LUA_TBOOLEAN) {err_pf(" %s", 1==lua_toboolean(L, i)?"true":"false");}
    err_pf("%s\n", idx_mean);
    ri--;
  }
}
static void debug_lua(lua_State *L, char* hint_mess){
  if (getenv("PB_LUA_DEBUG") != NULL) { _debug_lua(L, hint_mess); }
}

static int _traceback (lua_State *L) {
  if (!lua_isstring(L, 1))  /* 'message' not a string? */
    return 1;  /* keep it intact */
  lua_getglobal(L, "debug");
  if (!lua_istable(L, -1)) {
    lua_pop(L, 1);
    return 1;
  }
  lua_getfield(L, -1, "traceback");
  if (!lua_isfunction(L, -1)) {
    lua_pop(L, 2);
    return 1;
  }
  lua_pushvalue(L, 1);  /* pass error message */
  lua_pushinteger(L, 2);  /* skip this function and traceback */
  lua_call(L, 2, 1);  /* call debug.traceback */
  return 1;
}

#include "lupt/pb_lua.c"
#include "lupt/fennel_lua.c"
#ifndef _PB_LUAFN_PB
static void luafn_pb(lua_State* L){puts("PB N/A");}
#endif
#ifndef _PB_LUAFN_FENNEL
static void luafn_fennel(lua_State* L){puts("FNL N/A");}
#endif
#endif

static void* linit(){
  void *L = NULL;
#ifdef SUPPORT_LUA
  L = (lua_State *)luaL_newstate();  /* create state */
  if (L == NULL) {
    puts("cannot create lua_State: not enough memory");
  } else {
    char s_lua_precode[MAXLINE]={0};
    char pb_d_path[MAXLINE];
    get_exe_path(pb_d_path);
    lua_gc(L, LUA_GCSTOP, 0);  /* stop collector during initialization */
    luaL_openlibs(L);  /* open libraries */
#if LUA_VERSION_NUM < 503
    luaopen_utf8(L);
#endif
    luaopen_dt (L);
    luaopen_bit32(L);
#ifdef USE_WALIB
    luaopen_enc(L);
#endif
#ifdef USE_VENDOR
    luaopen_lsqlite3(L);
#if LUA_VERSION_NUM > 501
    lua_setglobal(L, "sqlite3");
#endif
    //luaopen_lpeg(L);
#endif
    lua_gc(L, LUA_GCRESTART, 0);
    lua_pop(L, lua_gettop(L));  // pop custom open_lib
    luafn_pb(L);
    sprintf(s_lua_precode, "package.cpath=[[%s?.dll%c]]..package.cpath", pb_d_path, PATH_SEP);
    luaL_dostring(L, s_lua_precode);
  }
#endif
  return L;
}

static void _conv2lua(char *fname){
  char trans_name[96];
  char cmd[128];
  //transpiler_lua(fname, trans_name);
  sprintf(cmd, "mv %s %s", trans_name, fname);
  _cal_ext_fb2bb(cmd);
}

// bconv: 1-pylike conv lua 0-no conv
static int ldofile(void* L, char *fname, int narg, int bconv){
  int status = 0, base;
#ifdef SUPPORT_LUA
  //if (1==bconv){ _conv2lua(fname);}
  status = luaL_loadfile(L, fname);
  if (0!=status) {
    err_pf("pb load %s fail[%d]: %s\n", fname, status, lua_tostring(L, -1));
    lua_pop(L, lua_gettop(L));
    return status;
  }
  if (0<narg) {lua_insert(L, 1);}
  base = lua_gettop(L) - narg;  /* chunk function index */
  lua_pushcfunction(L, _traceback);  /* push traceback function */
  lua_insert(L, base);  /* put it under chunk and args */
  status = lua_pcall(L, narg, LUA_MULTRET, base); /*LUA_MULTRET*/
  if (0!=status) {
    err_pf("pb run %s fail[%d]: %s\n",fname, status, lua_tostring(L, -1));
  }
  lua_pop(L, lua_gettop(L));
#endif
  return status; /*0-ok 1-fail*/
}

static void lclose(void* L){
#ifdef SUPPORT_LUA
  lua_close((lua_State *)L);
#endif
}

void run_lua(int argc, char** argv){
#ifdef SUPPORT_LUA
  void *L = linit();
  if (2==argc) {
    char fname[MAXLINE];
    get_exe_path(fname);
    strcat(fname, "init.lua");
    ldofile(L, fname, 0, 0);
  } else if (0==strcmp(argv[2], "-e")) {
    char exprbuff[MAXLINE] = {0};
    int val_t, ret;
    if (3==argc){usage();}
    sprintf(exprbuff, "return %s", argv[3]);
    ret = luaL_dostring(L, exprbuff);
    val_t = lua_type(L, -1);
    if ((LUA_TSTRING==val_t) || (LUA_TNUMBER==val_t)) {
      puts(lua_tostring(L, -1));
    } else {
      printf("run %s, type: %s\n", ret==0?"ok":"fail", lua_typename(L, val_t));
    }
  } else if (0==strcmp(argv[2], "-h")) {
    puts("enhance with:\nfmt/fmtf; var_dump; ts; tie\n"
    "map/reduce/filter/range\n"
    "string.split/indexOf/replace/slice/trim; utf8.len/char/codes\n"
    "table.join/pop...; bit32.band...\n"
    "os.popen; dt.datediff/lsdir/lsfile; sqlite3\n"
    "enc.md5/sha1/btoa/atob; JSON.stringify/parse");
  } else {
    int i=2, j=0, fpos=2, bconv=0;
    lua_newtable(L);
    if ('i'==argv[1][1]) {j=1;} // fennel self on arg[0]
    //if (0==strcmp(argv[2], "-p")) {i=3;fpos=3;bconv=1;}
    for (; i < argc; i++) {
      lua_pushstring(L, argv[i]);
      lua_rawseti(L, -2, j++);
    }
    lua_setglobal(L, "arg");
    if ('i'==argv[1][1]){
      luafn_fennel(L);
    } else {
      ldofile(L, argv[fpos], 0, bconv);
    }
  }
  debug_lua(L, "exit lua");
  lclose(L);
#endif
}


static int lwriter (lua_State *L, const void* p, size_t sz, void* ud) {
  FILE* fp= (FILE*)ud;
  return fwrite(p, sz, 1, fp)==1?0:1;
}

static int _luac(char *fname){
#ifdef SUPPORT_LUA
  void *L = linit();
  FILE* fp = fopen("luac.out", "wb");
  luaL_loadfile(L, fname);
#if LUA_VERSION_NUM < 503
  if (0 != lua_dump(L, lwriter, fp)){
#else
  if (0 != lua_dump(L, lwriter, fp, 1)){
#endif
    err_pf("lua dump %s fail\n", fname);
  }
  fclose(fp);
  lclose(L);
#endif
  return 0;
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
  if (0!=ret){puts("luac fail");return;}

  sprintf(buf, "%s.c", argv[1]);
  pos = strchr(buf, '.');
  if (pos) {*pos = '_';}
  fr = fopen("luac.out", "rb");
  fw = fopen(buf, "w");
  printf("encode lua bytecode to %s\n", buf);
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
    "  else _debug_lua(L, \"err: buf_%s\");\n"
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
/*wh: 0-snip 1-comp*/
void lsnip(int argc, char** argv, int wh){
  char kwd[MAXLINE]={0};
  char fname[MAXLINE];
  char scname[16];
  char* snip_flag[2] = {"snip", "comp"};
  void *L = linit();
  int i=0;
  get_exe_path(fname);
  sprintf(scname, "_pb_%s", snip_flag[wh]);
  strcat(fname, scname);
  for (; i<argc; i++){
    strcat(kwd, argv[i]);strcat(kwd, " ");
  }
  luafn_snip(L);
  lua_pushstring(L, snip_flag[wh]);
  lua_pushstring(L, fname);
  lua_pushstring(L, kwd);
  lua_pcall(L, 3, LUA_MULTRET, 0);
}

int main(int argc, char** argv){
  int cmd = 0;
  char pp_txt[256] = {0};
  if (1<argc){cmd = argv[1][0];
  } else if (!isatty(fileno(stdin))) {
    if (fgets(pp_txt, sizeof(pp_txt), stdin)!=NULL) {
      cmd = pp_txt[0];
      printf("not fully support pipe, %s", pp_txt);
    } else {return 0;}
  } else {
    usage(); return 0;
  }
  // need the case -1, other -2
  switch(cmd){
    case 'a':
      ascii();
      break;
    case 'c':
      lsnip(argc-2, argv+2, 1);
      break;
    case 'd':
      dyn2str(argc-1, argv+1);
      break;
    case 'e':
      enc_lua(argc-1, argv+1);
      break;
    case 'h':
      help_show_csv(argc-1, argv+1);
      break;
    case 'l':
      run_lua(argc, argv);
      break;
    case 's':
      lsnip(argc-2, argv+2, 0);
      break;
    case 'x':
      xlispindent(argc-1, argv+1);
      break;
  default:
      usage();
      break;
  }
  return 0;
}

