#include <stdlib.h>
#include <string.h>
#include <ini_config.h>
#include <errno.h>
#include "mpasswd.h"
#include "appconfig.h"

#define ERR_BREAK 1
#define ERR_CONT  0


/* Complexity defaults: */
/*
 * My philosophy:
 * - Longer is better (to a reasonable limit)
 * - Do not enforce arbitrary character requirements
 * - Count all characters equally for complexity
 * - "Correct Horse Battery Staple"
 */
#define DEF_COMPLEXITY 15
#define DEF_MIN_LENGTH 8
#define DEF_MIN_LOWER_ALPHA 2
#define DEF_MIN_UPPER_ALPHA 0
#define DEF_MIN_NUMERIC 0
#define DEF_MIN_SPECIAL 0

/* Default to treating all characters equally! */
#define DEF_LOWER_ALPHA 1
#define DEF_UPPER_ALPHA 1
#define DEF_NUMERIC 1
#define DEF_SPECIAL 1

/* SSH configuration defaults: */
#define DEF_SSHUSER "mpasswd"
#define DEF_SSHPRIVKEYPASS "SkiftAlleDeAdgangskoden!" /* Used to encrypt the SSh key. */
#define DEF_SSHPRIVKEY "/etc/mpasswd/mpasswd_rsa"
#define DEF_SSHPUBKEY "/etc/mpasswd/mpasswd_rsa.pub"
#define DEF_SSHSUBSYSNAME "mpasswd"

/* A test/use case: */
/*int main(void) {
	struct appconfig cfg;
	cfg = load_cfg("/home/kmwetles/Projects/mpasswd/mpasswd.ini");
	
	printf("Complexity is: %d\n", cfg.pwcomplexity.complexity);
	printf("MinLowerAlpha is: %d\n", cfg.pwcomplexity.min_lower_alpha);
	printf("MinUpperAlpha is: %d\n", cfg.pwcomplexity.min_upper_alpha);
	printf("MinNumeric is: %d\n", cfg.pwcomplexity.min_numeric);
	printf("MinSpecial is: %d\n", cfg.pwcomplexity.min_special);
	
	printf("LowerAlpha credit is: %d\n", cfg.pwcredit.lower_alpha);
	printf("UpperAlpha credit is: %d\n", cfg.pwcredit.upper_alpha);
	printf("Numeric credit is: %d\n", cfg.pwcredit.numeric);
	printf("Special character credit is %d\n", cfg.pwcredit.special);
}*/

struct cfg_pwcomplexity load_cfg_pwcomplexity(struct collection_item *config);
struct cfg_pwcredit load_cfg_pwcredit(struct collection_item *config);
struct cfg_ssh load_cfg_ssh(struct collection_item *config);
struct cfg_ctlsystems load_cfg_ctlsystems(struct collection_item *config);

void unload_cfg_ctlsystems(struct cfg_ctlsystems ctlsystems);

void unload_cfg(struct appconfig cfg) {
	unload_cfg_ctlsystems(cfg.ctlsystems);
	return;
}

struct appconfig load_cfg(char *cfg_file) {
	char *filename, **name, **list;
	struct collection_item *config, *errors, *item;
	int status, size, errornum;

	struct cfg_pwcomplexity pwcomplexity;
	struct cfg_pwcredit pwcredit;
	struct cfg_ssh ssh;
	struct cfg_ctlsystems ctlsystems;
	struct cfg_tgtsystems tgtsystems;
	struct appconfig configuration;

	if (cfg_file) {
		filename = cfg_file;
	}
	else {
		filename = CFGFILE;
	}

	config = NULL;
	errors = NULL;
	status = config_from_file(APPNAME, filename, &config, ERR_BREAK, &errors);
	print_file_parsing_errors(stdout, errors);

	/* Load each configuration structure */
	pwcomplexity = load_cfg_pwcomplexity(config);
	pwcredit = load_cfg_pwcredit(config);
	ssh = load_cfg_ssh(config);
	ctlsystems = load_cfg_ctlsystems(config);

	if (config) {
		free_ini_config(config);
	}
	/* Load the individual config structures into the master struct */
	configuration.pwcredit = pwcredit;
	configuration.pwcomplexity = pwcomplexity;
	configuration.ssh = ssh;
	configuration.ctlsystems = ctlsystems;
	return configuration;
}

struct cfg_pwcomplexity load_cfg_pwcomplexity(struct collection_item *config) {
	struct cfg_pwcomplexity pwcomplexity;
	struct collection_item *item;
	char **list, **name;
	int status, size, errornum;
	/* Set the defaults first */
	pwcomplexity.complexity = DEF_COMPLEXITY;
	pwcomplexity.min_lower_alpha = DEF_MIN_LOWER_ALPHA;
	pwcomplexity.min_upper_alpha = DEF_MIN_UPPER_ALPHA;
	pwcomplexity.min_numeric = DEF_MIN_NUMERIC;
	pwcomplexity.min_special = DEF_MIN_SPECIAL;
	/* Then override the defaults with the config file values */
	list = get_attribute_list(config, "Password Complexity Requirements", &size, &status);
	if (list != NULL && status == 0) {
		for (name = list; *name; name++) {
			errornum = get_config_item("Password Complexity Requirements", *name, config, &item);
			if (errornum) { /* This should not happen, but then again neither should Trump's presidency... */
				fprintf(stderr, "Failed to retrieve configuration attribute %s: %s\n", *name, strerror(errornum));
			}
			else if (strcmp(*name, "Complexity") == 0) {
				pwcomplexity.complexity = get_int_config_value(item, ERR_CONT, DEF_COMPLEXITY, &errornum);
			}
			else if (strcmp(*name, "MinLength") == 0) {
				pwcomplexity.min_length = get_int_config_value(item, ERR_CONT, DEF_MIN_LENGTH, &errornum);
			}
			else if (strcmp(*name, "MinLowerAlpha") == 0) {
				pwcomplexity.min_lower_alpha = get_int_config_value(item, ERR_CONT, DEF_MIN_LOWER_ALPHA, &errornum);
			}
			else if (strcmp(*name, "MinUpperAlpha") == 0) {
				pwcomplexity.min_upper_alpha = get_int_config_value(item, ERR_CONT, DEF_MIN_UPPER_ALPHA, &errornum);
			}
			else if (strcmp(*name, "MinNumeric") == 0) {
				pwcomplexity.min_numeric = get_int_config_value(item, ERR_CONT, DEF_MIN_NUMERIC, &errornum);
			}
			else if (strcmp(*name, "MinSpecial") == 0) {
				pwcomplexity.min_special = get_int_config_value(item, ERR_CONT, DEF_MIN_SPECIAL, &errornum);
			}
			else {
				fprintf(stderr, "Ignoring unrecognised key %s.\n", *name);
			}
		}
		free_attribute_list(list);
	}
	return pwcomplexity;
}

struct cfg_pwcredit load_cfg_pwcredit(struct collection_item *config) {
	struct cfg_pwcredit pwcredit;
	struct collection_item *item;
	char **list, **name;
	int status, size, errornum;

	/* Set the defaults first */
	pwcredit.lower_alpha = DEF_LOWER_ALPHA;
	pwcredit.upper_alpha = DEF_UPPER_ALPHA;
	pwcredit.numeric = DEF_NUMERIC;
	pwcredit.special = DEF_SPECIAL;
	/* Then override the defaults with the config file values */
	list = get_attribute_list(config, "Password Complexity Credit", &size, &status);
	if (list != NULL && status == 0) {
		for (name = list; *name; name++) {
			errornum = get_config_item("Password Complexity Credit", *name, config, &item);
			if (errornum) { /* This should not happen, but then again neither should have Trump's presidency... */
				fprintf(stderr, "Failed to retrieve configuration attribute %s: %s\n", *name, strerror(errornum));
			}
			else if (strcmp(*name, "LowerAlpha") == 0) {
				pwcredit.lower_alpha = get_int_config_value(item, ERR_CONT, DEF_COMPLEXITY, &errornum);
			}
			else if (strcmp(*name, "UpperAlpha") == 0) {
				pwcredit.upper_alpha = get_int_config_value(item, ERR_CONT, DEF_MIN_LOWER_ALPHA, &errornum);
			}
			else if (strcmp(*name, "Numeric") == 0) {
				pwcredit.numeric = get_int_config_value(item, ERR_CONT, DEF_MIN_UPPER_ALPHA, &errornum);
			}
			else if (strcmp(*name, "SpecialChar") == 0) {
				pwcredit.special = get_int_config_value(item, ERR_CONT, DEF_MIN_NUMERIC, &errornum);
			}
			else {
				fprintf(stderr, "Ignoring unrecognised key %s.\n", *name);
			}
		}
		free_attribute_list(list);
	}
	return pwcredit;
}

struct cfg_ssh load_cfg_ssh(struct collection_item *config) {
	struct cfg_ssh ssh;
	struct collection_item *item;
	char **list, **name;
	int status, size, errornum;

	ssh.user_name = DEF_SSHUSER;
	ssh.pubkey_filename = DEF_SSHPUBKEY;
	ssh.privkey_filename = DEF_SSHPRIVKEY;
	ssh.privkey_passphrase = DEF_SSHPRIVKEYPASS;
	list = get_attribute_list(config, "SSH Configuration", &size, &status);
	if (list != NULL && status == 0) {
		for (name = list; *name; name++) {
			errornum = get_config_item("SSH Configuration", *name, config, &item);
			if (errornum) { /* This should not happen, but then again neither should have Trump's presidency... */
				fprintf(stderr, "Failed to retrieve configuration attribute %s: %s\n", *name, strerror(errornum));
			}
			else if (strcmp(*name, "UserName") == 0) {
				ssh.user_name = get_string_config_value(item, &errornum);
				if (errornum != 0) {
					fprintf(stderr, "Parse error while retrieving configuration attribute %s: %s\n", *name, strerror(errornum));
					ssh.user_name = DEF_SSHUSER;
				}
			}
			else if (strcmp(*name, "PublicKeyFileName") == 0) {
				ssh.pubkey_filename = get_string_config_value(item, &errornum);
				if (errornum != 0) {
					fprintf(stderr, "Parse error while retrieving configuration attribute %s: %s\n", *name, strerror(errornum));
					ssh.pubkey_filename = DEF_SSHPUBKEY;
				}
			}
			else if (strcmp(*name, "PrivateKeyFileName") == 0) {
				ssh.privkey_filename = get_string_config_value(item, &errornum);
				if (errornum != 0) {
					fprintf(stderr, "Parse error while retrieving configuration attribute %s: %s\n", *name, strerror(errornum));
					ssh.privkey_filename = DEF_SSHPRIVKEY;
				}
			}
			else if (strcmp(*name, "PrivateKeyPassphrase") == 0) {
				ssh.privkey_passphrase = get_string_config_value(item, &errornum);
				if (errornum != 0) {
					fprintf(stderr, "Parse error while retrieving configuration attribute %s: %s\n", *name, strerror(errornum));
					ssh.privkey_passphrase = DEF_SSHPRIVKEYPASS;
				}
			}
			else if (strcmp(*name, "SubsystemName") == 0) {
				ssh.subsys_name = get_string_config_value(item, &errornum);
				if (errornum != 0) {
					fprintf(stderr, "Parse error while retrieving configuration attribute %s: %s\n", *name, strerror(errornum));
					ssh.subsys_name = DEF_SSHSUBSYSNAME;
				}
			}
			else {
				fprintf(stderr, "Ignoring unrecognised key %s.\n", *name);
			}
		}
		free_attribute_list(list);
	}
	return ssh;
}

struct cfg_ctlsystems load_cfg_ctlsystems(struct collection_item *config) {
	struct cfg_ctlsystems ctlsystems;
	struct ctlsystem *cur;
	struct collection_item *item;
	char **list, **name, *system;
	int size, errornum;

	memset(&ctlsystems, '\0', sizeof(ctlsystems));

	list = get_attribute_list(config, "Control Systems", &size, &errornum);
	
	if (errornum != 0) {
		fprintf(stderr, "Failed to retrieve control system list: %s\n", strerror(errornum));
		if (list != NULL) /* Not entirely sure if get_attribute_list() returns NULL on error */
			free_attribute_list(list);
	}
	else {
		ctlsystems.list = (struct ctlsystem *)malloc(size * sizeof(struct ctlsystem));
		ctlsystems.size = size;
	}
	if (ctlsystems.list) {
		cur = ctlsystems.list;
		
		for (name = list; *name; name++) {
			memset(cur, '\0', sizeof(struct ctlsystem));
			
			cur->ctlsystem = (char *)malloc((strlen(*name) + 1) * sizeof(char));
			if (!cur->ctlsystem) {
				perror("Memory allocation failure whilst loading control system list element");
				continue;
			}
			
			errornum = get_config_item("Control Systems", *name, config, &item);
			/* An error should not happen, but then again 
			 * neither should have Trump's presidency... */
			if (errornum) { 
				fprintf(stderr, "Failed to retrieve system '%s' from system list: %s\n", *name, strerror(errornum));
				continue;
			}
			
			system = get_string_config_value(item, &errornum);
			if (errornum) {
				fprintf(stderr, "Failed to retrieve system '%s' from system list: %s\n", *name, strerror(errornum));
				continue;
			}
			
			printf("System name: %s\n", system);
			strcpy(cur->ctlsystem, *name);
			cur->ctlsystem_hostname = system;
			cur++;
		}
	}
	else if (errornum == 0) { /* then something broke with malloc, out of memory? */
		perror("Memory allocation failure whilst loading control system list");
	}
	return ctlsystems;
}

void unload_cfg_ctlsystems(struct cfg_ctlsystems ctlsystems) {
	int i;
	struct ctlsystem *list;
	list = ctlsystems.list;

	for (i = 0; i < ctlsystems.size; i++) {
		if (list[i].ctlsystem) free(list[i].ctlsystem);
		if (list[i].ctlsystem_hostname) free(list[i].ctlsystem_hostname);
	}
	free(ctlsystems.list);
	return;
}

struct cfg_tgtsystems load_cfg_tgtsystems(struct collection_item *config) {
	struct cfg_tgtsystems tgtsystems;
	struct tgtsystem *cur;
	struct collection_item *item;
	char **syslist, **keylist, **name, *system;
	int size_syslist, size_keylist, errornum_syslist, errornum_keylist, errornum;

	memset(&tgtsystems, '\0', sizeof(tgtsystems));

	syslist = get_attribute_list(config, "Target Systems", &size_syslist, &errornum_syslist);
	keylist = get_attribute_list(config, "Target System Keys", &size_keylist, &errornum_keylist);

	
	if (errornum_syslist != 0) {
		fprintf(stderr, "Failed to retrieve control system list: %s\n", strerror(errornum_syslist));
		errornum = errornum_syslist;
		if (syslist != NULL) /* Not entirely sure if get_attribute_list() returns NULL on error */
			free_attribute_list(syslist);
	}
	if (errornum_keylist != 0) {
		fprintf(stderr, "Failed to retrieve control system key list: %s\n", strerror(errornum_keylist));
		errornum = errornum_keylist;
		if (keylist != NULL) /* Not entirely sure if get_attribute_list() returns NULL on error */
			free_attribute_list(keylist);
	}
	if (size_syslist != size_keylist) {
		fputs("WARNING: Target system list length does not equal key list length.", stderr);
	}

	if (errornum == 0) {
		tgtsystems.list = (struct tgtsystem *)malloc(size_syslist * sizeof(struct tgtsystem));
		tgtsystems.size = size_syslist;
	}
	if (tgtsystems.list) {
		cur = tgtsystems.list;
		
		for (name = syslist; *name; name++) {
			memset(cur, '\0', sizeof(struct tgtsystem));
			printf("cur: 0x%X\n", cur);
			printf("cur->tgtsystem: 0x%X\n", cur->tgtsystem);
			printf("cur->tgtsystem_hostname: 0x%X\n", cur->tgtsystem_hostname);
			printf("System entry: %s\n", *name);

			cur->tgtsystem = (char *)malloc((strlen(*name) + 1) * sizeof(char));
			if (!cur->tgtsystem) {
				perror("Memory allocation failure whilst loading control system list element");
				continue;
			}
			
			errornum = get_config_item("Target Systems", *name, config, &item);
			/* An error should not happen, but then again 
			 * neither should have Trump's presidency... */
			if (errornum) { 
				fprintf(stderr, "Failed to retrieve system '%s' from target system list: %s\n", *name, strerror(errornum));
				continue;
			}
			
			system = get_string_config_value(item, &errornum);
			if (errornum) {
				fprintf(stderr, "Failed to parse system host name for '%s': %s\n", *name, strerror(errornum));
				if (system) free(system);
				continue;
			}
			
			errornum = get_config_item("Target System Keys", *name, config, &item);
			/* An error should not happen, but then again 
			 * neither should have Trump's presidency... */
			if (errornum) { 
				fprintf(stderr, "Failed to retrieve key for system '%s' from system key list: %s\n", *name, strerror(errornum));
				continue;
			}
			
			system = get_string_config_value(item, &errornum);
			if (errornum) {
				fprintf(stderr, "Failed to parse system host name for '%s': %s\n", *name, strerror(errornum));
				if (system) free(system);
				continue;
			}
			
			printf("System name: %s\n", system);
			strcpy(cur->tgtsystem, *name);
			cur->tgtsystem_hostname = system;
			cur++;
		}
	}
	else if (errornum == 0) { /* then something broke with malloc, out of memory? */
		perror("Memory allocation failure whilst loading control system list");
	}
	return tgtsystems;
}
