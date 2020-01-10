#ifndef GETPASS_H
#define GETPASS_H

#define PWMAXLEN 1024

/* Apparently, getpass collides with a function in the unistd.h library... */
char *getpass_fd(FILE *fd, char *prompt);
void freepass(char *password);
void echo_off(void);
void echo_on(void);
#endif
