#include "mctp-ssh.h"
#include <libssh2.h>

LIBSSH2_CHANNEL *cur_channel;

void MCTP_SSH_set_channel(LIBSSH2_CHANNEL *channel) {
	cur_channel = channel;
	return;
}

int MCTP_read(char *payload) {
	return libssh2_channel_read(cur_channel, payload, MCTP_BUFLEN) == MCTP_BUFLEN;
}

int MCTP_write(char *payload) {
	return libssh2_channel_write(cur_channel, payload, MCTP_BUFLEN) == MCTP_BUFLEN;
}
