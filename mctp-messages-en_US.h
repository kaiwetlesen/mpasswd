#ifndef MCTP_MESSAGES
#define MCTP_MESSAGES
#include "mctp.h"

/* Double expansion to enable insertion of status code into message string */
#define TOSTR1(S) #S
#define TOSTR(S) TOSTR1(S)

/*
 * Convention:
 * 
 * Each command can acknowledge or negatively acknowledge the input that it
 * receives. Depending on the condition, it will either send Code 250 (OK) with
 * a friendly message or a status code appropriate for the situation.
 *
 * There are some messages where it is useful to provide additional information
 * at runtime, and thus are inappropriate for use with fputs. These can be 
 * interpolated with sprintf and then to fputs or through fprintf, and are denoted
 * by the suffix '_F'.
 */

/* Generic system error messages: */
#define MCTP_MSG_COMMANDUNKNWN TOSTR(MCTP_CODE_COMMANDUNKWN) " Command unknown.\n"
#define MCTP_MSG_SYNTAXERROR TOSTR(MCTP_CODE_SYNTAXERROR) " Syntax error.\n"

/* Messages for commands that cannot negatively acknowledge: */
#define MCTP_MSG_NOOP TOSTR(MCTP_CODE_COMMANDOKAY) " OK"
#define MCTP_MSG_RESET TOSTR(MCTP_CODE_COMMANDOKAY) " Okay, resetting...\n"
#define MCTP_MSG_SHUTTINGDOWN TOSTR(MCTP_CODE_SHUTTINGDOWN) " Shutting down at your request. Thank you for using MCTP!\n"

#define MCTP_MSG_ACK_HELO_F TOSTR(MCTP_STAT_ACK_HELO) " Hello %s, pleased to meet you.\n"
#define MCTP_MSG_NACK_HELO_F TOSTR(MCTP_STAT_NACK_HELO) " Sorry %s, you are not allowed to control me.\n"

#define MCTP_MSG_ACK_AUTH_F TOSTR(MCTP_STAT_ACK_AUTH) " Okay, thank you.\n"
#define MCTP_MSG_NACK_AUTH_F TOSTR(MCTP_STAT_NACK_AUTH) " Sorry, key authorization failed.\n"

#define MCTP_MSG_ACK_VRFY TOSTR(MCTP_STAT_ACK_VRFY) " Oh hey, I know '%s'!\n"
#define MCTP_MSG_NACK_VRFY TOSTR(MCTP_STAT_NACK_VRFY) " My apologies, but I do not know '%s'.\n"

#define MCTP_MSG_ACK_CHPW_F TOSTR(MCTP_STAT_ACK_CHPW) " Change password for '%s' accepted. Please begin sending password DATA.\n"
#define MCTP_MSG_NACK_CHPW_F TOSTR(MCTP_STAT_NACK_CHPW) " My apologies, but I do not know '%s'.\n"

/* Messages for indicating that a precondition is not yet met (wrong sequence of commands usually): */
#define MCTP_MSG_REQ_HELO TOSTR(MCTP_STAT_REQ_HELO) " I do not know who you are, please say HELO first!\n"
#define MCTP_MSG_REQ_AUTH TOSTR(MCTP_STAT_REQ_AUTH) " Please authenticate yourself prior to requesting this command. Thanks!\n"
#define MCTP_MSG_REQ_VRFY TOSTR(MCTP_STAT_REQ_VRFY) " User is not verified. This should not have been checked during CHPW.\n"
#define MCTP_MSG_REQ_CHPW TOSTR(MCTP_STAT_REQ_CHPW) " You need to request a CHPW prior to providing data.\n"

/* Messages that are used during data transfer: */
#define MCTP_MSG_BGN_DATA TOSTR(MCTP_STAT_BGN_DATA " Okay, please begin sending data. End transfer with a '.' on its own line.\n"
#define MCTP_MSG_CTU_DATA TOSTR(MCTP_STAT_CTU_DATA) " Thank you, would you like to send more?\n"

/* Error messages: */
#define MCTP_MSG_EXC_DATA TOSTR(MCTP_STAT_EXC_DATA) " Unable to accept data transfer, data size exceeds receiving buffer.\n"

#define MCTP_MSG_TRANSACTFAIL "Transaction failed.\n"

#endif
