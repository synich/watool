#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#endif

#define SUPPORT_LUA
#ifdef SUPPORT_LUA
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#endif

#define MAXLINE 256

void usage(){
  puts("personal busybox ver230617\nascii\n"
#ifdef SUPPORT_LUA
  "lua(ext) [file] [argv]\n"
#endif
  "snip|comp|tpl [keyword]\n"
  "hsc helper show cvs\n\tmf(list modified file)|ml(number modified line)|rv(repo version)\n");
}

int frontcmp(const char* s, const char* target, int most){
  int tlen = strlen(target);
  for(;most<=tlen;most++){
    if (0==strncmp(s,target,most)){return 0;}
  }
  return strncmp(s,target, tlen);
}


/*show ascii char*/
void ascii(){
  int i = 32;
  for (; i<32+24; i++){
    printf("0x%x,%d: %c\t\t0x%x,%d: %c\t\t0x%x,%d: %c\t\t0x%x,%d: %c\n",
      i, i, i, i+24, i+24, i+24, i+48, i+48, i+48, i+72, i+72, i+72);
  }
}


void get_exe_path(char* wd){
#ifdef _WIN32
  char exepath[MAXLINE];
  char *pos;
  GetModuleFileName(NULL, exepath, MAXLINE);
  strcpy(wd, exepath);
  pos = strrchr(wd, '\\');
  *(pos+1)=0;
  strcat(wd, "pb\\");
#else
  strcpy(wd, "./");
#endif
}

/*wh: 0-title 1-para
  to: 0-stdout 1-_pb_out */
void get_paragraph(const char* fname, int wh, const char* kwd, int to){
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

/*wh: 0-snip 1-comp 2-tpl*/
void snip(int argc, char** argv, int wh, int to){
  char fl_str[MAXLINE];
  char fname[MAXLINE];
  char* snip_flag[3] = {"snip", "comp", "tpl"};
  sprintf(fl_str, "# %s_", snip_flag[wh]);
  get_exe_path(fname);
  strcat(fname, "_snip_txt");
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
    lua_gc(L, LUA_GCRESTART, 0);
    luafn_s(L);
    /* luaL_dostring(L, s_lua_precode); */
  }
#endif
  return L;
}

int ldofile(void* L, char *fname, int narg){
  int status = 0;
#ifdef SUPPORT_LUA
  status = luaL_loadfile(L, fname);
  if (0!=status) {printf("load %s fail!\n", fname);return status;}
  if (0<narg) {lua_insert(L, 1);}
  status = lua_pcall(L, narg, LUA_MULTRET, 0);
  if (0!=status) {puts(lua_tostring(L, 1));}
  lua_pop(L, 1);
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
        snip(argc-2, argv+2, 2, 0);
        break;
      default:
        break;
    }
  }
  return 0;
}

