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
}

int MCTP_recv_cred(void) {
	if      (strncmp(command, MCTP_CMD_HELO, 4) == 0) {
	}
	else if (strncmp(command, MCTP_CMD_AUTH, 4) == 0) {
	}
	else if (strncmp(command, MCTP_CMD_VRFY, 4) == 0) {
	}
	else if (strncmp(command, MCTP_CMD_CHPW, 4) == 0) {
	}
	else if (strncmp(command, MCTP_CMD_DATA, 4) == 0) {
	}
	else if (strncmp(command, MCTP_CMD_RSET, 4) == 0) {
	}
	else if (strncmp(command, MCTP_CMD_NOOP, 4) == 0) {
	}
	else if (strncmp(command, MCTP_CMD_QUIT, 4) == 0) {
	}
	else {
		
	}
}

int MCTP_ctlr_recv(char *message, int message_maxlen) {
	char buffer[MCTP_BUFLEN];
	char *status_code, *received_message, *extra;
	int status;
	long status_long;

	/* Read first line from MCTP target, should be a status line. */
	memset(buffer, '\0', MCTP_BUFLEN);
	MCTP_read(buffer);

	/* Divide the message into status code and message on receipt and parse. */
	buffer[3] = '\0';
	buffer[MCTP_BUFLEN - 1] = '\0';
	status_code = buffer;
	received_message = buffer + 4;
	status_long = strtol(status_code, &extra, 10);

	/* Return failure if garbage received */
	if (extra || status_long > INT_MAX) {
		status = MCTP_STAT_TRANSACTFAIL;
		strncpy(message, MCTP_MSG_TRANSACTFAIL, message_maxlen);
	}
	else {
		status = (int)status_long;
		strncpy(message, received_message, message_maxlen);
	}
	if (message_maxlen > 0) message[message_maxlen - 1] = '\0';
	return status;
}

int MCTP_trgt_recv(char *command, char *argument, int argument_maxlen) {
	char *buffer[MCTP_BUFLEN];
	char *cmd, *arg;
	int len;

	/* Read first line from MCTP target, should be a status line. */
	memset(buffer, '\0', MCTP_MSGLEN);
	MCTP_read(buffer);

	buffer[MCTP_CMDLEN - 1] = '\0'; /* Divide the received line into command and argument */
	cmd = buffer;
	arg = buffer + MCTP_CMDLEN;
	if (command)  strncpy(command,  cmd, MCTP_CMDLEN);
	if (argument) strncpy(argument, arg, argument_maxlen);
	if (message_maxlen > 0) argument[argument_maxlen - 1] = '\0';

	/* Indicate if we got a command that seems to be the right format... */
	return strnlen(cmd, MCTP_CMDLEN) == MCTP_CMDLEN;
}

/**
 * MCTP Controller Next State
 *
 * It determines the next state for the controller's state machine based
 * on the current state of the machine and the input flags. The state machine 
 * is a Moore model state machine where inputs are purely for decisional 
 * structures and do not affect the output.
 */
uint8_t MCTP_ctlr_next_state(uint8_t cur_state, uint32_t inputs) {
	switch (cur_state) {
		
	}
}

/**
 * MCTP Target Next State
 *
 * It determines the next state for the target's state machine based
 * on the current state and the input flags. The state machine is a Moore
 * model state machine where inputs are purely for decisional structures
 * and do not affect the output.
 *
 * cur_state : 8-bit unsigned integer indicative of the current protocol state
 *
 */
uint8_t MCTP_trgt_next_state(uint8_t cur_state, uint32_t inputs) {
	switch (cur_state) {
		case MCTP_TRGT_STATE_CONNECTING:
			break;
		case MCTP_TRGT_STATE_READY:
			break;
		case MCTP_TRGT_STATE_SHUTDOWN:
			break;
		case MCTP_TRGT_STATE_BEGIN_DATA:
			break;
	}
}

