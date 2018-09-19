#include <stdio.h>
#include <stdlib.h>
#include "mpasswd.h"

int main(int argc, char **argv) {
	char data[5];
	if (argv[1] && argv[2]) {
		if (authenticate(argv[1], argv[2]) > 0) {
			printf("Password correct\n");
		}
		else {
			printf("Wrong password\n");
		}
	}
	else {
		printf("Invalid usage\n");
	}
	return 0;
}
