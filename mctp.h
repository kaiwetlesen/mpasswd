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
#define MCTP_PROTCOL_ERR -1

#define MCTP_CLT_STATE_HELO 0
#define MCTP_CLT_STATE_AUTH 0
#define MCTP_CLT_STATE_VRFY 0

#define MCTP_STATE_TRANSITIONS {  }

int MCTP_send_cred(char *from_host, char *username, char *password);

int MCTP_recv_cred(void);

#endif
