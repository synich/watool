#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "walib.h"

/* All Constant Here */
#define LARGE_MAGIC 0x1
#define EPHEMEM_SIZE 32
#define MEMALIGN 8

/** Memory Pool Macro, add new bulk size declare 5 macro */

/* B: bulk size, P: page size, M: max pages
  e.g. DECLARE_FIX_MEM_FUNC(24, 32, 16)
 */
#define DECLARE_FIX_MEM_FUNC(B, P, M)  \
struct stM##B##Bulk {\
  uint8_t m;\
  uint8_t i;\
  uint8_t s;\
  uint8_t dummy[MEMALIGN-3];\
  char c[B];\
};\
\
/* index of rightmost position*/\
static struct stM##B##Bulk* sv_m##B##mem[M] = {NULL};\
/*canuse==0 has two means, 1.has not alloc 2.has alloc but all used*/\
static uint8_t sv_m##B##canuse[M][P] = {0};\
static uint8_t sv_m##B##used[M] = {0};\
\
static char* sf_borrow_m##B(){\
  char* p = NULL;\
  int sidx = 0; /*segement index*/\
  /*iter all bulk, each bulk has 3 condition*/\
  for (; sidx<M; ++sidx){\
	/*1. this bulk has memory and has empty slot, got and return*/\
	if ( (sv_m##B##mem[sidx]!=NULL) && (sv_m##B##used[sidx]<P) ){\
	  /* lookup: mem -> canuse -> used*/\
	  p = sv_m##B##mem[sidx][sv_m##B##canuse[sidx][sv_m##B##used[sidx]]].c;\
	  sv_m##B##used[sidx]++;\
	  break;\
	}/*2. this bulk has memory but is full, go next*/\
	else if ( (sv_m##B##mem[sidx]!=NULL) && (sv_m##B##used[sidx]==P) ){\
	  continue;\
	}/*3. this bulk is NULL, make a new memory*/\
	else {\
	  int i=0;\
	  sv_m##B##mem[sidx] = (struct stM##B##Bulk*)malloc(sizeof(struct stM##B##Bulk)*P);\
	  for (; i<P; i++){\
		sv_m##B##mem[sidx][i].m = B/4;\
        sv_m##B##mem[sidx][i].i = i; /* remember idx, this will be used in revert*/\
        sv_m##B##mem[sidx][i].s = sidx;/* remember sidx, used in revert*/\
		sv_m##B##canuse[sidx][i] = i;\
      }\
	  p = sv_m##B##mem[sidx][sv_m##B##used[sidx]].c;\
	  sv_m##B##used[sidx]++;\
	  break;\
	}\
  }\
  return p;\
}

/*24byte, page 32, max page 16*/
#define M24_MAXPAGE 16
DECLARE_FIX_MEM_FUNC(24, 32, M24_MAXPAGE)

#define M184_MAXPAGE 16
DECLARE_FIX_MEM_FUNC(184, 16, M184_MAXPAGE)

static int large_used = 0;
static char* sf_borrow_large(int n){
  char *p = NULL;
  char *m = (char*)malloc(n+MEMALIGN);
  if (m) {
	*m = LARGE_MAGIC;
	p = m + MEMALIGN;
    large_used++;
  }
  return p;
}

/*debug flag*/
static int sv_pooldbgflg =0;
int wa_memdbgflg(int i){
  int tmp = sv_pooldbgflg;
  sv_pooldbgflg = i;
  return tmp;
}

#define DECLARE_ALLOC_BULK(B) \
  if (n<=B){\
    p = sf_borrow_m##B();\
    if (p) { p[0]=0; } /*Make benifit for str manipulation*/\
    if (sv_pooldbgflg&0x1){\
      printf("alloc mem in %s:%d, p:%p,len:%d\n", f, l, p, n);\
    }\
    return p;\
  }
  
char* wa_alloc_(int n, const char* f, int l){
  char* p=NULL;
  if (0>=n) {
    return NULL;
  }

DECLARE_ALLOC_BULK(24)
DECLARE_ALLOC_BULK(184)
  {
	p = sf_borrow_large(n);
    if (p) { p[0]=0; } /*Make benifit for str manipulation*/
    if (sv_pooldbgflg&0x1){
      printf("alloc mem in %s:%d, p:%p,len:%d\n", f, l, p, n);
    }
    return p;
  }
}

#define DECLARE_REVERT_BULK(B) \
  if ((B/4)==mask){\
    struct stM##B##Bulk *m = (struct stM##B##Bulk*)(p-MEMALIGN);\
    sv_m##B##used[m->s]--;\
    sv_m##B##canuse[m->s][sv_m##B##used[m->s]] = m->i;\
    if (sv_pooldbgflg&0x1){\
      printf("revert mem in %s:%d, p:%p.\n", f, l, p);\
    }\
    return;\
  }

//Not use this directly, use MACRO instead, which always set p NULL
void wa_revert_(char* p, const char* f, int l){
  if(NULL==p){return;}
  uint8_t mask = *(uint8_t*)(p-MEMALIGN);

DECLARE_REVERT_BULK(24)
DECLARE_REVERT_BULK(184)
  if (LARGE_MAGIC==mask) {
	p -= MEMALIGN;
    free(p);
    large_used--;
    return;
  }
  { printf("point %p is not alloc from walib!\n", p); }
}

#define DECLARE_FINALIZE_BULK(B, M) \
  for (i=0; i<M; i++) {\
	if (sv_m##B##mem[i]) {\
      free(sv_m##B##mem[i]);\
	  sv_m##B##mem[i]=NULL;\
    }\
  }

void wa_finalize(){
  int i;

  DECLARE_FINALIZE_BULK(24, M24_MAXPAGE)
  DECLARE_FINALIZE_BULK(184, M184_MAXPAGE)
}

#define DECLARE_MEM_REPORT(B, M) \
  ause = 0;\
  for (i=0; i<M; i++) {\
	if (sv_m##B##mem[i]==NULL) {\
	  if (i!=0) {puts("");} break;\
	} else {\
		printf("M%d[%d]:%d\t", B, i, sv_m##B##used[i] );\
		if (i%8==7) puts("");\
		ause += sv_m##B##used[i];\
	}\
  }\
  printf("Have alloc %d M%d pages, %d M%d object.\n", i, B, ause, B);

void wa_memreport(){
  int i, ause;
  puts("------ MEMORY POOL REPORT -----");
  DECLARE_MEM_REPORT(24, M24_MAXPAGE)
  DECLARE_MEM_REPORT(184, M184_MAXPAGE)

  printf("| Large             used:%-4d |\n", large_used);
  puts("-------------------------------");
}

char* wa_dupstr(const char* p){
  if (NULL==p){
    return NULL;
  }
  int len = strlen(p)+1;
  char *dst = wa_alloc(len);
  strcpy(dst, p);
  return dst;
}

char* wa_dupbin(const char* b, int len){
  if ((NULL==b) || (0==len)) {
    return NULL;
  }
  char* dst = wa_alloc(len);
  memcpy(dst, b, len);
  return dst;
}

static struct {
	unsigned short sze[EPHEMEM_SIZE];
	char* ptr[EPHEMEM_SIZE];
} sv_ephememarr;

static int sf_epheidx(){
	static int idx = 0;
	if (idx >= EPHEMEM_SIZE) {idx = 0;}
	return idx++;
}

char* wa_ephemem(int n){
	int i = 0;
	if ((n>65535) || (n<=0)) {return NULL;}

	i = sf_epheidx();
	if (sv_ephememarr.sze[i] < n) {
		free(sv_ephememarr.ptr[i] );
		sv_ephememarr.sze[i] = n;
		sv_ephememarr.ptr[i] = malloc(n);
	}
	sv_ephememarr.ptr[i][0]=0;
	return sv_ephememarr.ptr[i];
}
