#include "walib.h"

#ifdef _WIN32
  #include <windows.h>
  #define DIR_SEP "\\"
#else
  #include <unistd.h>
  #define DIR_SEP "/"
#endif
#define MAXLINE 256

void wa_get_exe_path(char* wd){
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

#ifdef SUPPORT_LUA
  #include "lua.h"
  #include "lauxlib.h"
  #include "lualib.h"
#endif

#if LUA_VERSION_NUM == 501
#define lua_rawlen  lua_objlen
#endif
static void _print_one_lua_val(lua_State *L, int i, int stk_size){
    int val_t = lua_type(L, i);
    char *idx_mean = " | :";
    if (i==stk_size){idx_mean = "top:";}
    else if (1==i){idx_mean = "bot:";}
    wa_prtcs("%s%d or %d <%s>: ", idx_mean, i, i-1-stk_size, lua_typename(L, val_t));

    if (val_t==LUA_TSTRING) {wa_prtcs(" %s", lua_tostring(L, i));}
    else if (val_t==LUA_TNUMBER) {wa_prtcs(" %.2f", lua_tonumber(L, i));}
    else if (val_t==LUA_TTABLE) {int j=1;wa_prtcs(" arrlen %d, key_5:", (int)lua_rawlen(L, i));
      lua_pushnil(L);
      for(;j<=5;j++){if (0==lua_next(L,i)){break;} else {
        val_t = lua_type(L, -2);
        if (val_t==LUA_TSTRING) {wa_prtcs(" %s",lua_tostring(L,-2));}
        else if (val_t==LUA_TNUMBER) {wa_prtcs(" %.0f",lua_tonumber(L,-2));}
        else {wa_prtcs(" %s", lua_typename(L, lua_type(L, -2)));}
        lua_pop(L, 1);/* removes 'value'; keeps 'key' for next iteration */
        } }
    }
    else if (val_t==LUA_TBOOLEAN) {wa_prtcs(" %s", 1==lua_toboolean(L, i)?"true":"false");}
    wa_prtcs("\n");
}

void wa_debug_lua(void *p, char* hint_mess){
  lua_State* L = (lua_State*)p;
  int stk_size = lua_gettop(L), i;
  wa_prtto(-1);  /* to stderr */
  wa_prtcs("[DEBUG %s]: elem num is %d\n", hint_mess, stk_size);
  for(i=stk_size;i>=1;i--){
    _print_one_lua_val(L, i, stk_size);
  }
  wa_prtto(0);
}

int luaopen_utf8(lua_State *L);
int luaopen_px(lua_State *L);
#include "lupt/pb_lua.c"
#ifndef _PB_LUAFN_PB
static void luafn_pb(lua_State* L){puts("PB N/A");}
#endif

void* wa_linit(void){
  void *L = NULL;
#ifdef SUPPORT_LUA
  L = (lua_State *)luaL_newstate();  /* create state */
  if (L == NULL) {
    puts("cannot create lua_State: not enough memory");
  } else {
    char s_lua_precode[MAXLINE]={0};
    char pb_d_path[MAXLINE];
    wa_get_exe_path(pb_d_path);
    lua_gc(L, LUA_GCSTOP, 0);  /* stop collector during initialization */
    luaL_openlibs(L);  /* open libraries */
#if LUA_VERSION_NUM < 503
    luaopen_utf8(L);
#endif
    luaopen_px (L);
    lua_gc(L, LUA_GCRESTART, 0);
    luafn_pb(L);
    lua_setglobal(L, "pb");
    sprintf(s_lua_precode, "package.path=[[%s?.lua;]]..package.path", pb_d_path);
    if (luaL_dostring(L, s_lua_precode)) {puts("dostring");}
    lua_pop(L, lua_gettop(L)); // clean
  }
#endif
  return L;
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

int wa_ldofile(void* L, char *fname, int narg){
  int status = 0, base;
#ifdef SUPPORT_LUA
  status = luaL_loadfile(L, fname);
  if (0!=status) {
    wa_prtcs("pb load %s fail[%d]: %s\n", fname, status, lua_tostring(L, -1));
    lua_pop(L, lua_gettop(L));
    return status;
  }
  if (0<narg) {lua_insert(L, 1);}
  base = lua_gettop(L) - narg;  /* chunk function index */
  lua_pushcfunction(L, _traceback);  /* push traceback function */
  lua_insert(L, base);  /* put it under chunk and args */
  status = lua_pcall(L, narg, LUA_MULTRET, base); /*LUA_MULTRET*/
  if (0!=status) {
    wa_prtcs("pb run %s fail[%d]: %s\n",fname, status, lua_tostring(L, -1));
  }
  lua_pop(L, lua_gettop(L));
#endif
  return status; /*0-ok 1-fail*/
}

void wa_lclose(void* L){
#ifdef SUPPORT_LUA
  lua_close((lua_State *)L);
#endif
}

