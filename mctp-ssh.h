#ifdef USE_MCTP_SSH
/**************************************
 * MCTP over LibSSH
 *
 * Adaptor library which enables use of 
 * MCTP over SSH using LibSSH2 via 
 * LibSSHClient.
 **************************************/

#ifndef MCTP_SSH_H
#define MCTP_SSH_H
#include "mctp.h"
#include <libssh2.h>

// Sets up MCTP to use current open SSH channel
int MCTP_SSH_set_channel(LIBSSH2_CHANNEL *channel);

// Interfaces MCTP read/write with libssh2_channel_read/write
int MCTP_write(char *payload);
int MCTP_read(char *payload); // Obeys MCTP maximum buffer lengths

#endif
#endif
