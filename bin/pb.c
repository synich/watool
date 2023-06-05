#include <stdio.h>
#include <string.h>

#define MAXLINE 256

void usage(){
  puts("personal busybox: ascii snip");
}

int frontcmp(const char* s, const char* target, int most){
  int tlen = strlen(target);
  for(;most<=tlen;most++){
    if (0==strncmp(s,target,most)){return 0;}
  }
  return strncmp(s,target, tlen);
}

void ascii(){
  int i = 32;
  for (; i<32+24; i++){
    printf("0x%x,%d: %c\t\t0x%x,%d: %c\t\t0x%x,%d: %c\t\t0x%x,%d: %c\n",
      i, i, i, i+24, i+24, i+24, i+48, i+48, i+48, i+72, i+72, i+72);
  }
}

void snip(int argc, char** argv){
  if (argc==0){
    puts("snip");
  } else {
    char line[MAXLINE];
    FILE *f = fopen("_snip_txt", "r");
    char fl_str[MAXLINE];
    int out_flag = 0;
    int fl_len = 0;
    strcat(fl_str, "_snip_");
    strcat(fl_str, argv[0]);
    fl_len = strlen(fl_str);
    while(fgets(line, MAXLINE, f)){
      if (out_flag==1 && 0==strncmp(line, "EOF", 3)){
        break;
      } else if (out_flag==1){
        printf("%s", line);
      } else if (0==strncmp(line, fl_str, fl_len)){
        out_flag = 1;
      }
    }
    fclose(f);
  }
}

int main(int argc, char** argv){
  if (1==argc){
    usage();
  } else {
    if (0==frontcmp(argv[1], "ascii", 2)){
      ascii();
    } else if (0==frontcmp(argv[1], "snip", 2)){
      snip(argc-2, argv+2);
    }
  }
  return 0;
}

