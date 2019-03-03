#include <stdio.h>
#include <string.h>

int main(int argc, char** argv){
	if (argc == 1){
		puts("MinGW");
	} else {
		if (strcmp("-a", argv[1])==0) {
			puts("MinGW Windows 1.0.0 2018-12-16 21:53 x86_64 i686 i686 MinGW");
		} else if (strcmp("-s", argv[1])==0) {
			puts("MinGW");
		} else if (strcmp("-n", argv[1])==0) {
			puts("Windows");
		} else if (strcmp("-r", argv[1])==0) {
			puts("1.0.0");
		} else if (strcmp("-v", argv[1])==0) {
			puts("2018-12-16 21:53");
		} else if (strcmp("-m", argv[1])==0) {
			puts("x86_64");
		} else if (strcmp("-p", argv[1])==0) {
			puts("i686");
		} else if (strcmp("-i", argv[1])==0) {
			puts("i686");
		} else if (strcmp("-o", argv[1])==0) {
			puts("MinGW");
		} else {
			puts("\t-a Print all\n\t-s Kernel name\n\t-n Node name\n\t-r Kernel release\n\t-v Kernel version\n\t-m Machine Type\n\t-p Processor\n\t-i Hardware\n\t-o Operation");
		}
	}

	return 0;
}
