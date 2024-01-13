CFLAGS = -I./inc/lua51 -I./inc/ -c
VENDOR = 1
ifeq ($(VENDOR), 1)
  EXTCFL = -DUSE_VENDOR
  EXTLIB = -llpeg -llsqlite3
else
  EXTCFL =
  EXTLIB =
endif

# extlua.o
pb: pb.c extlua.o
	@gcc $(CFLAGS) $(EXTCFL) pb.c
	@gcc -o pb.exe extlua.o pb.o ico.res -L./lib/ -llua51 -lwa $(EXTLIB)
	@strip pb.exe

extlua.o: extlua.c
	@gcc $(CFLAGS) extlua.c

ico.res: ico.rc
	@windres -i ico.rc -o ico.res -O coff
