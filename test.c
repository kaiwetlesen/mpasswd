#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <libssh2.h>

int main(void) {
	struct addrinfo *hostaddr, *mover, hints;
	int result, connected;
	int socket_fd;
	int flags;

	LIBSSH2_SESSION *session;
	LIBSSH2_CHANNEL *channel;

	int errnum;
	char *errline;
	
	session = NULL;
	channel = NULL;
	errline = NULL;

	memset(&hints, '\0', sizeof(hints));
	hints.ai_flags = AI_CANONNAME;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	
	//result = getaddrinfo("ldap-primary.almaden.ibm.com", "ssh", &hints, &hostaddr);
	result = getaddrinfo("ldap-secondary01.almaden.ibm.com", "ssh", &hints, &hostaddr);
	//result = getaddrinfo("ldap-secondary02.sl.cloud9.ibm.com", "ssh", &hints, &hostaddr);
	//result = getaddrinfo("ldap-secondary03.sl.cloud9.ibm.com", "ssh", &hints, &hostaddr);
	//result = getaddrinfo("ldap-secondary04.sl.cloud9.ibm.com", "ssh", &hints, &hostaddr);
	if (result == 0) {
		for (mover = hostaddr, connected = 0; !connected && mover != NULL; mover = mover->ai_next) {

			socket_fd = socket(mover->ai_family, mover->ai_socktype, mover->ai_protocol);
			if (socket_fd < 0) {
				continue;
			}
			if (connect(socket_fd, mover->ai_addr, mover->ai_addrlen) == 0) {
				connected = 1; /* We connected successfully! Stop looking through returned addresses. */
			}
			else {
				close(socket_fd); /* Didn't work, close, move on... */
			}
		}
		freeaddrinfo(hostaddr);
	}

	/* If the connection failed here, bail. */
	if (!connected) {
		printf("Connection failed\n");
		if (socket_fd >= 0) close(socket_fd);
		exit(-1);
	}

	if (libssh2_init(0) != 0) {
		printf("SSH initialization failed -- Cannot continue.\n");
		exit(-1);
	}

	session = libssh2_session_init();

	/*
	 * Known issues:
	 * - Large number of memory leaks in v1.4.3 of LibSSH2
	 * - KEx fails on RHEL7 when SHA1 HMAC (hmac-sha1) is not available
	 */
	if (session == NULL || libssh2_session_handshake(session, socket_fd) != 0) {
		errnum = libssh2_session_last_error(session, &errline, NULL, 0);
		printf("Failed to establish a session: (%d) %s\n", errnum, errline);
		exit(-1);
	}
	else {
		printf("Session worked!\n");
	}

	int i;
	const char *fingerprint;
	fingerprint = libssh2_hostkey_hash(session, LIBSSH2_HOSTKEY_HASH_MD5);

	fprintf(stderr, "Fingerprint: ");
	for(i = 0; i < 16; i++) {
		fprintf(stderr, "%s%01x", i == 0 ? "MD5:" : ":", (unsigned char)fingerprint[i]);
	}
	fprintf(stderr, "\n");

	if (libssh2_userauth_publickey_fromfile(session, "kaiw", "/home/kaiw/.ssh/id_rsa.pub", "/home/kaiw/.ssh/id_rsa", "hello123") == 0) {
		printf("Authenticated successfully!\n");
	}
	else {
		errnum = libssh2_session_last_error(session, &errline, NULL, 0);
		printf("Could not authenticate: (%d) %s\n", errnum, errline);
	}

	char buffer[100];
	channel = libssh2_channel_open_session(session);
	if (channel) {
		puts("Channel open");

		/* Do more SSH stuff here... */
		//libssh2_channel_request_pty(channel, "xterm");
		//libssh2_channel_shell(channel);
		libssh2_channel_exec(channel, "ls /");

		//libssh2_channel_write(channel, "ls\n", 3);
		while (libssh2_channel_read(channel, buffer, 100) > 0) {
			printf("%s", buffer);
			memset(buffer, '\0', 100);
		}

		libssh2_channel_close(channel);
		libssh2_channel_free(channel);
	}
	else {
		puts("Channel failed to open");
	}
	
	
	if (session) {
		libssh2_session_disconnect(session, "SSH session closed");
		libssh2_session_free(session);
	}

	libssh2_exit();

	if (socket_fd >= 0) {
		close(socket_fd);
	}

	return 0;
}
