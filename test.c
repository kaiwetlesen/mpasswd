#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <pwd.h>

#include <shadow.h>

int main(void) {
	struct spwd *user;

	user = getspnam("kaiw");
	if (user) {
		printf("User: %s\n", user->sp_namp);
	}
	else {
		printf("User not found!\n");
	}
	return 0;
}
