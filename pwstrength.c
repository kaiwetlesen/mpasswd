#include <string.h>
#include <ini_config.h>
#include "mpasswd.h"
#include "appconfig.h"
#include "pwstrength.h"

/*
int main(int argc, char **argv) {
	struct appconfig cfg;
	uint8_t requirements;
	int complexity;

	if (argc < 2 || !argv[1]) {
		printf("Usage: %s [password]\n", argv[0]);
		return 0;
	}

	cfg = load_cfg("/home/kmwetles/Projects/mpasswd/mpasswd.ini");
	
	printf("Complexity is: %d\n", cfg.pwcomplexity.complexity);
	printf("MinLowerAlpha is: %d\n", cfg.pwcomplexity.min_lower_alpha);
	printf("MinUpperAlpha is: %d\n", cfg.pwcomplexity.min_upper_alpha);
	printf("MinNumeric is: %d\n", cfg.pwcomplexity.min_numeric);
	printf("MinSpecial is: %d\n", cfg.pwcomplexity.min_special);
	
	printf("LowerAlpha credit is: %d\n", cfg.pwcredit.lower_alpha);
	printf("UpperAlpha credit is: %d\n", cfg.pwcredit.upper_alpha);
	printf("Numeric credit is: %d\n", cfg.pwcredit.numeric);
	printf("Special character credit is: %d\n", cfg.pwcredit.special);
	
	requirements = check_mins(argv[1], cfg.pwcomplexity);
	complexity = compute_complexity(argv[1], cfg.pwcredit);
	printf("Requirements met: 0x%02X\n",requirements);
	printf("Password complexity: %d\n", complexity);
}
*/

/* 
 * LIBRARY NOTES:
 * This is by no means an ideal way by which to check if a password is sufficiently
 * strong. A much better approach involves verifying if a password clusters among
 * commonly chosen passwords (often uncovered by breaches etc). See this article
 * for more information:
 * https://diogomonica.com/2014/10/11/password-security-why-the-horse-battery-staple-is-not-correct/
 * In the future, I'll write a library that incorporates these findings, as system
 * access passwords are intended to be memorised (for better or worse).
 */

uint8_t check_mins(char *passwd, struct cfg_pwcomplexity complexitycfg) {
	char *pw_c;
	int num_lalpha, num_ualpha, num_numeric, num_special;
	uint8_t result;

	num_lalpha = num_ualpha = num_numeric = num_special = 0;
	for (pw_c = passwd; *pw_c != '\0'; pw_c++) {
		if (*pw_c <= 'z' && *pw_c >= 'a') {
			num_lalpha++;
		}
		else if (*pw_c <= 'Z' && *pw_c >= 'A') {
			num_ualpha++;
		}
		else if (*pw_c <= '9' && *pw_c >='0') {
			num_numeric++;
		}
		else if (*pw_c < 0x7F && *pw_c > 0x20) { /* Treat other ASCII stuff as a special character. */
			num_special++;
		}
		else { /* Lower range control characters, spaces, exASCII, etc. */
			num_lalpha++; /* TODO: consider handling extended ASCII better... */
		}
	}

	/* Set up a flag set which indicates what passed and what didn't. */
	result = 0;
	if (num_lalpha >= complexitycfg.min_lower_alpha) {
		result |= HAS_MIN_LALPHA;
	}
	if (num_ualpha >= complexitycfg.min_upper_alpha) {
		result |= HAS_MIN_UALPHA;
	}
	if (num_numeric >= complexitycfg.min_numeric) {
		result |= HAS_MIN_NUMERIC;
	}
	if (num_special >= complexitycfg.min_special) {
		result |= HAS_MIN_SPECIAL;
	}
	if (strlen(passwd) >= complexitycfg.min_length) {
		result |= HAS_MIN_LENGTH;
	}
	return result;
}

int compute_complexity(char *passwd, struct cfg_pwcredit creditcfg) {
	char *pw_c;
	int complexity;

	complexity = 0;
	for (pw_c = passwd; *pw_c != '\0'; pw_c++) {
		if (*pw_c <= 'z' && *pw_c >= 'a') {
			complexity += creditcfg.lower_alpha;
		}
		else if (*pw_c <= 'Z' && *pw_c >= 'A') {
			complexity += creditcfg.upper_alpha;
		}
		else if (*pw_c <= '9' && *pw_c >='0') {
			complexity += creditcfg.numeric;
		}
		else if (*pw_c > 0x40 && *pw_c < 0x7F) { /* Treat other ASCII stuff as a special character. */
			complexity += creditcfg.special;
		}
		else { /* Lower range control characters, spaces, exASCII, etc. */
			complexity += creditcfg.lower_alpha; /* TODO: consider handling extended ASCII better... */
		}
	}
	return complexity;
}
