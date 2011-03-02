#include <stdint.h>

/* return codes for USB-communication */
#define msgOK 0     /**< Return code for OK. */
#define msgErr 1    /**< Return code for Error. */

/* These are the vendor specific SETUP commands implemented by our USB device */
#define CMD_ECHO		0 /**< Command to echo the sent data */
#define CMD_GET			1 /**< Command to fetch values */
#define CMD_SET			2 /**< Command to send values */
