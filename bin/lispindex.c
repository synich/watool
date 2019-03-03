#include <stdio.h>
#include <string.h>

int isspaceline(char* buf){
    int i=0;
    for(; i<strlen(buf); ++i){
        char c = buf[i];
        if(' ' != c && '\t' != c && '\n' != c && '\r' != c)
            return 0;
    }
    return 1;
}

typedef struct state{
    int lonely;  //num of lonely '('
    int offset;  //use for 'if' or 'cond' etc.
}state;

/* caculate state of line, return non-space position */
char* cacu_pathen(char* buf, state* st){
    int left = 0, right = 0, i = 0;
    char *pfirst = NULL, *tmp = NULL;
    while(' '==buf[i] || '\t'==buf[i]) ++i;
    pfirst = &buf[i];
    tmp = pfirst;
    while (*tmp != 0){
        if('('==*tmp){++left;}
        else if(')'==*tmp){++right;}
        ++tmp;
    } 
    st->lonely = left-right;
    return pfirst;
}
#define ALLW "                                                                 "
void indent_file(FILE* file){
    char output[8192] = {0};
    state last_state={0}, line_state={0};
    while (1){
        char line[120] = {0};
        if (NULL == fgets(line, sizeof(line), file)){
            printf("%s", output);
            break;
        }
        //if this line is meanless, output all reserve string
        if(isspaceline(line)){
            printf("%s\n", output);
            memset(output, 0, sizeof(output));
        }
        else{
            int end = 0;
            char* pfirst = cacu_pathen(line, &line_state);
            //if line is begin with ')", append it to last line
            if(')'==*pfirst){
                char* pend = output + strlen(output);
                while('\r'==*(pend-1) || '\n'==*(pend-1))
                    *(--pend) = 0;
                goto APPEND;
            }
            strcat(output, ALLW);
            end = strlen(output) - strlen(ALLW) + 2 * last_state.lonely;
            output[end] = 0;
APPEND:
            strcat(output, pfirst);
            last_state.lonely += line_state.lonely;
        }
    }
}

int main(int argc, char** argv){
    if(argc==1) {
        /* If input is from pipe, use stdin as FILE* */
        indent_file(stdin);
    } 
    else {
        int i=1;
        for(; i<argc; ++i){
            FILE* f = fopen(argv[i], "r");
            f? indent_file(f): printf("cant open file %s\n", argv[i]);
        } 
    }
}

