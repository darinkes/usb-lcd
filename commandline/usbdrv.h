/**
 * \file usbdrv.h
 * \brief USB-driver-parts for implementing a client.
 * \author Ronald Schaten
 *
 * License: See documentation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <usb.h>                /* this is libusb, see http://libusb.sourceforge.net/ */

#define USBDEV_SHARED_VENDOR		0x16C0	/**< VOTI */
#define USBDEV_SHARED_PRODUCT		0x05DC	/**< Obdev's free shared PID. Use obdev's generic shared VID/PID pair and follow the rules outlined in firmware/usbdrv/USBID-License.txt. */

/* These are error codes for the communication via USB. */
#define USB_ERROR_NOTFOUND		1	/**< Error code if the device isn't found. */
#define USB_ERROR_ACCESS		2	/**< Error code if the device isn't accessible. */
#define USB_ERROR_IO			3	/**< Error code if errors in the communication with the device occur. */

/**
 * Connect to the USB-device. Loops through all connected USB-Devices and
 * searches our counterpart.
 * \param device Handle to address the device.
 * \param vendor USBDEV_SHARED_VENDOR as defined.
 * \param vendorName
 * \param product USBDEV_SHARED_PRODUCT as defined.
 * \param productName
 * \return Error code.
 */
int usbOpenDevice(usb_dev_handle ** device, int vendor, char *vendorName, int product, char *productName);
int usbGetStringAscii(usb_dev_handle * dev, int index, int langid, char *buf, int buflen);
int usbSend(usb_dev_handle * dev, int message);
int transform(char letter);
