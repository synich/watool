#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include "walib.h"
#define SUPPORT_LUA
#ifdef SUPPORT_LUA
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
int luaopen_utf8(lua_State *L);
#endif

#define MAXLINE 256

void usage(){
  puts("personal busybox ver230625\nascii\n"
  "dyn2str file -- convert script into C string file\n"
#ifdef SUPPORT_LUA
  "lua|el file [argv]\n"
#endif
  "snip|comp [keyword]\n"
  "todo sth|[d line]\n"
  "hsc helper show cvs\n\tmf(list modified file)|ml(number modified line)|rv(repo version)\n"
  "wph(WaProjectHelper)\n\twph c c -- create c project scafold\n\twph l p -- generate lua plugin by c\n"
  "xlispindent file|stdin"
  );
  exit(0);
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
  for (; i<32+24; i++){
    printf("0x%x,%d: %c\t\t0x%x,%d: %c\t\t0x%x,%d: %c\t\t0x%x,%d: %c\n",
      i, i, i, i+24, i+24, i+24, i+48, i+48, i+48, i+72, i+72, i+72);
  }
}


/******** snip ********/
void get_exe_path(char* wd){
#ifdef _WIN32
  char *pos;
  GetModuleFileName(NULL, wd, MAXLINE);
  pos = strstr(wd, ".exe");
  *pos=0;
  strcat(wd, "_d\\");
#else
  int ret = readlink("/proc/self/exe", wd, MAXLINE);
  wd[ret] = 0;
  strcat(wd, "_d/");
#endif
}

/*wh: 0-title 1-para
  to: 0-stdout 1-_pb_out */
static void get_paragraph(const char* fname, int wh, const char* kwd, int to){
  FILE *f = fopen(fname, "r");
  FILE *fout = stdout;
  if (1==to) {
    char fop[MAXLINE];
    get_exe_path(fop);
    strcat(fop, "_pb_out");
    fout = fopen(fop, "w");
  }
  if (NULL==f){printf("cant find '%s'\n",fname);return;}
  char line[MAXLINE];
  int fl_len = strlen(kwd);
  int out_flag = 0;
  while(fgets(line, MAXLINE, f)){
    if (0==wh){
      if (0==strncmp(line, kwd, fl_len)){
        char *pos = strchr(line, '_');
        fprintf(fout, "%s", pos+1);
      }
    }else{
      if (out_flag==1){
        if (0==strncmp(line, "@@", 2)) {
          break;
        } else {
          fprintf(fout, "%s", line);
        }
      } else if (0==strncmp(line, kwd, fl_len)){
        out_flag = 1;
      }
    }
  }
  fclose(f);
  if (1==to) {fclose(fout);}
}

/*wh: 0-snip 1-comp*/
void snip(int argc, char** argv, int wh, int to){
  char fl_str[MAXLINE];
  char fname[MAXLINE];
  char* snip_flag[2] = {"snip", "comp"};
  sprintf(fl_str, "# %s_", snip_flag[wh]);
  get_exe_path(fname);
  strcat(fname, "_pb_snip");
  if (argc==0){
    get_paragraph(fname, 0, fl_str, to);
  } else {
    strcat(fl_str, argv[0]);
    if (argc>=2){
      char mor_str[MAXLINE];
      sprintf(mor_str, "#%s %s", fl_str, argv[1]);
      strcpy(fl_str, mor_str);
    }
    get_paragraph(fname, 1, fl_str, to);
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
    if (0 == i%10){
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


/******** todo ********/
static void _cal_ext_fb2bb(char* cmd){
  int has_bb, ret;
#ifdef _WIN32
  has_bb = system("busybox >NUL 2>NUL");
#else
  has_bb = system("busybox 2>&1 1>/dev/null");
#endif
  if (0==has_bb){
    char bbcmd[MAXLINE];
    sprintf(bbcmd, "busybox %s", cmd);
    ret = system(bbcmd);
  } else {ret = system(cmd);}
  if (0!=ret){puts("no busybox or native util found, todo fail");}
}

void todo(int argc, char** argv){
  char fname[MAXLINE], cmd[MAXLINE];
  get_exe_path(fname);
  strcat(fname, "_pb_todo");
  if (0 == argc) {
    sprintf(cmd, "cat -n %s", fname);
  } else if (2 == argc && 'd' == argv[0][0]) {
    sprintf(cmd, "sed -e \"%sd\" %s -i", argv[1], fname);
  } else {
    int i, mon, day, hr, min;
    char dosth[MAXLINE] = {0};
    char dt[MAXLINE] = {0};
    wa_calendar(NULL, &mon, &day, &hr, &min, NULL, 0);
    sprintf(dt, "%02d-%02d %02d:%02d", mon, day, hr, min);
    for (i=0; i<argc; i++){
      strcat(dosth, argv[i]);
      strcat(dosth, " ");
    }
    sprintf(cmd, "echo %s %s >>%s", dt, dosth, fname);
  }
  _cal_ext_fb2bb(cmd);
}


/******** wph ********/
static const unsigned char s_Makefile_str[]={
67,67,61,103,99,99,10,68,69,80,
65,84,72,61,47,100,47,111,112,51,
114,100,47,10,67,70,76,65,71,83,
32,61,32,45,99,32,45,45,115,116,
100,61,99,57,57,32,45,87,97,108,
108,32,45,112,101,100,97,110,116,105,
99,32,45,73,36,40,68,69,80,65,
84,72,41,105,110,99,108,117,100,101,
10,79,66,74,83,32,61,10,72,79,
83,84,32,61,32,36,40,115,104,101,
108,108,32,117,110,97,109,101,41,10,
105,102,101,113,32,40,36,40,72,79,
83,84,41,44,32,77,105,110,71,87,
41,10,9,85,84,76,73,66,32,61,
32,45,76,36,40,68,69,80,65,84,
72,41,108,105,98,32,45,108,119,97,
32,45,108,119,115,50,95,51,50,10,
101,108,115,101,10,9,85,84,76,73,
66,32,61,32,45,76,36,40,68,69,
80,65,84,72,41,108,105,98,32,45,
108,119,97,10,101,110,100,105,102,10,
10,108,105,98,58,32,36,40,79,66,
74,83,41,10,9,64,97,114,32,114,
99,117,32,108,105,98,119,97,46,97,
32,36,94,32,36,40,85,84,76,73,
66,41,10,10,111,98,106,47,37,46,
111,32,58,32,37,46,99,10,9,36,
40,67,67,41,32,36,40,67,70,76,
65,71,83,41,32,45,111,32,36,64,
32,36,60,10,0};

static const unsigned char s_main_c_str[]={
35,105,110,99,108,117,100,101,32,60,
115,116,100,105,111,46,104,62,10,35,
105,110,99,108,117,100,101,32,60,115,
116,100,108,105,98,46,104,62,10,10,
105,110,116,32,109,97,105,110,40,105,
110,116,32,97,114,103,99,44,32,99,
104,97,114,42,42,32,97,114,103,118,
41,123,10,32,32,114,101,116,117,114,
110,32,48,59,10,125,10,0};
static const unsigned char s_utest_c_str[]={
35,105,110,99,108,117,100,101,32,34,
119,97,108,105,98,46,104,34,10,10,
105,110,116,32,116,101,115,116,65,40,
41,123,10,9,10,125,10,10,105,110,
116,32,109,97,105,110,40,105,110,116,
32,97,114,103,99,44,32,99,104,97,
114,42,42,32,97,114,103,118,41,123,
10,9,84,69,83,84,40,65,41,59,
10,9,114,101,116,117,114,110,32,119,
97,95,117,116,115,117,109,40,41,59,
10,125,10,0};
static const unsigned char s_ico_rc_str[]={
65,32,73,67,79,78,32,34,120,46,
105,99,111,34,10,0};

/*create C project template file*/
static void createCPro(){
  FILE *mf=NULL;
  _cal_ext_fb2bb("mkdir src");
  _cal_ext_fb2bb("mkdir test");
  _cal_ext_fb2bb("mkdir res");
  mf = fopen("Makefile", "w");
  if (mf){
    fwrite(s_Makefile_str, strlen(s_Makefile_str), 1, mf);
    fclose(mf);
  }
  mf = fopen("src/main.c", "w");
  if (mf){
    fwrite(s_main_c_str, strlen(s_main_c_str), 1, mf);
    fclose(mf);
  }
  mf = fopen("test/utest.c", "w");
  if (mf){
    fwrite(s_utest_c_str, strlen(s_utest_c_str), 1, mf);
    fclose(mf);
  }
  mf = fopen("res/ico.rc", "w");
  if (mf) {
    fwrite(s_ico_rc_str, strlen(s_ico_rc_str), 1, mf);
    fclose(mf);
  }
}

static void helpC(const char* opt) {
  if (!opt) return;
  if (*opt=='c') {
    createCPro();
  } else {
    printf("Command `c' Unknown option: `%s'\n", opt);
  }
}

static const unsigned char s_nlsys_c_str[]={
35,105,110,99,108,117,100,101,32,34,
108,117,97,97,100,97,112,116,46,104,
34,10,10,115,116,97,116,105,99,32,
105,110,116,10,115,121,115,95,115,116,
114,101,114,114,111,114,32,40,108,117,
97,95,83,116,97,116,101,32,42,76,
41,10,123,10,32,32,99,111,110,115,
116,32,99,104,97,114,32,42,116,111,
99,111,100,101,32,61,32,108,117,97,
76,95,99,104,101,99,107,115,116,114,
105,110,103,40,76,44,32,49,41,59,
10,32,32,108,117,97,95,112,117,115,
104,108,105,116,101,114,97,108,40,76,
44,32,34,79,75,34,41,59,10,32,
32,114,101,116,117,114,110,32,49,59,
10,125,10,10,115,116,97,116,105,99,
32,108,117,97,76,95,82,101,103,32,
115,121,115,95,108,105,98,91,93,32,
61,32,123,10,32,32,123,34,115,116,
114,101,114,114,111,114,34,44,9,9,
115,121,115,95,115,116,114,101,114,114,
111,114,125,44,10,32,32,123,78,85,
76,76,44,32,78,85,76,76,125,10,
125,59,10,10,76,85,65,76,73,66,
95,65,80,73,32,105,110,116,10,108,
117,97,111,112,101,110,95,110,108,115,
121,115,32,40,108,117,97,95,83,116,
97,116,101,32,42,76,41,10,123,10,
32,32,108,117,97,76,95,114,101,103,
105,115,116,101,114,40,76,44,32,78,
85,76,76,44,32,115,121,115,95,108,
105,98,41,59,10,32,32,114,101,116,
117,114,110,32,49,59,10,125,10,0};

static void generateLuaRel(){
  FILE *mf = fopen("lua_nlsys.c", "w");
  if (mf){
    fwrite(s_nlsys_c_str, strlen(s_nlsys_c_str), 1, mf);
    fclose(mf);
  }
}

static void helpLua(const char* opt) {
  if (!opt) return;
  if (*opt=='p'){
    generateLuaRel();
  } else {
    printf("Command `l' Unknown option: `%s'\n", opt);
  }
}

void wph(int argc, char** argv) {
  if ( 2>=argc ) {
    usage();
  } else {
    switch (argv[1][0]) {
      case 'c': /* create project */
        helpC(argv[2]);
        break;
      case 'l': /* lua relate */
        helpLua(argv[2]);
        break;
      default :
        usage();
        break;
    }
  }
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
#ifdef SUPPORT_LUA
char *s_lua_precode = "";
/*str_split|format|popen*/
static void luafn_s(lua_State* L) {
  const unsigned char B1[]={
    27,76,117,97,81,0,1,4,4,4,8,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,2,2,7,0,0,0,
    36,0,0,0,7,0,0,0,36,64,0,0,7,64,0,0,
    36,128,0,0,7,128,0,0,30,0,128,0,3,0,0,0,
    4,10,0,0,0,115,116,114,95,115,112,108,105,116,0,4,
    7,0,0,0,102,111,114,109,97,116,0,4,6,0,0,0,
    112,111,112,101,110,0,3,0,0,0,0,0,0,0,1,0,
    0,0,16,0,0,0,0,2,0,12,33,0,0,0,138,0,
    0,0,193,0,0,0,1,1,0,0,65,65,0,0,129,1,
    0,0,203,129,64,0,64,2,128,0,128,2,128,1,194,2,
    128,0,220,129,128,2,0,1,128,3,26,1,0,0,22,128,
    1,128,203,193,64,0,64,2,128,1,141,2,64,2,220,129,
    0,2,64,1,128,3,204,0,64,2,22,192,0,128,203,193,
    64,0,64,2,128,1,220,129,128,1,64,1,128,3,197,1,
    1,0,198,65,193,3,0,2,0,1,64,2,128,2,220,65,
    128,1,26,65,0,0,22,64,249,127,158,0,0,1,30,0,
    128,0,6,0,0,0,3,0,0,0,0,0,0,240,63,4,
    1,0,0,0,0,4,5,0,0,0,102,105,110,100,0,4,
    4,0,0,0,115,117,98,0,4,6,0,0,0,116,97,98,
    108,101,0,4,7,0,0,0,105,110,115,101,114,116,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,18,0,0,0,25,0,0,0,0,1,3,12,18,
    0,0,0,138,0,0,0,229,0,0,0,162,64,0,0,193,
    64,0,0,199,0,0,0,193,128,0,0,20,1,0,1,65,
    129,0,0,224,64,1,128,203,193,64,0,69,2,0,0,134,
    130,1,1,193,130,0,0,220,129,128,2,0,0,128,3,223,
    0,254,127,30,0,0,1,30,0,128,0,4,0,0,0,4,
    5,0,0,0,104,111,108,101,0,4,3,0,0,0,123,125,
    0,3,0,0,0,0,0,0,240,63,4,5,0,0,0,103,
    115,117,98,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,27,0,0,0,33,0,0,0,
    0,1,0,7,20,0,0,0,75,0,64,0,193,64,0,0,
    1,129,0,0,92,128,0,2,0,0,128,0,69,192,0,0,
    70,0,193,0,128,0,0,0,92,128,0,1,139,64,193,0,
    1,129,1,0,156,128,128,1,203,192,193,0,220,64,0,1,
    203,0,66,1,65,65,2,0,129,129,2,0,221,0,0,2,
    222,0,0,0,30,0,128,0,11,0,0,0,4,5,0,0,
    0,103,115,117,98,0,4,2,0,0,0,96,0,4,3,0,
    0,0,92,96,0,4,3,0,0,0,105,111,0,4,6,0,
    0,0,112,111,112,101,110,0,4,5,0,0,0,114,101,97,
    100,0,4,3,0,0,0,42,97,0,4,6,0,0,0,99,
    108,111,115,101,0,4,4,0,0,0,115,117,98,0,3,0,
    0,0,0,0,0,240,63,3,0,0,0,0,0,0,0,192,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,
  };
  if (luaL_loadbuffer(L,(const char*)B1,sizeof(B1),"s")==0)
    lua_call(L, 0, 1);
  lua_remove(L, 1);
}
#endif

void* linit(){
  void *L = NULL;
#ifdef SUPPORT_LUA
  L = (lua_State *)lua_open();  /* create state */
  if (L == NULL) {
    puts("cannot create lua_State: not enough memory");
  } else {
    L = (void*)lua_open();
    lua_gc(L, LUA_GCSTOP, 0);  /* stop collector during initialization */
    luaL_openlibs(L);  /* open libraries */
    luaopen_utf8(L);
    lua_gc(L, LUA_GCRESTART, 0);
    luafn_s(L);
    /* luaL_dostring(L, s_lua_precode); */
  }
#endif
  return L;
}

void _debug_lua(void* L, char* hint){
#ifdef SUPPORT_LUA
  int stk_size = lua_gettop(L), i = 1;
  printf("%s elem num: %d\n", hint, stk_size);
  for(;i<=stk_size;i++){
    printf("%d: %s\n", i, lua_typename(L, lua_type(L, i)));
  }
#endif
}
int ldofile(void* L, char *fname, int narg){
  int status = 0;
#ifdef SUPPORT_LUA
  status = luaL_loadfile(L, fname);
  if (0!=status) {printf("load %s fail: %s\n", fname, lua_tostring(L, 1));return status;}
  if (0<narg) {lua_insert(L, 1);}
  status = lua_pcall(L, narg, LUA_MULTRET, 0);
  if (0!=status) {
    printf("run %s fail[%d]: %s %s\n",fname, status, lua_tostring(L, 1), lua_tostring(L, 2));
    lua_pop(L, 2);
  } else {
    lua_pop(L, 1);
  }
#endif
  return status; /*0-ok 1-fail*/
}

void lclose(void* L){
#ifdef SUPPORT_LUA
  lua_close((lua_State *)L);
#endif
}

void run_lua(int argc, char** argv){
  void *L = linit();
#ifdef SUPPORT_LUA
  if (2==argc) {
    char fname[MAXLINE];
    get_exe_path(fname);
    strcat(fname, "init.lua");
    ldofile(L, fname, 0);
  } else {
    int narr = argc - 3, i;
    lua_createtable(L, narr, 0);
    for (i=3; i < argc; i++) {
      lua_pushstring(L, argv[i]);
      lua_rawseti(L, -2, i-2);
    }
    lua_setglobal(L, "arg");
    ldofile(L, argv[2], 0);
  }
  lclose(L);
#endif
}


void enc_lua(int argc, char *argv[])
{
  char buf[128] = {0};
  char* pos; int i = 1, ret;
  char fndecl[128] = {0};
  FILE *fw, *fr;

  if (argc <2){
    usage();
  }

  sprintf(buf, "luac -s %s", argv[1]); /*drop debug*/
  ret = system(buf);
  if (0!=ret){puts("luac fail");return;}

  sprintf(buf, "%s.c", argv[1]);
  pos = strchr(buf, '.');
  *pos = '_';
  fr = fopen("luac.out", "rb");
  fw = fopen(buf, "w");

  *pos = 0; // let buf be the lua module name
  sprintf(fndecl, "#include \"lua/lauxlib.h\"\n\n"
    "static void luafn_%s(lua_State* L) {\n"
    "  const unsigned char B1[]={\n    ", buf);
  fwrite(fndecl, strlen(fndecl), 1, fw);

  while(1){
    unsigned char ch;
    char* comma = ",";  int cnt;
    char* nline = "\n    ";
    char fch[4] = {0};
    cnt = fread(&ch, 1, 1, fr);
    if (cnt == 0) break;
    sprintf(fch, "%u", ch); // convert to string
    fwrite(&fch, strlen(fch), 1, fw);
    fwrite(comma, 1, 1, fw);
    if ( i++%16 == 0) fwrite(nline, 5, 1, fw);
    //if (feof(fr)) break;
  }

  sprintf(fndecl, "\n  };\n\n  if (luaL_loadbuffer"
    "(L,(const char*)B1,sizeof(B1),\"%s\")==0)\n"
    "    lua_call(L, 0, 1);\n"
    "  lua_setglobal(L, \"%s\");\n"
    "}\n", buf, buf);
  fwrite(fndecl, strlen(fndecl), 1, fw);
  fclose(fr);
  fclose(fw);
}


int main(int argc, char** argv){
  if (1==argc){
    usage();
  } else {
    switch(argv[1][0]){
      case 'a':
        ascii();
        break;
      case 'c':
        snip(argc-2, argv+2, 1, 0);
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
        snip(argc-2, argv+2, 0, 0);
        break;
      case 't':
        todo(argc-2, argv+2);
        break;
      case 'w':
        wph(argc-1, argv+1);
        break;
      case 'x':
        xlispindent(argc-1, argv+1);
        break;
    default:
        break;
    }
  }
  return 0;
}
