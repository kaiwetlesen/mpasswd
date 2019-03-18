#ifndef MCTP_H
#define MCTP_H

/*
 * REQUIRED LIBRARIES
 *
 * These libraries are required for the proper functioning
 * of MCTP.
 */
#include <stdint.h>

/*
 * CONFIGURABLE ADAPTORS
 *
 * MCTP may be configured with any supported adaptor.
 * Currently, only SSH is supported. Additional transport
 * protocols may be supported by addition of adaptor
 * libraries.
 */

#ifdef USE_MCTP_SSH
/* Configures as SSH subsystem. */
#include "mctp-ssh.h"
#elif USE_MCTP_TLS
/* Library currently doesn't exist, please do not use. */
#include "mctp-tls.h"
#endif

/*
 * Per MCTP specification, maximum buffer length
 * is 128 characters, including command, any 
 * arguments, newline, and null terminator.
 */
#define MCTP_BUFLEN 128
#define MCTP_CMDLEN 5 /* Four characters plus null terminator */
#define MCTP_MSGLEN MCTP_BUFLEN - MCTP_CMDLEN + 2 /* +2 to account for addition */

#define CMD(cmdtext) ((cmdtext)[0] | (cmdtext)[1] << 8 | (cmdtext)[2] << 16 | (cmdtext)[3] << 24)

#define MCTP_CMDTXT_HELO "HELO"
#define MCTP_CMDTXT_AUTH "AUTH"
#define MCTP_CMDTXT_VRFY "VRFY"
#define MCTP_CMDTXT_CHPW "CHPW"
#define MCTP_CMDTXT_DATA "DATA"
#define MCTP_CMDTXT_RSET "RSET"
#define MCTP_CMDTXT_NOOP "NOOP"
#define MCTP_CMDTXT_QUIT "QUIT"

#define MCTP_CMD_HELO CMD(MCTP_CMDTXT_HELO)
#define MCTP_CMD_AUTH CMD(MCTP_CMDTXT_AUTH)
#define MCTP_CMD_VRFY CMD(MCTP_CMDTXT_VRFY)
#define MCTP_CMD_CHPW CMD(MCTP_CMDTXT_CHPW)
#define MCTP_CMD_DATA CMD(MCTP_CMDTXT_DATA)
#define MCTP_CMD_RSET CMD(MCTP_CMDTXT_RSET)
#define MCTP_CMD_NOOP CMD(MCTP_CMDTXT_NOOP)
#define MCTP_CMD_QUIT CMD(MCTP_CMDTXT_QUIT)

#define MCTP_CODE_READY         220
#define MCTP_CODE_SHUTTINGDOWN  221
#define MCTP_CODE_COMMANDOKAY   250
#define MCTP_CODE_BEGINDATA     354
#define MCTP_CODE_CONTINUEDATA  355
#define MCTP_CODE_UNAVAILABLE   421
#define MCTP_CODE_ABORTED       451
#define MCTP_CODE_COMMANDUNKNWN 500
#define MCTP_CODE_SYNTAXERROR   501
#define MCTP_CODE_WRONGSEQUENCE 503
#define MCTP_CODE_CONTROLREJECT 521
#define MCTP_CODE_ACCESSDENIED  530
#define MCTP_CODE_USERUNKNOWN   551
#define MCTP_CODE_TRANSACTFAIL  554

#define MCTP_STAT_ACK_HELO MCTP_CODE_COMMANDOKAY
#define MCTP_STAT_NACK_HELO MCTP_CODE_CONTROLREJECT

#define MCTP_STAT_ACK_AUTH MCTP_CODE_COMMANDOKAY
#define MCTP_STAT_NACK_AUTH MCTP_CODE_ACCESSDENIED

#define MCTP_STAT_ACK_VRFY MCTP_CODE_COMMANDOKAY
#define MCTP_STAT_NACK_VRFY MCTP_CODE_USERUNKNOWN

#define MCTP_STAT_ACK_CHPW MCTP_CODE_COMMANDOKAY
#define MCTP_STAT_NACK_CHPW MCTP_CODE_USERUNKNOWN

#define MCTP_STAT_REQ_HELO MCTP_CODE_WRONGSEQUENCE
#define MCTP_STAT_REQ_AUTH MCTP_CODE_WRONGSEQUENCE
#define MCTP_STAT_REQ_VRFY MCTP_CODE_WRONGSEQUENCE
#define MCTP_STAT_REQ_CHPW MCTP_CODE_WRONGSEQUENCE

#define MCTP_MSG_TRANSACTFAIL "Transaction failed"

/* These constants most likely will not be used */
#define MCTP_CMDFORM_IMMEDIATE 1 /* CMD */
#define MCTP_CMDFORM_VARIANT   2 /* CMD ARGUMENT */
#define MCTP_CMDFORM_STREAMING 3 /* Streaming data header  */
#define MCTP_CMDFORM_STREAMDAT 4 /* Streaming data element */

typedef struct state_fields {
	uint32_t helo_ok : 1;
	uint32_t auth_ok : 1;
	uint32_t vrfy_ok : 1;
	uint32_t chpw_run : 1;
	uint32_t streaming_enabled : 1;
	uint32_t closing_down : 1;
} StateFields;

typedef union state {
	uint32_t value;
	StateFields field;
} State;

int MCTP_send_cred(char *from_host, char *username, char *password);

int MCTP_recv_cred(void);

int MCTP_ctlr_send(char *message, int message_len);
int MCTP_ctlr_recv(char *message, int message_len);

#endif
