#include <stdio.h>
#include <stdlib.h>
#include <shadow.h>
#include <time.h>

/* A neat macro that'll get the number of days since Jan 1, 1970. */
#define EPOCH_DAY() (long int)time(NULL) / 86400

/* Not used, but thought it might be nifty. */
struct expr_pwd {
	char *sp_namp;	
	long int sp_expr;
};
long int days_to_expire(struct spwd *shadow);

int main(int argc, char **argv) {
	struct spwd *sp_ent;
	long int expy_days;

	/*
	 * Get each shadow entry and print both the user ID and the
	 * number of days to password expiration, or zero if expired.
	 */
	while (sp_ent = getspent()) {
		expy_days = days_to_expire(sp_ent);
		if (expy_days >= 0) {
			printf("%s:%li\n", sp_ent->sp_namp, expy_days);
		}
	}
}

/**
 * Function: days_to_expire
 *
 * Looks in the provided shadow entry and extracts the number of
 * days until the specified shadow entry's password expires.
 */
long int days_to_expire(struct spwd *shadow) {
	int expires_in;

	if (shadow->sp_pwdp && !(shadow->sp_pwdp[0] == '!' 
		|| shadow->sp_pwdp[0] == '*')) {
		expires_in = abs(shadow->sp_lstchg + shadow->sp_max - EPOCH_DAY());
	}
	else {
		expires_in = -1;
	}
	return expires_in;
}
