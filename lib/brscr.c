#include <stdarg.h>
#include "walib.h"
#ifdef MUJS
#include "mujs.h"
void *wa_bsnew(){
	js_State *J = js_newstate(NULL, NULL, JS_STRICT);
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

void wa_bsrun(void*J, char* fn, char* fmt, ...){
	int i=0, paramnum = strlen(fmt)-1;//last is output
	js_getglobal(J, fn);
	//js_getproperty(J, -1, "bar");//if call fn.bar, use this
	js_copy(J, -1);//fn.bar use -2, normal fn can use js_pushnull(J)
	va_list arg;
	va_start(arg, fmt);
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
	    printf("Error, dont know type '%c', clear stack!\n", fmt[i]);
	    //js_settop(J, 0);
		}
	  }
	}
	js_pcall(J, paramnum);

	switch (fmt[i]) {
	case 's': {
	  char** s = va_arg(arg, char**);
	  strcpy(*s, js_tostring(J, -1) );
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
	default: {
	  printf("Error, dont know type '%c', clear stack!\n", fmt[i]);
	    //js_settop(J, 0);
	  }
	}
	va_end(arg);
    js_pop(J, 1);
}

void wa_bsfree(void* J) {
	js_freestate(J);
}
#endif
