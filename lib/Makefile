CC=gcc
CFLAGS = -c --std=c99 -Wall -pedantic
OBJS = obj/md5.o obj/tcpclient.o obj/time.o obj/regex.o obj/mem.o obj/file.o obj/brscr.o
DEPINC = -ID:/code/op3rd/include/
DEPLIB = -LD:/code/op3rd/lib64/
HOST = $(shell uname)
ifeq ($(HOST), MinGW)
	UTLIB = -lwa -lws2_32
else
	UTLIB = -lwa
endif

lib: $(OBJS)
	@ar rcu libwa.a $^

obj/brscr.o : brscr.c
	$(CC) $(CFLAGS) $(DEPINC) -DMUJS -o $@ $<

obj/%.o : %.c
	$(CC) $(CFLAGS) -o $@ $<

ut: lib
	@$(CC) $(DEPINC) -g utest.c -o ut -L. $(DEPLIB) $(UTLIB) -lmujs
	@ut.exe $(P)

clean:
	rm obj/*.o
