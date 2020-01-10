#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "sshclient.h"

int main(void) {
	int socketfd, error, status;
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
		exit(EXIT_FAILURE);
	}
	session = ssh_connect(socketfd, cfg, &error, &errorline);
	if (!session) {
		printf("Could not connect: (%d) %s.\n", error, errorline);
		disconnect(socketfd);
		exit(EXIT_FAILURE);
	}
	channel = ssh_open(session);
	if (!channel) {
		printf("Could not establish session: (%d) %s\n", error, errorline);
		ssh_disconnect(session);
		disconnect(socketfd);
		exit(EXIT_FAILURE);
	}
	/* Do SSH things here */
		libssh2_channel_subsystem(channel, "list");

	/* Do MCTP things here */
		libssh2_channel_write(channel, "HELO lilo.almaden.ibm.com\n", 7);
		libssh2_channel_read(channel, buffer, 100);
		printf("%s", buffer);
		memset(buffer, '\0', 100);
		printf("Exiting...\n");
		libssh2_channel_write(channel, "exit", 5);
		printf("Final status: %d\n", status);
		
	ssh_close(channel);
	#if 0
	#endif
	ssh_disconnect(session);
	disconnect(socketfd);
	return EXIT_SUCCESS;
}
