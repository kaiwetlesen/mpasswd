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

/*
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
*/

int MCTP_ctlr_send_cmnd(char *cmd, char *arg) {
	char buffer[MCTP_BUFLEN];
	char *destcmd, *destarg;
	
	memset(buffer, '\0', MCTP_BUFLEN);
	destcmd = buffer;
	destarg = buffer + MCTP_CMDLEN;

	if (cmd && strlen(cmd) < MCTP_CMDLEN) {
		strncpy(destcmd, cmd, MCTP_CMDLEN - 1);
	}
	else {
		/* Fast fail if command is too long */
		return -1;
	}

	/* Set up the output buffer correctly */
	if (arg && strlen(arg) < MCTP_ARGLEN) {
		strncpy(destarg, arg, MCTP_ARGLEN);
		buffer[MCTP_CMDLEN] = ' ';
		buffer[MCTP_BUFLEN] = '\n';
		buffer[MCTP_BUFLEN] = '\0';
	}
	else if (!arg) {
		buffer[MCTP_CMDLEN] = '\n';
		buffer[MCTP_CMDLEN] = '\0';
	}
	else {
		return -1; /* We can't do this... sorry! */
	}

	return MCTP_write(buffer);
}

int MCTP_trgt_recv_cmnd(char *command, char *argument, int argument_maxlen) {
	char *buffer[MCTP_BUFLEN];
	char *cmd, *arg;
	int len;

	/* Read first line from MCTP target, should be a status line. */
	memset(buffer, '\0', MCTP_BUFLEN);
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

/* Messages are pre-computed in the MCTP header macros */
int MCTP_trgt_send_stat(char *message) {
	return MCTP_write(message);
}

int MCTP_ctlr_recv_stat(char *message, int message_maxlen) {
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
		status = MCTP_CODE_TRANSACTFAIL;
		strncpy(message, MCTP_MSG_TRANSACTFAIL, message_maxlen);
	}
	else {
		status = (int)status_long;
		strncpy(message, received_message, message_maxlen);
	}
	/* Ensure the message is null-terminated */
	message[message_maxlen - 1] = '\0';
	return status;
}

/*
 * Sends data to an MCTP target, one chunk at a time.
 * Returns the number of bytes sent.
 * NOTE: The number of bytes sent may exceed the length
 * of the input data set.
 */
int MCTP_ctlr_send_data(char *data) {
	char *buffer[MCTP_BUFLEN];
	char *mark, *end;
	int bytes_sent;
	success = MCTP_read(buffer) < 0;
	if (!success) {
		bytes_sent = -1;
	}
	else if (strcmp(buffer, MCTP_MSG_BGN_DATA) != 0) {
		bytes_sent = -1;
	}
	else {
		end = data + strlen(data);
		for (mark = data; mark < end; mark += MCTP_BUFLEN) {
			
		}
	}
}

/* 
 * Receives one chunk of MCTP data during an MCTP 
 * data transfer.
 */
int MCTP_trgt_recv_data(char *data, int data_maxlen) {
	char *buffer[MCTP_BUFLEN];
	char *dest;
	int len, chunklen, chunkmaxlen;

 	/* Set up to begin copying into the given data space */
	memset(data, '\0', data_maxlen);
	dest = data;
	MCTP_write(MCTP_MSG_BGN_DATA);

	while (len < data_maxlen) {
		/* Clean the buffers */
		memset(buffer, '\0', MCTP_BUFLEN);
		MCTP_read(buffer);

		if (strcmp(buffer, ".\n") == 0) { /* Period on its own line */
			len = data_maxlen;
		}
		else {
			/* Copy in the data chunk. Drop bytes in data chunk if exceeds length. */
			chunklen = strlen(buffer);
			chunkmaxlen = data_maxlen - len;
			chunklen = chunklen < chunkmaxlen ? chunklen : chunkmaxlen;
			len += chunklen;
			strncpy(data, buffer, chunklen - 2); // Exclude the newline
			/* Inform the client we've received and they may send more. */
			MCTP_write(MCTP_MSG_CTU_DATA);
		}
	}
	return len;
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
 * MCTP Target Process Command
 *
 * It processes the current command based on the current state of the
 * target machine, then advances the state based on the decision structure.
 * The state machine is a Moore model state machine where the next state
 * is purely a function of the command and current state.
 * 
 * Input:
 * - State cur_state : The current state of the machine.
 * - char *cmd       : The command to process.
 * - char *cmdarg    : Any arguments to the command. Passed to command 
 *                     processor subroutines if required.
 *
 * Output            : The next state of the machine.:
 *
 * Note: This function fast-fails at the FAST FAIL point.
 */
State MCTP_trgt_process_cmd(State state, char *cmd, char *cmdarg) {
	int syncheck;
	
	syncheck = 0;

	/* 
	 * FAST FAIL: Verify that arg is set for all variant commands. 
	 * Return current state if check fails.
	 */
	switch (CMD(cmd)) {
		case MCTP_CMD_HELO:
		case MCTP_CMD_AUTH:
		case MCTP_CMD_VRFY:
		case MCTP_CMD_CHPW:
			syncheck = strlen(cmdarg) > 1;
			if (!syncheck) {
				fputs(MCTP_MSG_SYNTAXERROR, stdout);
				return state; /* The syntax is invalid, fast-fail out. */
			}
		default:
			syncheck = 1;
	}
	
	switch (command) {
		case MCTP_CMD_HELO:
			state.field.helo_ok = process_helo(cmdarg);
			if (state.field.helo_ok) {
				fprintf(stdout, MCTP_MSG_ACK_HELO_F, cmdarg);
			}
			else {
				fprintf(stdout, MCTP_MSG_NACK_HELO_F, cmdarg);
			}
			break;
		case MCTP_CMD_AUTH:
			if (state.field.helo_ok) {
				state.field.auth_ok = process_auth(cmdarg);
				if (state.field.auth_ok) {
					fputs(MCTP_MSG_ACK_AUTH, stdout);
				}
				else {
					fputs(MCTP_MSG_NACK_AUTH, stdout);
				}
			}
			else {
				fputs(MCTP_MSG_REQ_HELO, stdout);
			}
			break;
		case MCTP_CMD_VRFY:
			if (state.field.helo_ok && state.field.auth_ok) {
				if (process_vrfy(cmdarg)) {
					fprintf(stdout, MCTP_MSG_ACK_VRFY_F, cmdarg);
				}
				else {
					fprintf(stdout, MCTP_MSG_NACK_VRFY_F, cmdarg);
				}
			}
			else if (state.field.helo_ok) {
				fputs(MCTP_MSG_REQ_AUTH, stdout);
			}
			else {
				fputs(MCTP_MSG_REQ_HELO, stdout);
			}
			break;
		case MCTP_CMD_CHPW:
			if (state.field.helo_ok && state.field.auth_ok) {
				// Reverify the user (just in case)
				
				state.field.vrfy_ok = process_vrfy(cmdarg);
				if (state.field.vrfy_ok) { // Then enable chpw once DATA recvd
					state.field.chpw_run = 1;
					fprintf(stdout, MCTP_MSG_ACK_CHPW, cmdarg);
				}
				else {
					fprintf(stdout, MCTP_MSG_NACK_CHPW, cmdarg);
				}
			}
			else if (state.field.helo_ok) {
				fputs(MCTP_MSG_REQ_AUTH, stdout);
			}
			else {
				fputs(MCTP_MSG_REQ_HELO, stdout);
			}
		case MCTP_CMD_DATA:
			/* If we're here, then everything better be in line! */
			if (state.field.helo_ok && 
			    state.field.auth_ok && 
			    state.field.vrfy_ok && 
			    state.field.chpw_ok) {
				// Gather up the password data then change the password!
			} /* Otherwise provide a narrowed status as to what to fix */
			else if (state.field.helo_ok && 
			         state.field.auth_ok && 
			         state.field.vrfy_ok) {
				fputs(MCTP_MSG_REQ_CHPW, stdout);
			}
			else if (state.field.helo_ok && 
			         state.field.auth_ok) {
				fputs(MCTP_MSG_REQ_VRFY, stdout);
			}
			else if (state.field.helo_ok) {
				fputs(MCTP_MSG_REQ_AUTH, stdout);
			}
			else {
				fputs(MCTP_MSG_REQ_HELO, stdout);
			}
		case MCTP_CMD_RSET:
			state.value = 0x0;
			fputs(MCTP_MSG_RESET, stdout);
		case MCTP_CMD_NOOP:
			fputs(MCTP_MSG_NOOP, stdout);
		case MCTP_CMD_QUIT:
			state.value = 0x0; /* Reset state machine then close down ops */
			state.field.closing_down = 1;
			fputs(MCTP_MSG_SHUTTINGDOWN, stdout);
		default:
			fputs(MCTP_MSG_COMMANDUNKNWN, stdout);
	}
	return state;
}

