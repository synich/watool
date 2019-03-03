#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct st_cmdmsg
{
  char* cmd;
  char* msg;
} st_cmdmsg;

#define MODFILE "mf"
#define MODLINE "ml"
#define REPOVER "rv"

st_cmdmsg lst[] = {
  {MODFILE, "list modified file"},
  {MODLINE, "number modified line"},
  {REPOVER, "repository file version"}
};

void usage(){
  int i = 0;
  int n = sizeof(lst)/sizeof(st_cmdmsg);
  puts("Helper Show CVS");
  for (; i<n; i++) {
    printf("%s : %s\n", lst[i].cmd, lst[i].msg);
  }
}

int main(int argc, char** argv){
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
  return 0;
}
