# Protocol Description
This file describes the mpasswd protocol. It is similar to SMTP with a
set of commands and codes to direct the protocol flow.

## Commands
HELO [host name] : Establishes contact with the MPasswd target and identifies
                   the control system to the target via the `host name`
				   parameter.
