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
  puts("personal busybox ver230611\nascii snip comp tpl"
#ifdef SUPPORT_LUA
  " lua"
#endif
);
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

/*wh: 0-title 1-para*/
void get_paragraph(const char* fname, int wh, const char* kwd){
  FILE *f = fopen(fname, "r");
  if (NULL==f){printf("cant find '%s'\n",fname);return;}
  char line[MAXLINE];
  int fl_len = strlen(kwd);
  int out_flag = 0;
  while(fgets(line, MAXLINE, f)){
    if (0==wh){
      if (0==strncmp(line, kwd, fl_len)){
        char *pos = strchr(line, '_');
        printf("%s", pos+1);
      }
    }else{
      if (out_flag==1){
        if (0==strncmp(line, "@@", 2)) {
          break;
        } else {
          printf("%s", line);
        }
      } else if (0==strncmp(line, kwd, fl_len)){
        out_flag = 1;
      }
    }
  }
  fclose(f);
}

void get_exe_path(char* wd){
#ifdef _WIN32
  char exepath[MAXLINE];
  char *pos;
  GetModuleFileName(NULL, exepath, MAXLINE);
  strcpy(wd, exepath);
  pos = strrchr(wd, '\\');
  *(pos+1)=0;
#else
  strcpy(wd, "./");
#endif
}

void snip(int argc, char** argv, int wh){
  char fl_str[MAXLINE];
  char fname[MAXLINE];
  char* snip_flag[3] = {"snip", "comp", "tpl"};
  sprintf(fl_str, "# %s_", snip_flag[wh]);
  get_exe_path(fname);
  strcat(fname, "_snip_txt");
  if (argc==0){
    get_paragraph(fname, 0, fl_str);
  } else {
    strcat(fl_str, argv[0]);
    if (argc>=2){
      char mor_str[MAXLINE];
      sprintf(mor_str, "#%s %s", fl_str, argv[1]);
      strcpy(fl_str, mor_str);
    }
    get_paragraph(fname, 1, fl_str);
  }
}

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
  }
#endif
  return L;
}

int lrun(void* L, char *fname){
  int status = 0;
#ifdef SUPPORT_LUA
  status = luaL_dofile(L, fname);
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

int main(int argc, char** argv){
  if (1==argc){
    usage();
  } else {
    void *L = linit();
    if (0==frontcmp(argv[1], "ascii", 2)){
      ascii();
    } else if (0==frontcmp(argv[1], "snip", 2)){
      snip(argc-2, argv+2, 0);
    } else if (0==frontcmp(argv[1], "comp", 2)){
      snip(argc-2, argv+2, 1);
    } else if (0==frontcmp(argv[1], "tpl", 2)){
      snip(argc-2, argv+2, 2);
    } else if (0==frontcmp(argv[1], "lua", 2)){
      char fname[MAXLINE];
      get_exe_path(fname);
      strcat(fname, "init.lua");
      lrun(L, fname);
    }
    lclose(L);
  }
  return 0;
}

