# Multisystem Credential Transfer Protocol (MCTP)
## Document Purpose
This file describes the multisystem credential transfer protocol (MCTP). This 
includes a description of the protocol, its purpose, as well as the commands and
status codes required for full functionality.

## Protocol Description
MCTP is an application layer protocol that facilitates delivery of new 
credentials from a sanctioned control system to a target system. is similar 
to SMTP with a set of fixed commands and codes to direct the protocol flow. 
It is designed to run atop any secured and authenticated transport protocol 
such as TLS or SSH.

Note: MCTP may be implemented using raw sockets, but this is highly 
discouraged as MCTP currently does not provide payload encryption.

MCTP is a symmetric stateful protocol where the target systems returns a
status code in confirmation of each transmission from the control system. This 
includes input data transmissions sent as part of a streaming command. It is
strongly encouraged to reference the status after each attempted transmission.

### Example Protocol Flow
This example shows how a full conversation might happen between an MCTP control
host and a target host. In a production setting it is preferred that 
communication steps be reduced as much as possible.

- CS indicates transmission from control system to target system.
- TS indicates a transmission from target system to control system.

 1. `CS`: Secure Connect
 2. `TS`: 220 MCTP Ready
 3. `CS`: HELO controlsystem.example.com
 4. `TS`: 250 Hello controlsystem.example.com, pleased to meet you. I am targetsystem.other.domain.com.
 5. `CS`: AUTH wrongkeyhere
 6. `TS`: 503 Sorry, key authorization failed.
 7. `CS`: AUTH correcttargetkeyhere
 8. `TS`: 250 OK
 9. `CS`: VRFY someuser
10. `TS`: 551 Username 'someuser' not known.
11. `CS`: VRFY kaiwetlesen
12. `TS`: 250 Oh hey, I know 'kaiwetlesen'!
13. `CS`: CHPW unknownuser
14. `TS`: 551 I do not know 'unknownuser'. Change password rejected.
13. `CS`: CHPW otherknownuser
14. `TS`: 250 Change password request for 'otherknownuser' accepted. Please begin sending password DATA.
15. `CS`: RSET
16. `TS`: 250 Okay, resetting...
17. `CS`: CHPW kaiwetlesen
18. `TS`: 250 Change password request for 'kaiwetlesen' accepted. Please begin sending password DATA.
19. `CS`: DATA
20. `TS`: 354 Okay, please begin sending data. End transfer with a '.' on its own line.
21. `CS`: _password data here_
22. `TS`: 355 Thank you, would you like to send more?
23. `CS`: .
24. `TS`: 250 Okay thank you. Password change initiated.
25. `CS`: QUIT
26. `TS`: 221 Shutting down at your request. Thank you for using MCTP!

All commands, command parameters, status codes, and status detail lines must
be sent using single-byte characters in the first page of the UTF-8 character 
set. This includes all characters in the first 127 characters of the ISO 
Latin-1 character set. Data may be sent in any supported character format.

## Control Commands
All commands are sent by the controlling system to the target. They are not
sent by the target system back to the controlling host. Variant commands are
separated from their arguments by a _single space character_ (0x20 in UTF-8).

### General Command Format
|   Type    |                  Format                      |
| --------- | -------------------------------------------- |
| Immediate | `^CMDW\n$`                                   |
|  Variant  | `^CMDW ARGUMENT\n$`                          |
| Streaming | `^CMDW\nInput data\n[More input data\n].\n$` |

^: Beginning of input.

$: End of input.

CMDW: Four character command identifier (see Table 1).

ARGUMENT: A free-form string terminated by a new-line.

\n: Newline character (UTF-8 character 0x0A).

### Table 1: Control Commands

|     Command      |   Type    |                       Description                           |
| ---------------- | --------- | ----------------------------------------------------------- |
| HELO [host name] |  Variant  | Establishes first contact with the MPasswd target and       |
|                  |           | `[host name]` parameter. Target host may wish to verify     |
|                  |           | this argument against the control system's connection.      |
| AUTH [secret]    |  Variant  | Sends the protocol authentication secret to the MPasswd     |
|                  |           | target for authentication.                                  |
| VRFY [user name] |  Variant  | Verifies that a user account exists for `[user name]`.      |
|                  |           | Requires that the controlling host be authenticated using   |
|                  |           | `AUTH`.                                                     |
| CHPW [user name] |  Variant  | Initiates a password change request for `[user name]`.      |
|                  |           | Requires that the controlling host be authenticated using   |
|                  |           | `AUTH`.                                                     |
| DATA             | Streaming | Initiates data transfer sequence. This is used for          |
|                  |           | transferring the new password to the target host. Data may  |
|                  |           | be written after target host affirms transfer. Data transfer|
|                  |           | terminates when the control system writes a '.' (period)    |
|                  |           | character on its own line. Controlling systems must         |
|                  |           | authenticate using `AUTH` and initiate a `CHPW` prior to    |
|                  | Immediate | using this command.                                         |
| RSET             |           | Resets the MPasswd protocol on the target system. A control |
|                  |           | system must re-authenticate and resend any required         |
|                  |           | commands prior to continuing.                               |
| NOOP             | Immediate | Performs no operation.                                      |
| QUIT             | Immediate | Terminates the connection to the target host.               |

## Status Codes
All status are sent by the target system to the controller in response to an
action or command. They are not sent by the control system back to the target
host.

A status code is returned for each and every line of input entered by the
control system. It is up to the protocol implementer to supply short and more
detailed descriptions via a lookup table (see Table 2). Detailed descriptions
are free-form and not meant to be used for anything more than displaying to
the user.

All status codes are sent as stringified three-digit integers followed by one
UTF-8 space and a brief detail line. All returned characters are fixed size 
at one byte per character streams. Status detail lines are permitted to be any
composition of 2 to 123 lower-range UTF-8 characters (maximum character 
value 0x7F) terminated by a newline.

### Status Code Format
`^999 A{2-123}\n$`

^: Beginning of input.

$: End of input.

999: Stringified numeric error code

A{2-123}: Two to one hundred twenty three single-byte UTF-8 characters

\n: Newline character (UTF-8 character 0x0A).

### Table 2: Status Codes

| Code |     Status     |                       Description                           |
| ---- | -------------- | ----------------------------------------------------------- |
|  220 |     Ready      | Indicates that MCTP service is ready. Sent by target system to the control system upon successful connect. |
|  221 | Shutting Down  | MCTP is closing down the protocol.                          |
|  250 |  Command Okay  | Requested MCTP command processed successfully.              |
|  354 |  Begin Data    | Indicates that the MCTP target system is ready to receive initial password data transmission. |
|  355 | Continue Data  | Indicates that the MCTP target system is ready to receive additional password data. |
|  421 |  Unavailable   | Indicates that the MCTP service is unavailable or disabled. |
|  451 | Aborted Error  | Error in local processing of MCTP command caused command to abort. See target system error log. |
|  500 | Command Unknwn | Command is unknown to MCTP or is invalid.                   |
|  501 |  Syntax Error  | An error was encountered with the command syntax.           |
|  503 | Wrong Sequence | MCTP commands were sent in the wrong sequence.              |
|  521 | Control Reject | Indicates that the MCTP target system does not accept MCTP requests from controlling host. |
|  530 | Access Denied  | MCTP target host has denied the target system's access to the previous command, or authentication failed. |
|  551 |  User Unknown  | Indicates that the user is unknown to the target system.    |
|  554 | Transact Fail  | The transaction failed due to an unknown error.             |
