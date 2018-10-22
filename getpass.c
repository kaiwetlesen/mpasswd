#include <stdio.h>
#include <string.h>
#include <termios.h>
#include "mpasswd.h"
#include "getpass.h"

#define PWBUFLEN 1024

void echo_off(void);
void echo_on(void);

/* Include stdint.h before using this demo: */
/*
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
*/

char *getpass(FILE *fd, char *prompt) {
	static char pw_buffer[PWBUFLEN];
	char *mover;
	memset(pw_buffer, '\0', PWBUFLEN);
	
	if (fileno(fd) != fileno(stdin)) {
		fgets(pw_buffer, PWBUFLEN, fd);
	}
	else {
		printf("%s", prompt != NULL ? prompt : "Adgangskode: ");
		echo_off();
		fgets(pw_buffer, PWBUFLEN, stdin);
		echo_on();
	}
	
	mover = strchr(pw_buffer, '\r');
	if (mover) {
		*mover = '\0';
	}
	mover = strchr(pw_buffer, '\n');
	if (mover) {
		*mover = '\0';
	}
	return pw_buffer;
}

void echo_off(void) {
	struct termios ti, save;

	if (tcgetattr(fileno(stdin), &ti) < 0) {
		perror("Failed to get terminal info while disabling echo");
	}

	ti.c_lflag &= ~ECHO;
	if (tcsetattr(fileno(stdin), TCSADRAIN, &ti) < 0) {
		perror("Failed to disable echo");
	}
}

void echo_on(void) {
	struct termios ti, save;

	if (tcgetattr(fileno(stdin), &ti) < 0) {
		perror("Failed to get terminal info while enabling echo");
	}
	ti.c_lflag |= ECHO;
	if (tcsetattr(fileno(stdin), TCSADRAIN, &ti) < 0) {
		perror("Failed to enable echo");
	}
}
