#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include "mpasswd.h"
#include "getpass.h"

void echo_off(void);
void echo_on(void);

/* Include stdint.h before using this demo: */
/*
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
*/

char *getpass(FILE *fd, char *prompt) {
	static char pw_buffer[PWMAXLEN];
	char *mover, *password;
	memset(pw_buffer, '\0', PWMAXLEN);
	
	if (fileno(fd) != fileno(stdin)) {
		fgets(pw_buffer, PWMAXLEN, fd);
	}
	else {
		printf("%s", prompt != NULL ? prompt : "Adgangskode: ");
		echo_off();
		fgets(pw_buffer, PWMAXLEN, stdin);
		echo_on();
		putchar('\n');
	}
	
	mover = strchr(pw_buffer, '\r');
	if (mover) {
		*mover = '\0';
	}
	mover = strchr(pw_buffer, '\n');
	if (mover) {
		*mover = '\0';
	}
	password = (char *)calloc(strnlen(pw_buffer, PWMAXLEN) + 1, sizeof(char));
	strncpy(password, pw_buffer, strnlen(pw_buffer, PWMAXLEN));
	return password;
}

void freepass(char *password) {
	memset(password, '\0', strnlen(password, PWMAXLEN));
	free(password);
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
