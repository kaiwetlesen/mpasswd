#ifndef GETPASS_H
#define GETPASS_H

char *getpass(FILE *fd, char *prompt);
void freepass(char *password);
#endif
