#ifndef MCTP_H
#define MCTP_H

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

#define MCTP_CMD_HELO "HELO"
#define MCTP_CMD_AUTH "AUTH"
#define MCTP_CMD_VRFY "VRFY"
#define MCTP_CMD_CHPW "CHPW"
#define MCTP_CMD_DATA "DATA"
#define MCTP_CMD_RSET "RSET"
#define MCTP_CMD_NOOP "NOOP"
#define MCTP_CMD_QUIT "QUIT"

#define MCTP_STAT_READY         220
#define MCTP_STAT_SHUTTINGDOWN  221
#define MCTP_STAT_COMMANDOKAY   250
#define MCTP_STAT_BEGINDATA     354
#define MCTP_STAT_CONTINUEDATA  355
#define MCTP_STAT_UNAVAILABLE   421
#define MCTP_STAT_ABORTED       451
#define MCTP_STAT_COMMANDUNKNWN 500
#define MCTP_STAT_SYNTAXERROR   501
#define MCTP_STAT_WRONGSEQUENCE 503
#define MCTP_STAT_CONTROLREJECT 521
#define MCTP_STAT_ACCESSDENIED  530
#define MCTP_STAT_USERUNKNOWN   551
#define MCTP_STAT_TRANSACTFAIL  554

#define MCTP_MSG_TRANSACTFAIL "Transaction failed"

/* These constants most likely will not be used */
#define MCTP_CMDFORM_IMMEDIATE 1 /* CMD */
#define MCTP_CMDFORM_VARIANT   2 /* CMD ARGUMENT */
#define MCTP_CMDFORM_STREAMING 3 /* Streaming data header  */
#define MCTP_CMDFORM_STREAMDAT 4 /* Streaming data element */

int MCTP_send_cred(char *from_host, char *username, char *password);

int MCTP_recv_cred(void);

int MCTP_ctlr_send(char *message, int message_len);
int MCTP_ctlr_recv(char *message, int message_len);

#endif
