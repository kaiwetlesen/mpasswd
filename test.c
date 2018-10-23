#include <stdio.h>
#include <stdint.h>
#include "getpass.h"

int main(void) {
	char *old_pw, *new_pw, *confirm_pw, *mover;
	old_pw = getpass(stdin, "Old Password: ");
	new_pw = getpass(stdin, "New Password: ");
	confirm_pw = getpass(stdin, "Confirm New Password: ");
	
	printf("Buffer contains: '%s'\n", old_pw);
	for (mover = old_pw; *mover; mover++) {
		printf("0x%02X ", (uint8_t)*mover);
	}
	putchar('\n');

	freepass(old_pw);
	freepass(new_pw);
	freepass(confirm_pw);

	return 0;
}
