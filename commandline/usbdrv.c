#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <usb.h>                /* this is libusb, see
                                   http://libusb.sourceforge.net/ */
#include "usbdrv.h"
#include "main.h"

int
transform(char letter) {

	int buchstabe;

	switch(letter){
		case 'A': buchstabe = 0;
			break;
		case 'B': buchstabe = 1;
			break;
		case 'C': buchstabe = 2;
			break;
		case 'D': buchstabe = 3;
			break;
		case 'E': buchstabe = 4;
			break;
		case 'F': buchstabe = 5;
			break;
		case 'G': buchstabe = 6;
			break;
		case 'H': buchstabe = 7;
			break;
		case 'I': buchstabe = 8;
			break;
		case 'J': buchstabe = 9;
			break;
		case 'K': buchstabe = 10;
			break;
		case 'L': buchstabe = 11;
			break;
		case 'M': buchstabe = 12;
			break;
		case 'N': buchstabe = 13;
			break;
		case 'O': buchstabe = 14;
			break;
		case 'P': buchstabe = 15;
			break;
		case 'Q': buchstabe = 16;
			break;
		case 'R': buchstabe = 17;
			break;
		case 'S': buchstabe = 18;
			break;
		case 'T': buchstabe = 19;
			break;
		case 'U': buchstabe = 20;
			break;
		case 'V': buchstabe = 21;
			break;
		case 'W': buchstabe = 22;
			break;
		case 'X': buchstabe = 23;
			break;
		case 'Y': buchstabe = 24;
			break;
		case 'Z': buchstabe = 25;
			break;
		case 'a': buchstabe = 26;
			break;
		case 'b': buchstabe = 27;
			break;
		case 'c': buchstabe = 28;
			break;
		case 'd': buchstabe = 29;
			break;
		case 'e': buchstabe = 30;
			break;
		case 'f': buchstabe = 31;
			break;
		case 'g': buchstabe = 32;
			break;
		case 'h': buchstabe = 33;
			break;
		case 'i': buchstabe = 34;
			break;
		case 'j': buchstabe = 35;
			break;
		case 'k': buchstabe = 36;
			break;
		case 'l': buchstabe = 37;
			break;
		case 'm': buchstabe = 38;
			break;
		case 'n': buchstabe = 39;
			break;
		case 'o': buchstabe = 40;
			break;
		case 'p': buchstabe = 41;
			break;
		case 'q': buchstabe = 42;
			break;
		case 'r': buchstabe = 43;
			break;
		case 's': buchstabe = 44;
			break;
		case 't': buchstabe = 45;
			break;
		case 'u': buchstabe = 46;
			break;
		case 'v': buchstabe = 47;
			break;
		case 'w': buchstabe = 48;
			break;
		case 'x': buchstabe = 49;
			break;
		case 'y': buchstabe = 50;
			break;
		case 'z': buchstabe = 51;
			break;
		case ' ': buchstabe = 52;
				break;
		case '0': buchstabe = 53;
				break;
		case '1': buchstabe = 54;
				break;
		case '2': buchstabe = 55;
				break;
		case '3': buchstabe = 56;
				break;
		case '4': buchstabe = 57;
				break;
		case '5': buchstabe = 58;
				break;
		case '6': buchstabe = 59;
				break;
		case '7': buchstabe = 60;
				break;
		case '8': buchstabe = 61;
				break;
		case '9': buchstabe = 62;
				break;
		case ':': buchstabe = 63;
				break;
		case '!': buchstabe = 64;
				break;
		case '?': buchstabe = 65;
				break;
		case '-': buchstabe = 66;
				break;
		case '.': buchstabe = 67;
				break;
		case '/': buchstabe = 68;
				break;
		case '\\': buchstabe = 69;
				break;
		default: buchstabe = 52;
	}
	return buchstabe;
}

int
usbGetStringAscii(usb_dev_handle * dev, int index, int langid,
			char *buf, int buflen) {

	char buffer[256];
	int rval, i;

	if ((rval = usb_control_msg(dev, USB_ENDPOINT_IN, USB_REQ_GET_DESCRIPTOR,
	    (USB_DT_STRING << 8) + index, langid, buffer,
	    sizeof(buffer), 1000)) < 0) {
		return rval;
	}

	if (buffer[1] != USB_DT_STRING) {
		return 0;
	}

	if ((unsigned char)buffer[0] < rval) {
		rval = (unsigned char)buffer[0];
	}

	rval /= 2;

	/* lossy conversion to ISO Latin1 */
	for (i = 1; i < rval; i++) {
		if (i > buflen) {
		/* destination buffer overflow */
			break;
		}
		buf[i - 1] = buffer[2 * i];
		if (buffer[2 * i + 1] != 0) {
		/* outside of ISO Latin1 range */
			buf[i - 1] = '?';
		}
	}

	buf[i - 1] = 0;
	return i - 1;
}

int
usbOpenDevice(usb_dev_handle ** device, int vendor, char *vendorName,
		int product, char *productName) {

	struct usb_bus *bus;
	struct usb_device *dev;
	usb_dev_handle *handle = NULL;
	int errorCode = USB_ERROR_NOTFOUND;
	static int didUsbInit = 0;

	if (!didUsbInit) {
		didUsbInit = 1;
		usb_init();
	}

	usb_find_busses();
	usb_find_devices();

	for (bus = usb_get_busses(); bus; bus = bus->next) {
		for (dev = bus->devices; dev; dev = dev->next) {
			if (dev->descriptor.idVendor == vendor
			    && dev->descriptor.idProduct == product) {

				char string[256];
				int len;

				handle = usb_open(dev);	/* we need to open the device in
							   order to query strings */

				if (!handle) {
					errorCode = USB_ERROR_ACCESS;
					fprintf(stderr,
						"Warning: cannot open USB device: %s\n",
						usb_strerror());
					continue;
				}

				if (vendorName == NULL && productName == NULL) {
					/* name does not matter */
					break;
				}

				/* now check whether the names match: */
				len = usbGetStringAscii(handle,
							dev->descriptor.iManufacturer,
							0x0409, string, sizeof(string)
							);

				if (len < 0) {
					errorCode = USB_ERROR_IO;
					fprintf(stderr,
						"Warning: cannot query manufacturer for device: %s\n",
						usb_strerror());
				} else {
					errorCode = USB_ERROR_NOTFOUND;
					if (strcmp(string, vendorName) == 0) {
						len = usbGetStringAscii(handle,
									dev->descriptor.iProduct,
									0x0409, string,
									sizeof(string)
									);

						if (len < 0) {
							errorCode = USB_ERROR_IO;
							fprintf(stderr,
								"Warning: cannot query product for device: %s\n",
								usb_strerror()
								);
						} else {
							errorCode = USB_ERROR_NOTFOUND;
							if (strcmp(string, productName) == 0) {
								break;
							}
						}
					}
				}
				usb_close(handle);
				handle = NULL;
			}
		}

		if (handle) {
			break;
		}

	}

	if (handle != NULL) {
		errorCode = 0;
		*device = handle;
	}

	return errorCode;
}

int usbSend(usb_dev_handle * handle, int message) {
	unsigned char buffer[8];
	int nBytes;

	//fprintf(stderr, "Sending: %i\n", message);
	
	nBytes = usb_control_msg(handle,
				 USB_TYPE_VENDOR | USB_RECIP_DEVICE |
				 USB_ENDPOINT_IN, CMD_SET, message, 0,
				 (char *)buffer, sizeof(buffer), 5000);
	if (nBytes < 0) {
		fprintf(stderr, "USB error: %s\n", usb_strerror());
		return 1;
	}	

	return 0;
}
