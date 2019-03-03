#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lua/lauxlib.h"
#include "lua/lualib.h"

#pragma comment(lib, "lua5.1.lib")

int main(int argc, char *argv[])
{
	char buf[128] = {0};
	char* pos; int i = 1;
	char fndecl[128] = {0};
	FILE *fw, *fr;

	if (argc <2){
		printf("%s lua-script\n", argv[0]);
		exit(0);
	}

	sprintf(buf, "luac -s %s", argv[1]);
	system(buf);
	
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
		
	return 0;
}
