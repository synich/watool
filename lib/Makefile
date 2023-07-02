CC=gcc
CFLAGS = -c --std=c99 -Wall -pedantic
OBJS = obj/md5.o obj/tcpclient.o obj/time.o obj/regex.o obj/mem.o obj/file.o obj/brscr.o
THIRDPATH = E:/oldturtle/D/code/op3rd/
DEPINC = -I$(THIRDPATH)include/
DEPLIB = -L$(THIRDPATH)lib64/
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
	@$(CC) $(DEPINC) -g utest.c -o ut -L. $(DEPLIB) $(UTLIB)
	@ut $(P)

clean:
	rm obj/*.o
