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

