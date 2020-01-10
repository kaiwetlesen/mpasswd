#include "mctp.h"

/*************************************
 * MCTP_send_cred
 *
 * Sends a new credential to a target
 * system.
 *
 * from_host: Controlling host system
 * username:  Username that will recv
 *            new credential
 * passsword: New password to set
 *************************************/
int MCTP_send_cred(char *from_host, char *username, char *password) {
	char buffer[MCTP_BUFLEN];
	char *status_text, *message, *reject;
	long state;
	int result;

	/* Read first line from MCTP target, should be a status line. */
	while (memset(buffer, '\0', MCTP_BUFLEN) && MCTP_read(buffer)) {

		/* Segment the message into status code and message on receipt. */
		buffer[3] = '\0';
		status_code = buffer;
		message = buffer + 4;

		state = strtol(status_code, *reject, 10);
		if (reject) { /* The managed endpoint sent us garbage, bail */
			result = MCTP_PROTOCOL_ERR;
			break;
		}


	}
	return result;
}

int MCTP_recv_cred(void) {
}

int MCTP_recv(char *message, int message_len) {
	char buffer[MCTP_BUFLEN];
	char *status_code, *received_message, *extra;
	int status;
	long status_long;

	/* Read first line from MCTP target, should be a status line. */
	memset(buffer, '\0', MCTP_BUFLEN);
	MCTP_read(buffer);

	/* Divide the message into status code and message on receipt and parse. */
	buffer[3] = '\0';
	status_code = buffer;
	message = buffer + 4;
	status_long = strtol(status_code, &extra, 10);
	if (extra) {
		memset(message, '\0', message_len);
		status = 
	}
}
