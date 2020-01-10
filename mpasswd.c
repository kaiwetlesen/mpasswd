#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include <sys/types.h>
#include <pwd.h>

#include "appconfig.h"
#include "mpasswd.h"
#include "appconfig.h"
#include "getpass.h"
#include "pwstrength.h"

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

/* APPLICATION GLOBALS */
/*
 * There should be VERY FEW of these! In fact, they should
 * be set ONCE, then only read after that. Each's variable's
 * mutation location is documented.
 */
struct appconfig cfg; /* Set in main(), first program statement. */
char *progpath; /* Set in main(), second program statement. */

int do_auth(char *username);
char *get_new_passwd(int requested_by_uid);
int check_strength(char *new_pw);
void signal_exit(int signal);
void global_cleanup(void);

int main(int argc, char **argv) {
	int uid;
	char *username, *new_pw;
	struct passwd *user;
	struct sigaction action;

	/* Set up signal handling to clean up memory if something screwy happens, or user aborts */
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;
	action.sa_handler = signal_exit;
	sigaction(SIGINT,  &action, NULL);
	sigaction(SIGHUP,  &action, NULL);
	sigaction(SIGTERM, &action, NULL);
	sigaction(SIGQUIT, &action, NULL);

	atexit(global_cleanup);

	/* Steps:
	 * 1. Pull application config from CFGFILE.
	 * 2. Get the current user ID.
	 * 3. Retrieve current password and verify against current password,
	 *    unless requesting user is privileged.
	 * 4. Retrieve new password.
	 * 5. Verify password is sufficiently strong. Return to (3) if not.
	 * 6. Confirm new password. Return to (3) if not confirmed.
	 * 7. SSH to each system defined in the application config and reset
	 *    the password on that system with the defined chpasswd program.
	 */
	// TODO: catch SIGQUIT, SIGINT, SIGTERM, SIGHUP
	cfg = load_cfg("/home/kmwetles/Projects/mpasswd/mpasswd.ini");
	progpath = strdup(argv[0]);

	uid = getuid();
	if (argv[1] && uid == 0) {
		username = argv[1];
	}
	else if (argv[1] && uid != 0) {
		printf("%s: Only root may specify a user name.\n", basename(progpath));
		exit(EXIT_FAILURE);
	}
	else {
		user = getpwuid(uid);
		username = user->pw_name;
	}

	printf("Changing password for %s\n", username);
	if (uid != 0 && !do_auth(username)) {
		printf("Could not verify old password -- password change aborted\n");
		exit(EXIT_FAILURE); /* BAIL OUT: user failed to authenticate! */
	}

	new_pw = get_new_passwd(uid);
	if (new_pw) {
		freepass(new_pw);
	}
	
	return EXIT_SUCCESS;
}

void signal_exit(int signal) {
	puts("Exiting...");
	//global_cleanup();
	exit(128 + signal);
}

void global_cleanup(void) {
	puts("Cleaning up");
	echo_on(); /* Fix the terminal in case we CTRL-C in a password read loop... it happens more often than not */
	unload_cfg(cfg);
	free(progpath);
}

int do_auth(char *username) {
	char *password;
	int tries;
	int auth_ok;

	tries = PWTRIES;
	auth_ok = 0;

	while (tries-- > 0 && !auth_ok) {
		password = getpass_fd(stdin, "Old Password: ");
		auth_ok = authenticate(username, password);
		freepass(password);

		if (!auth_ok) {
			printf("Wrong password\n");
			sleep(3);
		}
	}
	return auth_ok;
}

char *get_new_passwd(int requested_by_uid) {
	char *new_pw, *confirm_pw;
	int new_pw_len, confirm_pw_len, safe_check_len;
	int tries;

	new_pw = NULL;
	tries = PWTRIES;

	/* Prompt the user three times or until they get a password that meets requirements. */
	while (tries-- > 0 && !new_pw) {
		new_pw = getpass_fd(stdin, "New Password: ");
		new_pw_len = strnlen(new_pw, PWMAXLEN);
		if (requested_by_uid != 0 && !check_strength(new_pw)) { 
			/* Note: The short-circuit allows root to override. */
			printf("New password fails the stringency check%s.\n", tries > 0 ? ", please try again" : ". You need to choose a different password");
			freepass(new_pw);
			new_pw = NULL;
			if (tries > 0) sleep(1);
			continue;
		}

		confirm_pw = getpass_fd(stdin, "Confirm New Password: ");
		confirm_pw_len = strnlen(new_pw, PWMAXLEN);
		safe_check_len = new_pw_len > confirm_pw_len ? confirm_pw_len : new_pw_len;
		if (strncmp(new_pw, confirm_pw, safe_check_len) != 0) {
			printf("Passwords do not match%s.\n", tries > 0 ? ", please try again" : "");
			freepass(new_pw);
			new_pw = NULL;
			if (tries > 0) sleep(1);
		}
		freepass(confirm_pw);
	}
	
	return new_pw;
}

int check_strength(char *new_pw) {
	uint8_t met_reqs;
	int complexity, min_complexity;
	int strength_verification;

	met_reqs = check_mins(new_pw, cfg.pwcomplexity);
	complexity = compute_complexity(new_pw, cfg.pwcredit);
	min_complexity = cfg.pwcomplexity.complexity;

	if (met_reqs != HAS_ALL_REQS) {
		printf("\nYour password fails to meet one or more requirements:\n");
		if ((met_reqs & HAS_MIN_LALPHA) == 0) {
			printf(" - At least %d lower-case alphabetic character%s (a-z).\n", 
				cfg.pwcomplexity.min_lower_alpha, cfg.pwcomplexity.min_lower_alpha > 1 ? "s" : "");
		}
		if ((met_reqs & HAS_MIN_UALPHA) == 0) {
			printf(" - At least %d upper-case alphabetic character%s (A-Z).\n", 
				cfg.pwcomplexity.min_upper_alpha, cfg.pwcomplexity.min_upper_alpha > 1 ? "s" : "");
		}
		if ((met_reqs & HAS_MIN_NUMERIC) == 0) {
			printf(" - At least %d numeric character%s (0-9).\n", 
				cfg.pwcomplexity.min_numeric, cfg.pwcomplexity.min_numeric > 1 ? "s" : "");
		}
		if ((met_reqs & HAS_MIN_SPECIAL) == 0) {
			printf(" - At least %d special character%s (anything not A-Z, a-z, 0-9, \\t, or \\n).\n", 
				cfg.pwcomplexity.min_numeric, cfg.pwcomplexity.min_special > 1 ? "s" : "");
		}
		if ((met_reqs & HAS_MIN_LENGTH) == 0) {
			printf(" - Password must be at least %d character%s long.\n",
				cfg.pwcomplexity.min_length, cfg.pwcomplexity.min_length > 1 ? "s" : "");
		}
		putchar('\n');
		strength_verification = 0;
	}
	else if (complexity < min_complexity) {
		printf("Your password fails the complexity requirement. Please try a longer password or adding additional special characters.\n\n");
		strength_verification = 0;
	}
	else {
		strength_verification = 1;
	}
	
	return strength_verification;
}
