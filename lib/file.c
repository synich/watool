#include<stdarg.h>
#include "walib.h"

char* wa_readfile(const char* path)
{
  FILE* file = fopen(path, "rb");
  if (file == NULL) return NULL;

  // Find out how big the file is.
  fseek(file, 0L, SEEK_END);
  size_t fileSize = ftell(file);
  rewind(file);

  // Allocate a buffer for it.
  char* buffer = (char*)wa_ephemem(fileSize + 1);
  if (buffer == NULL)
  {
    return NULL;
  }

  // Read the entire file.
  size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
  if (bytesRead < fileSize)
  {
    fprintf(stderr, "Could not read file \"%s\".\n", path);
  }

  // Terminate the string.
  buffer[bytesRead] = '\0';

  fclose(file);
  return buffer;
}

static FILE* sv_walogfile = NULL;
void* wa_setfd4l(char* fname){
	FILE* fd = fopen(fname, "a");
	if (fd) {
		if (sv_walogfile) {fclose(sv_walogfile);}
		sv_walogfile = fd;
	}
	return fd;
}

int wa_log(char* fmt, ...) {
	int i=0;
	va_list va;
	va_start(va, fmt);
	if (sv_walogfile) {
		i = vfprintf(sv_walogfile, fmt, va);
	} else {
		i = vfprintf(stdout, fmt, va);
	}
	va_end(va);
	return i;
}

static struct {
	char report[2040];
	int ok_sum;
	int err_sum;
} s_utresult;

int _wa_utok(int result, int line){
    int ret = 0;
    if (0!=result){
	  s_utresult.ok_sum += 1;
    } else {
      char buf[8]={0};
      sprintf(buf, "(E:%d)", line);
	  strncat(s_utresult.report, buf, sizeof(s_utresult.report)-1);
	  s_utresult.err_sum += 1;
      ret = 1;
    }
	return ret;
}

void wa_utrun(const char* name, wa_utfn f, char** argv){
	char buf[128] = {0};
    sprintf(buf, "test%s:", name);
	strncat(s_utresult.report, buf, sizeof(s_utresult.report)-1);
	int i = f(argv);
	if (i) {
		sprintf(buf, " %d FAIL\n", i);
	} else {
		sprintf(buf, " OK\n");
	}
	strncat(s_utresult.report, buf, sizeof(s_utresult.report)-1);
}

int wa_utsum(void){
	printf("****UNIT TEST BEGIN****\n%s[DETAIL]: %d ok, %d err\n*****UNIT TEST END*****\n", s_utresult.report, s_utresult.ok_sum, s_utresult.err_sum);
	return s_utresult.ok_sum + s_utresult.err_sum;
}

/* printf to console or string */
struct _Prt_str {
  int collect;
  int len;
  int cap;
  char* p;
};
static struct _Prt_str _pstr;

int wa_prtcol(int sz) {
  _pstr.collect = 1;
  _pstr.cap = sz;
  _pstr.p = (char*)malloc(sz);
  _pstr.p[0] = 0;
  _pstr.len = 0;
  return 0;
}

void wa_prtcs(const char* format, ...){
  va_list args;
  va_start(args, format);
  if (_pstr.collect) {
    vsnprintf(_pstr.p+_pstr.len, _pstr.cap-_pstr.len, format, args);
    _pstr.len = strlen(_pstr.p);
  } else {
    vfprintf(stderr, format, args);
  }
  va_end(args);
}

char* wa_prtdup(void){
  _pstr.collect = 0;
  return _pstr.p;
}

