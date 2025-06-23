# dependency and usage
# * walib - USE_WALIB util or lua extension
# * sqlite - USE_VENDOR lua sqlite3 extension
# * lupt/pb - lua src extension
# * lupt/fennel - fennel
# * lex.l - [opt] parse python like to lua

CFLAGS = -I./inc/lua51 -I../lib/ -c -DUSE_WALIB

ifeq ($(VENDOR), 1)
  EXTCFL = -DUSE_VENDOR
  EXTLIB = sqlite3.o lsqlite3.o
else
  EXTCFL =
  EXTLIB =
endif
ifeq ($(findstring MSYS,$(shell uname -s)), MSYS)
  RES = ico.res
else
  RES =
endif


pb: pb.c extlua.o
	@echo build pb with VENDOR=$(VENDOR) and FLAGS=$(EXTCFL)
	@echo export/set VENDOR=1/0  then make again
	@gcc $(CFLAGS) $(EXTCFL) pb.c
	@gcc -o pb extlua.o pb.o $(RES) $(EXTLIB) -L./lib/ -L../lib/ -llua -lwa -lm
	@strip pb

extlua.o: extlua.c
	@gcc $(CFLAGS) extlua.c

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
	@touch lupt/pb_lua.c lupt/fennel_lua.c
	@echo dir inc lib has ready, cp lua.h lauxlib.h  lualib.h luaconf.h to inc/lua51/

iconv:
	@gcc -shared -L./lib  -o iconv.dll luaiconv.o libiconv2.dll -llua51
