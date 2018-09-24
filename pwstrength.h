#ifndef PWSTRENGTH_H
#define PWSTRENGTH_H

#define HAS_MIN_LALPHA  0x01
#define HAS_MIN_UALPHA  0x02
#define HAS_MIN_NUMERIC 0x04
#define HAS_MIN_SPECIAL 0x08
#define HAS_MIN_LENGTH  0x10
#define HAS_ALL_REQS    0x1F

uint8_t check_mins(char *passwd, struct cfg_pwcomplexity complexitycfg);
int compute_complexity(char *passwd, struct cfg_pwcredit creditcfg);

#endif
