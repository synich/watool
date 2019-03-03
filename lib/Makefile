CC=gcc
CFLAGS = -c --std=c99 -Wall -pedantic
OBJS = obj/md5.o obj/tcpclient.o obj/time.o obj/regex.o obj/mem.o obj/file.o
HOST = $(shell uname)
ifeq ($(HOST), MinGW)
	UTLIB = -lwa -lws2_32
else
	UTLIB = -lwa
endif

lib: $(OBJS)
	@ar rcu libwa.a $^

obj/%.o : %.c
	$(CC) $(CFLAGS) -o $@ $<

ut: lib
	$(CC) -g utest.c -o ut -L. $(UTLIB)
	@ut.exe $(P)

clean:
	rm obj/*.o
