#include <string.h>
#include <ini_config.h>
#include "mpasswd.h"

#define ERR_BREAK 1
#define ERR_CONT  0

/* Complexity defaults: */
/*
 * My philosophy:
 * - Longer is better (to a reasonable limit)
 * - Do not enforce arbitrary character requirements
 * - Count all characters equally for complexity
 * - "correct Horse Battery staple"
 */
#define DEF_COMPLEXITY 15
#define DEF_MIN_LOWER_ALPHA 2
#define DEF_MIN_UPPER_ALPHA 0
#define DEF_MIN_NUMERIC 0
#define DEF_MIN_SPECIAL 0

/* Default to treating all characters equally! */
#define DEF_LOWER_ALPHA 1
#define DEF_UPPER_ALPHA 1
#define DEF_NUMERIC 1
#define DEF_SPECIAL 1

struct appconfig load_cfg(char *cfg_file);

int main(void) {
	load_cfg("/home/kmwetles/Projects/mpasswd/mpasswd.ini");
}

struct appconfig load_cfg(char *cfg_file) {
	char *filename, **name, **list;
	struct collection_item *config, *errors, *item;
	int status, size, errornum;

	struct cfg_pwcomplexity pwcomplexity;
	struct cfg_pwcredit pwcredit;
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

	list = get_attribute_list(config, "Password Complexity Requirements", &size, &status);
	if (status == 0) {
		for (name = list; *name; name++) {
			errornum = get_config_item("Password Complexity Requirements", *name, config, &item);
			if (errornum) { /* This should not happen, but then again neither should Trump's presidency... */
				fprintf(stderr, "Failed to retrieve configuration attribute %s: %s\n", *name, strerror(errornum));
			}
			else if (strcmp(*name, "Complexity") == 0) {
				pwcomplexity.complexity = get_int_config_value(item, ERR_CONT, DEF_COMPLEXITY, &errornum);
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
	}
	else {
		fputs("No 'Password Complexity Requirements' section found. Setting default values.\n", stderr);
		pwcomplexity.complexity = DEF_COMPLEXITY;
		pwcomplexity.min_lower_alpha = DEF_MIN_LOWER_ALPHA;
		pwcomplexity.min_upper_alpha = DEF_MIN_UPPER_ALPHA;
		pwcomplexity.min_numeric = DEF_MIN_NUMERIC;
		pwcomplexity.min_special = DEF_MIN_SPECIAL;
	}

	list = get_attribute_list(config, "Password Complexity Credit", &size, &status);
	if (status == 0) {
		for (name = list; *name; name++) {
			errornum = get_config_item("Password Complexity Credit", *name, config, &item);
			if (errornum) { /* This should not happen, but then again neither should Trump's presidency... */
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
	}
	else {
		fputs("No 'Password Complexity Credit' section found. Setting default values.\n", stderr);
		pwcredit.lower_alpha = DEF_LOWER_ALPHA;
		pwcredit.upper_alpha = DEF_UPPER_ALPHA;
		pwcredit.numeric = DEF_NUMERIC;
		pwcredit.special = DEF_SPECIAL;
	}

	printf("Complexity is: %d\n", pwcomplexity.complexity);
	printf("MinLowerAlpha is: %d\n", pwcomplexity.min_lower_alpha);
	printf("MinUpperAlpha is: %d\n", pwcomplexity.min_upper_alpha);
	printf("MinNumeric is: %d\n", pwcomplexity.min_numeric);
	printf("MinSpecial is: %d\n", pwcomplexity.min_special);

	printf("LowerAlpha credit is: %d\n", pwcredit.lower_alpha);
	printf("UpperAlpha credit is: %d\n", pwcredit.upper_alpha);
	printf("Numeric credit is: %d\n", pwcredit.numeric);
	printf("Special character credit is %d\n", pwcredit.special);
	configuration.pwcredit = pwcredit;
	configuration.pwcomplexity = pwcomplexity;
}
