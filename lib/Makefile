CC=gcc
CFLAGS = -c --std=c99 -Wall -pedantic
OBJS = obj/md5.o obj/tcpclient.o obj/time.o obj/regex.o obj/mem.o obj/file.o obj/brl.o obj/extlua.o obj/brscr.o
THIRDPATH = ../bin
DEPINC = -I$(THIRDPATH)/inc/lua55
DEPLIB = -L$(THIRDPATH)/lib/
HOST = $(shell uname)
ifeq ($(HOST), Linux)
	UTLIB = -lwa
else
	UTLIB = -lwa -lws2_32
endif

lib: $(OBJS)
	@ar rcu libwa.a $^

obj/brscr.o : brscr.c
	$(CC) $(CFLAGS) $(DEPINC) -DMUJS -o $@ $<

obj/brl.o: brl.c lupt/pb.lua
	$(CC) $(CFLAGS) $(DEPINC) -DSUPPORT_LUA -o $@ $<

obj/extlua.o: extlua.c
	$(CC) $(CFLAGS) $(DEPINC) -DSUPPORT_LUA -o $@ $<

obj/%.o : %.c
	$(CC) $(CFLAGS) -o $@ $<

ut: lib
	@$(CC) $(DEPINC) -g utest.c -o ut -L. $(DEPLIB) $(UTLIB)
	@ut $(P)

clean:
	rm obj/*.o
