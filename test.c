#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

	char line[101];

	memset(&hints, '\0', sizeof(hints));
	hints.ai_flags = AI_CANONNAME;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	
	result = getaddrinfo("ldap-secondary02.sl.cloud9.ibm.com", "ssh", &hints, &hostaddr);
	if (result == 0) {
		printf("LDAP host found!\n");
		for (mover = hostaddr, connected = 0; !connected && mover != NULL; mover = mover->ai_next) {
			printf("Canonical name: %s\n", mover->ai_canonname);
			printf("Sizeof struct sockaddr: %d\n", sizeof(struct sockaddr));

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
	
	//flags = fcntl(socket_fd, F_GETFL, 0);
	//fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK);

	if (libssh2_init(0) != 0) {
		printf("SSH initialization failed -- Cannot continue.\n");
	}

	/*while ((result = read(socket_fd, line, 100)) != 0) {
		printf("Received %d bytes\n", result);
		line[result] = '\0';
		puts(line);
	}
	printf("After while: Received %d bytes\n", result);*/

	session = libssh2_session_init();

	if (libssh2_session_handshake(session, socket_fd)) {
		printf("Failed to establish a session!\n");
	}
	else {
		printf("Session worked!\n");
	}

	int i;
	const char *fingerprint;
	fingerprint = libssh2_hostkey_hash(session, LIBSSH2_HOSTKEY_HASH_SHA1);

	fprintf(stderr, "Fingerprint: ");
	for(i = 0; i < 20; i++) {
		fprintf(stderr, "%02x", (unsigned char)fingerprint[i]);
	}
	fprintf(stderr, "\n");

	if (libssh2_userauth_publickey_fromfile(session, "kaiw", "/tmp/kaiw/id_rsa", "/tmp/kaiw/id_rsa.pub", "hello123") == 0) {
		printf("Authenticated successfully!\n");
	}
	else {
		printf("Could not authenticate.\n");
	}

	if (session) {
		libssh2_session_disconnect(session, "SSH session closed");
		libssh2_session_free(session);
	}

	if (socket_fd >= 0) {
		close(socket_fd);
	}

	return 0;
}
