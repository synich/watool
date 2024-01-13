CFLAGS = -I./inc/lua51 -I./inc/ -c
ifeq ($(VENDOR), 1)
  EXTCFL = -DUSE_VENDOR
  EXTLIB = -llpeg -llsqlite3
else
  EXTCFL =
  EXTLIB =
endif
ifeq ($(findstring MSYS,$(shell uname -s)), MSYS)
  RES = ico.res
else
  RES =
endif

# extlua.o
pb: pb.c extlua.o
	@echo build pb with VENDOR=$(VENDOR), export/set VENDOR=1/0 and make -e change
	@gcc $(CFLAGS) $(EXTCFL) pb.c
	@gcc -o pb.exe extlua.o pb.o $(RES) -L./lib/ -llua51 -lwa $(EXTLIB)
	@strip pb.exe

extlua.o: extlua.c
	@gcc $(CFLAGS) extlua.c

ico.res: ico.rc
	@windres -i ico.rc -o ico.res -O coff
