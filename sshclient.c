#include <stdio.h>
#include <stdlib.h>
//#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <libssh2.h>
#include "appconfig.h"

/* Connect-by-Name
 *
 * Establishes then connects a socket to a named 
 * system and protocol. Sets errno if final 
 * operation is a failure.
 */
int connect_by_name(char *hostname, char *proto) {
	struct addrinfo *hostaddr, *mover, hints;
	int result, connected;
	int socket_fd;
	
	memset(&hints, '\0', sizeof(hints));
	hints.ai_flags = AI_CANONNAME;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	
	result = getaddrinfo(hostname, proto, &hints, &hostaddr);
	
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
	else {
		socket_fd = -1;
	}
	return socket_fd;
}

	//socket = connect_by_name("ldap-primary.almaden.ibm.com", "ssh");
	//socket = connect_by_name("ldap-secondary01.almaden.ibm.com", "ssh");
	//socket = connect_by_name("ldap-secondary02.sl.cloud9.ibm.com", "ssh");
	//socket = connect_by_name("ldap-secondary03.sl.cloud9.ibm.com", "ssh");
	//socket = connect_by_name("ldap-secondary04.sl.cloud9.ibm.com", "ssh");
	
	/* If the connection failed here, bail. */
	/*if (socket < 0) {
		printf("Connection failed\n");
		exit(-1);
	}*/


	/* Part of bootup process */
	/*
	errnum = libssh2_init(0);
	if (errnum != 0) {
		//printf("SSH initialization failed -- Cannot continue.\n");
		//exit(-1);
		return errnum;
	}
	*/
	/* Part of shutdown process */
	/*
	libssh2_exit();
	*/


LIBSSH2_SESSION *ssh_connect(int socket_fd, struct cfg_ssh sshcfg, int *errnum, char **errmsg) {
	/*
	 * Known issues:
	 * - Large number of memory leaks in v1.4.3 of LibSSH2
	 * - KEx fails on RHEL7 when SHA1 HMAC (hmac-sha1) is not available
	 */
	LIBSSH2_SESSION *session;
	int *error, errdiscard;

	session = NULL;

	error = errnum != NULL ? errnum : &errdiscard; /* Set up a discardable error */
	*error = 0;

	/* Try to set up a session and handshake, bail if things go sideways */
	session = libssh2_session_init();
	session != NULL && (*error = libssh2_session_handshake(session, socket_fd));

	*error == 0 && (*error = libssh2_userauth_publickey_fromfile(session, sshcfg.user_name,
	    sshcfg.pubkey_filename, sshcfg.privkey_filename, sshcfg.privkey_passphrase));
	
	if (*error != 0 && session != NULL) {
		libssh2_session_last_error(session, errmsg, NULL, 0);
		libssh2_session_disconnect_ex(session, SSH_DISCONNECT_NO_MORE_AUTH_METHODS_AVAILABLE, "mpasswd authentication failed", "en_US");
		libssh2_session_free(session);
		session = NULL;
	}

	return session;
}

LIBSSH2_CHANNEL *ssh_open(LIBSSH2_SESSION *session) {
	return libssh2_channel_open_session(session);
}

void ssh_close(LIBSSH2_CHANNEL *channel) {
	libssh2_channel_flush(channel);
	libssh2_channel_flush_stderr(channel);
	libssh2_channel_close(channel);
	libssh2_channel_free(channel);
}

void ssh_disconnect(LIBSSH2_SESSION *session) {
	libssh2_session_disconnect_ex(session, SSH_DISCONNECT_BY_APPLICATION, "SSH session closed", "en_US");
	libssh2_session_free(session);
}

void disconnect(int socket_fd) {
	shutdown(socket_fd, SHUT_RDWR);
	close(socket_fd);
}
