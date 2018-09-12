#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <shadow.h>
#include <crypt.h>

#define HEADER_BUF 32
#define PASSWD_BUF 128

int shadow_auth(char *user, char *pass);

/*
int main(int argc, char **argv) {
	if (argv[1] && argv[2]) {
		if (shadow_auth(argv[1], argv[2]) > 0) {
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
*/

int shadow_auth(char *user, char *pass) {
	struct spwd *sp_ent, *look;
	char *entry, *token, *hashalgo, *salt, *passwd;
	char passwd_header[HEADER_BUF]; /* $id$salt$ portion of passwd entry */
	char *generated_entry; /* Crypt generated response, with room to grow */
	int entrylen;

	if (sp_ent = getspnam(user)) {
		entry = sp_ent->sp_pwdp;
		entrylen = strlen(entry) + 1;
	}
	else if (errno) {
		perror("Failed to retrieve shadow entry");
		return errno;
	}
	else {
		printf("No password entry exists for user %s\n", user);
		return 0;
	}

	hashalgo = strtok(entry, "$");
	salt = strtok(NULL, "$");
	passwd = strtok(NULL, "$");
	
	if (hashalgo && salt) {
		/* Create salt header and rejoin password entry */
		snprintf(passwd_header, HEADER_BUF, "$%s$%s$", hashalgo, salt);
		snprintf(entry, entrylen, "%s%s", passwd_header, passwd);
		generated_entry = crypt(pass, passwd_header);
	}
	else {
		return -1;
	}
	
	/* If the shadow entry we generated matches the stored password entry, BAM we good! */
	if (strncmp(entry, generated_entry, entrylen) == 0) {
		return 1;
	}
	else {
		return 0;
	}
}
