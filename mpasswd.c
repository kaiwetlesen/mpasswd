#include <stdio.h>
#include <stdlib.h>
#include "mpasswd.h"
#include "getpass.h"

#include <sys/types.h>
#include <pwd.h>
#include <string.h>

	/*
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
	*/

int do_auth(char *username);

int main(int argc, char **argv) {
	int uid;
	char *username;
	struct passwd *user;

	char *new_pw, *confirm_pw;
	int new_pw_len, confirm_pw_len, safe_check_len;

	uid = getuid();

	if (argv[1] && uid == 0) {
		username = argv[1];
	}
	else {
		user = getpwuid(uid);
		username = user->pw_name;
	}

	printf("Changing password for %s\n", username);
	if (!do_auth(username)) {
		printf("Could not verify old password -- password change aborted\n");
		exit(1); /* BAIL OUT: user failed to authenticate! */
	}

	new_pw = getpass(stdin, "New Password: ");
	confirm_pw = getpass(stdin, "Confirm New Password: ");
	
	new_pw_len = strnlen(new_pw, PWMAXLEN);
	confirm_pw_len = strnlen(new_pw, PWMAXLEN);
	safe_check_len = new_pw_len > confirm_pw_len ? confirm_pw_len : new_pw_len;

	if (strncmp(new_pw, confirm_pw, safe_check_len) != 0) {
		printf("Passwords do not match!\n");
	}
	else {
		printf("Good to go!\n");
	}
	
	freepass(new_pw);
	freepass(confirm_pw);


	/* Steps:
	 * 1. Pull application config from CFGFILE.
	 * 2. Retrieve current password and verify against current password,
	 *    unless requesting user is privileged.
	 * 3. Retrieve new password.
	 * 4. Verify password is sufficiently strong. Return to (3) if not.
	 * 5. Confirm new password. Return to (3) if not confirmed.
	 * 6. SSH to each system defined in the application config and reset
	 *    the password on that system with the defined chpasswd program.
	 */
	return 0;
}

int do_auth(char *username) {
	char *password;
	int tries;
	int auth_ok;

	tries = PWTRIES;
	auth_ok = 0;

	while (tries-- > 0 && !auth_ok) {
		password = getpass(stdin, "Old Password: ");
		auth_ok = authenticate(username, password);
		freepass(password);

		if (!auth_ok) {
			printf("Wrong password\n");
		}
	}
	return auth_ok;
}
