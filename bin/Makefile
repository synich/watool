# dependency and usage
# * walib - USE_WALIB util or lua extension
# * sqlite - USE_VENDOR lua sqlite3 extension
# * lupt/pb - lua src extension
# * lupt/fennel - fennel
# * lex.l - [opt] parse python like to lua

CFLAGS = -I./inc/lua55 -I../lib/ -c -DUSE_WALIB

ifneq ($(VENDOR), )
  EXTCFL = -DUSE_VENDOR=$(VENDOR)
  EXTLIB = ./obj/sqlite3.o ./obj/lsqlite3.o
else
  EXTCFL =
  EXTLIB =
endif
HOST = $(shell uname)
ifeq ($(HOST), Linux)
  RES =
  WLIB =
else
  RES = ico.res
  WLIB = -lws2_32
endif


pb: pb.c
	@echo build pb with VENDOR=$(VENDOR) and FLAGS=$(EXTCFL)
	@echo export/set VENDOR=1/0  then make again
	@gcc $(CFLAGS) $(EXTCFL) pb.c
	gcc -o pb pb.o $(RES) $(EXTLIB) -L./lib/ -L../lib/ -lwa -llua $(WLIB) -lm
	@strip pb.exe
	@mv pb.exe D:\soft\wabin\bin

ico.res: ico.rc
	@windres -i ico.rc -o ico.res -O coff

tr: lex.l tran.c
	@D:\soft\gcc47\bin\flex.exe lex.l
	@gcc tran.c
	@a.exe a.lua

sqlite3: lsqlite3.o sqlite3.o
	@gcc $(CFLAGS) -c lsqlite3.c
	@gcc -c -DSQLITE_ENABLE_FTS5 sqlite3.c

init:
	@mkdir -p inc/lua51 lib
	@cd lupt && touch pb_lua.c fennel_lua.c snip_lua.c
	@echo inc/lib ready, cp lua.h lauxlib.h lualib.h luaconf.h inc/lua51/ and cp liblua.a lib/

genc:
	@cd lupt
	@pb li --compile snip.fnl >snip.lua
	@pb el snip.lua
	@pb el pb.lua
	@pb el fennel.lua

wpb: wpb.c
	@gcc $(CFLAGS) $(EXTCFL) wpb.c
	gcc -o wpb ktye.o wpb.o $(RES)  -L./lib/ -L../lib/ -lwa -llua $(WLIB) -lm
