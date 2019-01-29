#ifndef SSHCLIENT_H
#define SSHCLIENT_H
#include <libssh2.h>
#include "appconfig.h"
int connect_by_name(char *hostname, char *proto);
LIBSSH2_SESSION *ssh_connect(int socket_fd, struct cfg_ssh sshcfg, int *errnum, char **errmsg);
LIBSSH2_CHANNEL *ssh_open(LIBSSH2_SESSION *session);
void ssh_close(LIBSSH2_CHANNEL *channel);
void ssh_disconnect(LIBSSH2_SESSION *session);
void disconnect(int socket_fd);
#endif
