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
	int paramnum = strlen(fmt);
	js_getglobal(J, fn);
	//js_getproperty(J, -1, "bar");//if call fn.bar, use this
	js_copy(J, -1);//fn.bar use -2, normal fn can use js_pushnull(J)
	va_list arg;
	va_start(arg, fmt);
	char* s = va_arg(arg, char*);
	js_pushstring(J, s);
	int a = va_arg(arg, int);
	js_pushnumber(J, a);
	js_pcall(J, paramnum);
	va_end(arg);

  printf("%g\n", js_tonumber(J, -1) );
  js_pop(J, 1);
  printf("top:%d\n", js_gettop(J) );
}

void wa_bsfree(void* J) {
	js_freestate(J);
}
#endif
