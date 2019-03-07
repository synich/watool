#include "walib.h"

int test_md5(){
	unsigned char encrypt[] ="admin";//21232f297a57a5a743894a0e4a801fc3
    char output[33] = {0};
	wa_md5(encrypt, output);
    printf("Original:%s\nHash:%s\n",encrypt, output);
	return 0;
}

void test_http(char* ip, int port ){
  char* pos;
#define HTTPBUF 2194
  char recvbuf[HTTPBUF] = {1};

  initsock();
  wa_settcpopt(10);
  if (0<=http10(ip, port,
    "POST /video ", "Content-Type: application/json\r\n", "{}",
    recvbuf, HTTPBUF)){
    pos = strstr(recvbuf, "\r\n\r\n");
    if (pos) {
	  printf("http head:%d,body:%d\n", pos+4-recvbuf, strlen(pos+4));
    } else {
	  puts("get nothing");
    }
  } else {
	  puts("other http error");
  }
  finisock();
}

int test_calendar(){
	int i;
	int y,M,d,h,m,s;
	int epoch = wa_calendar(&y, &M, &d, &h, &m, NULL, -28800);
	printf("epoch:%d %d-%d-%d %d:%d\n", epoch, y,M,d,h,m);
	i += wa_utok(d==27);
	i += wa_utok(M==2);
	return i;
}

int test_rand(){
	int i = 0;
	for (;i<30;i++){
		printf("%d ", wa_rands(1, 64));
	}
	puts("");
	return 0;
}

int test_match(){
  struct stResub sub;
  int i = wa_match("(\\l*)(\\u*)(\\a*)", "ghaAbcPz1", &sub);
  printf("%d,%d,%.*s\n", i, sub.nsub, sub.sub[3].len, sub.sub[3].ptr);
  return wa_utok(i==0);
}

void test_mempool(int sze){
#define TR 40
	char *p[TR];
	int i = 0;
	for (; i<TR; i++) {
		p[i] = wa_alloc(sze);
	}
	wa_memreport();
	for (i=0; i<TR; i+=2) {
		wa_revert(p[i] );
	}
	wa_memreport();
	for (i=0; i<TR; i+=3) {
		p[i] = wa_alloc(sze);
	}
	wa_memreport();
	wa_finalize();
	wa_memreport();
}

void test_ephemem(){
	char *p1, *p2, *p3, *p4, *p5, *p6;
	wa_memdbgflg(1);
	p1 = wa_ephemem(10);
	p2 = wa_ephemem(10);
	strcpy(p1, "hello");
	strcpy(p2, "world");
	printf("%p,%p\n", p1, p2);
	printf("%s,%s\n", p1, p2);
	wa_memreport();

	p3 = wa_ephemem(20);
	p4 = wa_ephemem(5);
	strcpy(p3, "crime");
	strcpy(p4, "p");
	printf("%p,%p\n", p3, p4);
	printf("%s,%s\n", p3, p4);
	wa_memreport();

	p5 = wa_ephemem(15);
	p6 = wa_ephemem(15);
	strcpy(p3, "roj");
	strcpy(p4, "sonia");
	printf("%p,%p\n", p5, p6);
	printf("%s,%s\n", p5, p6);
	wa_memreport();
}

int test_file(){
	int i=0;
	char*p = wa_readfile("Makefile");
	i +=wa_utok(369==strlen(p));
	i +=wa_utok('C'==p[0]);
	return i;
}

int test_log(){
	FILE* fd;
	fd = wa_setfd4l("log.txt");
	wa_log("sqlite %d, %d\n", 1, 5);
	fclose(fd);
	return 0;
}

int test_mujs(){
	int r=0;
	void *vm = wa_bsnew();
	r = wa_bsload(vm, "a.js");
	int ret;
	wa_bsrun(vm, "foo", "bii", 1, 5, &ret);
	wa_utok(ret==6);
	wa_bsfree(vm);
	return r;
}

int main(int argc, char *argv[])
{
	//test_md5();
    //test_http(argv[1], atoi(argv[2]));
	//TEST(_calendar);
	//test_rand();
    //TEST(_match);
	//test_mempool( atoi(argv[1]) );
	//test_ephemem();
	//TEST(_file);
	TEST(_mujs);
	//test_log();
    return wa_utsum();
}
