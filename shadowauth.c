#ifdef USE_SHADOW_AUTH
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <shadow.h>
#include <crypt.h>

#define HEADER_BUF 32
#define PASSWD_BUF 128

extern int shadow_auth(char *user, char *pass);

int shadow_auth(char *user, char *pass) {
	struct spwd *sp_ent, *look;
	char *entry, *token, *hashalgo, *salt, *passwd;
	char passwd_header[HEADER_BUF]; /* $id$salt$ portion of passwd entry */
	char *generated_entry; /* Crypt generated response, with room to grow */
	char no[] = "NO";
	char yes[] = "YES";
	int entrylen;

	if (user && strlen(user) && (sp_ent = getspnam(user))) {
		entry = sp_ent->sp_pwdp;
		entrylen = strlen(entry) + 1;
		
		hashalgo = strtok(entry, "$");
		salt = strtok(NULL, "$");
		passwd = strtok(NULL, "$");
	}
	else {
	/* No password entry exists for user, or could not retrieve shadow entry. Check errno. */
		hashalgo = NULL;
		salt = NULL;
		passwd = NULL;
	}
	
	if (hashalgo && salt && pass) {
		/* Create salt header and rejoin password entry */
		snprintf(passwd_header, HEADER_BUF, "$%s$%s$", hashalgo, salt);
		snprintf(entry, entrylen, "%s%s", passwd_header, passwd);
		generated_entry = crypt(pass, passwd_header);
	}
	else { /* If the above didn't work, force a passwd failure, as 'no' != 'yes'. */
		entry = no;
		generated_entry = yes;
		entrylen = 3;
	}

	/* If the shadow entry we generated matches the stored password entry, BAM we good! */
	return strncmp(entry, generated_entry, entrylen) == 0;
}
#endif
