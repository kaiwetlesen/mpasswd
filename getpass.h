#ifndef GETPASS_H
#define GETPASS_H

#define PWMAXLEN 1024

char *getpass(FILE *fd, char *prompt);
void freepass(char *password);
#endif
