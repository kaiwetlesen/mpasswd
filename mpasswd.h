#ifndef MPASSWD_H
#define MPASSWD_H

#define APPNAME "mpasswd"

#ifndef CFGFILE
#define CFGFILE "/etc/mpasswd/pwstrength.cfg"
#endif

struct cfg_pwcomplexity {
	int complexity;
	int min_lower_alpha;
	int min_upper_alpha;
	int min_numeric;
	int min_special;
};

struct cfg_pwcredit {
	int lower_alpha;
	int upper_alpha;
	int numeric;
	int special;
};

struct appconfig {
	struct cfg_pwcomplexity pwcomplexity;
	struct cfg_pwcredit pwcredit;
};

#endif
