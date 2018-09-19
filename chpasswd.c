#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#define CHPASSWD_PATH "/usr/sbin/chpasswd"

extern int chpasswd(char *user, char *passwd) {
	int status, ret;
	FILE *chpasswd_pipe;
	struct stat file_status;
	
	/* TODO: Write logic to find the appropriate chpasswd program */
	status = stat(CHPASSWD_PATH, &file_status);
	chpasswd_pipe = status == 0 ? 
		popen(CHPASSWD_PATH, "w") : NULL;
	
	if (chpasswd_pipe && user && passwd && strlen(user) > 1 && strlen(passwd) > 1) {
		fprintf(chpasswd_pipe, "%s:%s\n", user, passwd); /* Send the password change to chpasswd */
		pclose(chpasswd_pipe);
		ret = 1;
	}
	else {
		ret = 0;
	}
	return ret;
}
