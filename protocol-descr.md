# Protocol Description
This file describes the mpasswd credential transfer protocol (MCTP). It is 
similar to SMTP with a set of commands and codes to direct the protocol flow.

All commands are exactly four alphabetic characters in length.

## Control Commands
All commands are sent by the controlling system to the target. They are
not sent by the target system back to the controlling host.
HELO [host name] : Establishes first contact with the MPasswd target and 
                   identifies the control system to the target via the 
                   `[host name]` parameter.
AUTH [secret]    : Sends the protocol authentication secret to the MPasswd
                   target for authentication.
VRFY [user name] : Verifies that a user account exists for `[user name]`.
                   Requires that the controlling host be authenticated using 
                   `AUTH`.
CHPW [user name] : Initiates a password change request for `[user name]`.
                   Requires that the controlling host be authenticated using
                   `AUTH`.
DATA             : Initiates data transfer sequence. This is used for 
                   transferring the new password to the target host. Data may
                   be written after target host affirms transfer. Data transfer
                   terminates when the control system writes a '.' (period)
                   character on its own line. Controlling systems must 
                   authenticate using `AUTH` and initiate a `CHPW` prior to 
                   using this command.
RSET             : Resets the MPasswd protocol on the target system. A control
                   system must re-authenticate and resend any required 
                   commands prior to continuing.
NOOP             : Performs no operation. 
QUIT             : Terminates the connection to the target host.

## Status Codes
All status are sent by the target system to the controller in response to an
action or command. They are not sent by the control system back to the target
host.
220 Ready          : Indicates that MCTP service is ready. Sent by target 
                     system to the control system upon successful connect.
221 Shutting Down  : MCTP is closing down the protocol.
250 Command Okay   : Requested MCTP command processed successfully.
354 Begin Data     : Indicates that the MCTP target system is ready to receive
                     initial password data transmission.
355 Continue Data  : Indicates that the MCTP target system is ready to receive
                     additional password data.
421 Unavailable    : Indicates that the MCTP service is unavailable or disabled.
451 Aborted Error  : Error in local processing of MCTP command caused command to
                     abort. See target system error log.
500 Command Unknwn : Command is unknown to MCTP or is invalid.
501 Syntax Error   : An error was encountered with the command syntax.
521 Control Reject : Indicates that the MCTP target system does not accept MCTP
                     requests from controlling host.
530 Access Denied  : MCTP target host has denied the target system's access to
                     the previous command, or authentication failed.
551 User Unknown   : Indicates that the user is unknown to the target system.
