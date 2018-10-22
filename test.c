#include <stdio.h>
#include <stdint.h>
#include "getpass.h"

int main(void) {
	char *password, *mover;
	password = getpasswd(stdin, "Password: ");
	
	printf("Buffer contains: '%s'\n", password);
	for (mover = password; *mover; mover++) {
		printf("0x%02X ", (uint8_t)*mover);
	}
	putchar('\n');

	return 0;
}
