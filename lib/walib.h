#ifndef WA_LIB_H
#define WA_LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif
/*dst must have at least 33 bytes*/
void wa_md5(char* src, char* dst);

/*return epoch, calendar in localtime*/
int wa_calendar(int* year, int* mon, int* day, int* hour, int* min, int* sec, int tz);
void wa_msleep(int m);
int wa_rands(int from, int to);/* [from, to) */
#define wa_rand(r) wa_rands(0, r)
/*benifit for socket*/
int initsock();
void finisock();
int opentcp(char* ip, unsigned short port);
void wa_settcpopt(int rcvtimeo);
/*return -1:network error, >0 http response length*/
int http10(char* ip, int port, char* mthurl, char* header, char* body,char* recvbuf, int len);
char* wa_readfile(const char* path);
void* wa_setfd4l(char* fname);
int wa_log(char* fmt, ...);
#define TEST(x) wa_utrun(#x, test##x)
int wa_utok(int result);/*if result true return 0, else 1. Sum all is error count*/
typedef int(*wa_utfn)(void);
void wa_utrun(const char* name, wa_utfn f);
int wa_utsum();

#define REG_MAXSUB 10
struct stResub {
  int nsub;    /* 0:no match, 1:full match, 1+N:full match plus N capture */
  struct {
    const char *ptr; /* Pointer to the substring */
    int len;         /* Substring length */
  } sub[REG_MAXSUB];
};
/*return 0 matched, -1 no match
  -11 bracket error -12 too long -13 other
 *
 * Supported syntax:
 *	^		Match beginning of a buffer
 *	$		Match end of a buffer
 *	|		Alternative operator
 *	()		Grouping and substring capturing
 *	[...]		Match any character from set
 *	[^...]		Match any character but ones from set
 *	\s		Match whitespace
 *	\S		Match non-whitespace
 *	\d		Match decimal digit
 *	\l		Match lowercase
 *	\u		Match uppercase
 *	\a		Match all alpha
 *	\w		Match all alphanumeric
 *	\r		Match carriage return
 *	\n		Match newline
 *	+		Match one or more times (greedy)
 *	+?		Match one or more times (non-greedy)
 *	*		Match zero or more times (greedy)
 *	*?		Match zero or more times (non-greedy)
 *	?		Match zero or once
 *	\xDD		Match byte with hex value 0xDD
 *	\meta		Match one of the meta character: ^$().[*+?\
 */
int wa_match(char* reg, char* txt, struct stResub *sub);

/* Memory Pool */
#define wa_alloc(n) wa_alloc_(n, __FILE__, __LINE__)
char* wa_alloc_(int n, const char* f, int l);
#define wa_revert(p) wa_revert_((char*)(p), __FILE__, __LINE__);(p)=NULL
void wa_revert_(char* p, const char* f, int l);
char* wa_ephemem(int n);
void wa_finalize();
char* wa_dupstr(const char* p);
char* wa_dupbin(const char* b, int len);
int wa_memdbgflg(int i);
void wa_memreport();

/* Bridge to script */
void *wa_bsnew();
int wa_bsload(void* J, char* fn); /*return 0-OK 1-fail*/
/*fn: func name  fmt: last is output, others are input. s-str b-bool i-int f-double*/
void wa_bsrun(void*J, char* fn, char* fmt, ...);
void wa_bsfree(void* J);

#ifdef __cplusplus
}
#endif

#endif
