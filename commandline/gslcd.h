#include <stdint.h>

/* Signals for Communication */

#define SYMBOL		0xF9	/* Symbol-Transmission will start */
#define SETLINE		0xFA	/* Set cursor to lineX on LCD*/
#define READY		0xFB	/* Host is ready for Communication */
#define CLEAR		0xFD	/* Clear LCD */
#define STRING		0xFE	/* String-Transmission will start */
#define DONE		0xFF	/* Transmission finished*/

#define LINE0		0x00
#define LINE1		0x08
#define LINE2		0x10
#define LINE3		0x18
#define LINE4		0x20
#define LINE5		0x28
#define LINE6		0x30
#define LINE7		0x38

#define EMAIL_I		0x20
#define EMAIL_A		0x21
#define ICQ_I		0x22
#define ICQ_A		0x23

/* return codes for USB-communication */
#define msgOK		0	/**< Return code for OK. */
#define msgErr		1	/**< Return code for Error. */

/* These are the vendor specific SETUP commands implemented by our USB device */
#define CMD_ECHO	0x00 /* Command to echo the sent data */
#define CMD_GET		0x01 /* Command to fetch values */
#define CMD_SET		0x02 /* Command to send values */
