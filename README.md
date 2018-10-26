# MPasswd
MPasswd is a utility that allows a user to change their password on multiple
UNIX systems at once. MPasswd is designed to run as an SSh subsystem. It
incorporates some basic password strength checking typical to some conventional
IT policies.

When a user invokes MPasswd, it collects their current password and
authenticates them against the machine where they are logged in. Once
authenticated, this application then connects to each configured system via SSH
and resets the user's password with a configured chpass utility.

MPasswd references to a configuration file which, by default, is stored in 
/etc/mpasswd/mpasswd.conf and uses SSH keys stored in the containing directory.
