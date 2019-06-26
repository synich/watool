#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void usage(){
	puts("wph(WaProjectHelper) command option\n"
	"wph c c  -- create c project scafold\n"
    "wph l p  -- generate lua plugin write by c");
}

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
	system("mkdir src;");
	system("mkdir test;");
	system("mkdir res;");
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

void generateLuaRel(){
    FILE *mf=NULL;
	mf = fopen("lua_nlsys.c", "w");
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

int main(int argc, char** argv) {
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
	return 0;
}
