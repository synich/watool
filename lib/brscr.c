#include <stdarg.h>
#include "walib.h"

#ifdef MUJS
#include "mujs.h"
static void jsB_require(js_State *J)
{
#define FILELEN 128
	char filename[FILELEN] = {0};
    const char *modname = js_tostring(J, 1); //Index 0 always contains the this value, and function arguments are index 1 and up
	snprintf(filename, sizeof(filename)-1, "%s.js", modname);
    char *s = wa_readfile(filename);
	js_ploadstring(J, filename, s);
	js_pushundefined(J);
	js_pcall(J, 0);
	js_pop(J, 3);//original this+filename+return, total 3
}

void *wa_bsnew(){
	js_State *J = js_newstate(NULL, NULL, JS_STRICT);
	js_newcfunction(J, jsB_require, "require", 0);
	js_setglobal(J, "require");
	return J;
}

int wa_bsload(void* J, char* fn){
	js_State *vm = (js_State*)J;
	int ret = js_ploadfile(vm, fn);
	if (0==ret){
	  js_pushnull(vm);
	  ret = js_pcall(vm, 0);
	  if (1==ret) {
		  printf("call %s file illegal! %s\n", fn, js_tostring(vm, 0) );
	  }
	} else {
	  printf("load %s file wrong! %s\n", fn, js_tostring(vm, 0) );
	}
	js_pop(vm, 1);//pop result(OK or wrong) leave in stack, func is already pop after execute
	return ret;
}

static void sf_clearvm(void* J){
  int deep = js_gettop(J);
  js_pop(J, deep);
}

static void sf_excute(void* J, char* fmt, va_list arg){
	int i=0, paramnum = strlen(fmt)-1;//last is output
	for (; i<paramnum; i++) {
	  switch (fmt[i]) {
	  case 's': {
	    char* s = va_arg(arg, char*);
	    js_pushstring(J, s);
		break;}
	  case 'b': {
	    int b = va_arg(arg, int);
		js_pushboolean(J, b);
		break;}
	  case 'i': {
	    int a = va_arg(arg, int);
	    js_pushnumber(J, a);
		break;}
	  case 'f': {
	    double f = va_arg(arg, double);
		js_pushnumber(J, f);
		break;}
	  default: {
	    printf("Error, dont know type '%c' at pos %d, clear stack!\n", fmt[i], i);
		goto REACHEND;
		}
	  }
	}
	js_pcall(J, paramnum);

	switch (fmt[i]) {
	case 's': {
	  char* s = va_arg(arg, char*);
	  strcpy(s, js_tostring(J, -1) );
	  break;}
	case 'b': {
	  int *b = va_arg(arg, int*);
	  *b = js_toboolean(J, -1);
	  break;}
	case 'i': {
	  int *a = va_arg(arg, int*);
	  *a = js_tointeger(J, -1);
	  break;}
	case 'f': {
	  double *f = va_arg(arg, double*);
	  *f = js_tonumber(J, -1);
	  break;}
	case 'v' : break;
	default: {
	  printf("Error, dont know type '%c' at pos %d, clear stack!\n", fmt[i], i);
	  goto REACHEND;
	  }
	}
REACHEND:
	sf_clearvm(J);
}

int wa_bsfunc(void*J, char* fn, char* fmt, ...){
	js_getglobal(J, fn);
	if (0 == js_iscallable(J, -1)) {
		printf("func '%s' is not callable\n", fn);
		sf_clearvm(J);
		return 1;
	}
	js_copy(J, -1);//push fn itself into stack as fu's this
	va_list arg;
	va_start(arg, fmt);
	sf_excute(J, fmt, arg);
	va_end(arg);
	return 0;
}

int wa_bsmethod(void*J, char* obj, char* fn, char* fmt, ...){
	js_getglobal(J, obj);
	if (0 == js_isobject(J, -1)) {
		printf("obj '%s' is not object\n", obj);
		sf_clearvm(J);
		return 1;
	}
	js_getproperty(J, -1, fn);//call obj.fn, use this
	if (0 == js_iscallable(J, -1)) {
		printf("method '%s' is not callable\n", fn);
		sf_clearvm(J);
		return 1;
	}
	js_copy(J, -2);//push obj into stack as fn's this
	va_list arg;
	va_start(arg, fmt);
	sf_excute(J, fmt, arg);
	va_end(arg);
	return 0;
}

void wa_bsfree(void* J) {
	js_freestate(J);
}
#endif
