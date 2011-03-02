#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <usb.h>                /* this is libusb, see
                                   http://libusb.sourceforge.net/ */
#include "usbdrv.h"

#include "main.h"

static void usage (void) {
	fprintf(stderr,
"usage usblcd:\n"
"helo - send helo byte\n"
"clear - clear the screen\n"
"line[0-7] string - set string on line\n"
"symbol email=[in]active | icq=[in]active - set symbol status\n\n"
	);
	exit(1);
}

int main(int argc, char **argv) {
	usb_dev_handle *handle = NULL;
	int j;
	char string[64];

	usb_init();

	if (argc < 2) {
		usage();
	}

	if (usbOpenDevice(&handle, USBDEV_SHARED_VENDOR, "www.sri-dev.de",
	    USBDEV_SHARED_PRODUCT, "USB-Dingens") != 0) {
		fprintf(stderr,
			"Could not find USB device \"USB-Dingens\" with vid=0x%x pid=0x%x\n",
			USBDEV_SHARED_VENDOR, USBDEV_SHARED_PRODUCT);
		exit(1);
	}

	usbSend(handle, DONE);

	if (strcmp(argv[1], "clear") == 0) {
		usbSend(handle, CLEAR);
		exit(0);
	} else if (strcmp(argv[1], "helo") == 0) {
		usbSend(handle, READY);
		exit(0);
	} else if (argc == 2) {
		fprintf(stderr, "Not enough arguments");
		usage();
	}

	strlcpy(string, argv[2], 64);

	if (strcmp(argv[1], "symbol") == 0) {
		usbSend(handle, SYMBOL);
		if (strcmp(string, "email=active") == 0) {
			usbSend(handle, EMAIL_A);
		} else if (strcmp(string, "email=inactive") == 0) {
			usbSend(handle, EMAIL_I);
		} else if (strcmp(string, "icq=active") == 0) {
			usbSend(handle, ICQ_A);
		} else if (strcmp(string, "icq=inactive") == 0) {
			usbSend(handle, ICQ_I);
		} else {
			fprintf(stderr, "unknown: %s\n", string);
			usbSend(handle, DONE);
			exit(1);
		}
		usbSend(handle, DONE);
		exit(0);
	}

	if (strcmp(argv[1], "line0") == 0) {
		usbSend(handle, SETLINE);
		usbSend(handle, LINE0);
	} else if (strcmp(argv[1], "line1") == 0) {
		usbSend(handle, SETLINE);
		usbSend(handle, LINE1);
	} else if (strcmp(argv[1], "line2") == 0) {
		usbSend(handle, SETLINE);
		usbSend(handle, LINE2);
	} else if (strcmp(argv[1], "line3") == 0) {
		usbSend(handle, SETLINE);
		usbSend(handle, LINE3);
	} else if (strcmp(argv[1], "line4") == 0) {
		usbSend(handle, SETLINE);
		usbSend(handle, LINE4);
	} else if (strcmp(argv[1], "line5") == 0) {
		usbSend(handle, SETLINE);
		usbSend(handle, LINE5);
	} else if (strcmp(argv[1], "line6") == 0) {
		usbSend(handle, SETLINE);
		usbSend(handle, LINE6);
	} else if (strcmp(argv[1], "line7") == 0) {
		usbSend(handle, SETLINE);
		usbSend(handle, LINE7);
	} else {
		fprintf(stderr, "unknown: %s\n", argv[1]);
		usage();
	}

	usbSend(handle, DONE);

	usbSend(handle, STRING);

	for(j=0; j < strlen(string); j++){
		usbSend(handle, transform(string[j]));
	}

	usbSend(handle, DONE);

	return 0;
}
