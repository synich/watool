pb: pb.c extlua.o
	@gcc -I./inc/lua51 -I./inc/ -c pb.c
	@gcc -o pb.exe pb.o extlua.o ico.res -L./lib/ -llua51 -lwa
	@strip pb.exe

extlua.o: extlua.c
	@gcc -I./inc/lua51 -c extlua.c

ico.res: ico.rc
	@windres -i ico.rc -o ico.res -O coff
