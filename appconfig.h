#ifndef APPCONFIG_H
#define APPCONFIG_H

struct cfg_pwcomplexity {
	int complexity;
	int min_length;
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

struct cfg_ssh {
	char *user_name;
	char *privkey_filename;
	char *privkey_passphrase;
	char *pubkey_filename;
	char *subsys_name;
};

struct cfg_ctlsystems {
	struct ctlsystem *list;
	int size;
};

struct ctlsystem {
	char *ctlsystem;
	char *ctlsystem_hostname;
};

struct cfg_tgtsystems {
	struct tgtsystem *list;
	int size;
};

struct tgtsystem {
	char *tgtsystem;
	char *tgtsystem_hostname;
	char *tgtsystem_key;
};

struct appconfig {
	struct cfg_pwcomplexity pwcomplexity;
	struct cfg_pwcredit pwcredit;
	struct cfg_ssh ssh;
	struct cfg_ctlsystems ctlsystems;
	char *system_key;
	int is_ctl_system;
};

struct appconfig load_cfg(char *cfg_file);
void unload_cfg(struct appconfig cfg);

#endif
