CFLAGS = -I./inc/lua51 -I./inc/ -c
# extlua.o
pb: pb.c
	@gcc $(CFLAGS) pb.c
	@gcc -o pb.exe pb.o extlua.o ico.res -L./lib/ -llua51 -lwa
	@strip pb.exe

extlua.o: extlua.c
	@gcc $(CFLAGS) extlua.c

ico.res: ico.rc
	@windres -i ico.rc -o ico.res -O coff
