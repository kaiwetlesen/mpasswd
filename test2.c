#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "sshclient.h"

int main(void) {
	int socketfd, error;
	char *errorline;
	char buffer[100];
	LIBSSH2_SESSION *session;
	LIBSSH2_CHANNEL *channel;

	struct cfg_ssh cfg = {
		"kaiw", // cfg.user_name
		"/home/kaiw/.ssh/id_rsa", // cfg.privkey_filename
		"hello123", // cfg.privkey_passphrase
		"/home/kaiw/.ssh/id_rsa.pub", // cfg.pubkey_filename
		"mpasswd" // cfg.subsys_name
	};

	socketfd = connect_by_name("ldap-secondary01.almaden.ibm.com", "ssh");
	if (!socketfd) {
		perror("SSH connection failed");
		exit(-1);
	}
	session = ssh_connect(socketfd, cfg, &error, &errorline);
	if (!session) {
		printf("Could not connect: (%d) %s.\n", error, errorline);
		disconnect(socketfd);
		exit(error);
	}
	channel = ssh_open(session);
	if (!channel) {
		printf("Could not establish session: (%d) %s\n", error, errorline);
		ssh_disconnect(session);
		disconnect(socketfd);
		exit(errno);
	}
	/* Do SSH things here */
		libssh2_channel_subsystem(channel, "ls /");

		libssh2_channel_write(channel, "hello\n", 3);
		while (libssh2_channel_read(channel, buffer, 100) > 0) {
			printf("%s", buffer);
			memset(buffer, '\0', 100);
		}
		
	ssh_close(channel);
	#if 0
	#endif
	ssh_disconnect(session);
	disconnect(socketfd);
	return 0;
}
