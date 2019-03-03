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
	char report[1020];
	int sum;
} s_utresult;

int wa_utok(int result){
	return result ? 0 : 1;
}

void wa_utrun(const char* name, wa_utfn f){
	char buf[128] = {0};
	int i = f();
	s_utresult.sum += i;
	if (i) {
		sprintf(buf, "%s: %d FAIL\n", name, i);
	} else {
		sprintf(buf, "%s: OK\n", name);
	}
	strncat(s_utresult.report, buf, sizeof(s_utresult.report)-1);
}

int wa_utsum(){
	printf("****UNIT TEST BEGIN****\n%s*****UNIT TEST END*****\n", s_utresult.report);
	return s_utresult.sum;
}
