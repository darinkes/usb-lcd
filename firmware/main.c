#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "arial8.h"
#include "common.h"
#include "finger.h"
#include "mylcd.h"
#include "led.h"
#include "signals.h"
#include "symbols.h"
#include "usbdrv.h"
#include "oddebug.h"

static uint8_t received;

unsigned char letters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz 0123456789:!?-./\\";

int setLine(void) {
    while (1) {                 /* main event loop */
        usbPoll();
        if (TIFR & (1 << TOV0)) {
            TIFR |= 1 << TOV0;  /* clear pending flag */
		}
		if(received != DONE && received != INVALID) {
			if (received == LINE0) {
				lcd_set_cursor(25, received);
			} else {
				lcd_set_cursor(1, received);
			}
		} else if(received == DONE) {
			received = INVALID;
			return 1;
		}
		received = INVALID;
	}
	return 0;
}

int setString(void) {
    while (1) {                 /* main event loop */
        usbPoll();
        if (TIFR & (1 << TOV0)) {
            TIFR |= 1 << TOV0;  /* clear pending flag */
		}
		if(received != DONE && received != INVALID) {
			lcd_putc(arial8, letters[received]);
		} else if(received == DONE) {
			received = INVALID;
			return 1;
		}
		received = INVALID;
	}
	return 0;
}

int setSymbol(void) {
    while (1) {                 /* main event loop */
        usbPoll();
        if (TIFR & (1 << TOV0)) {
            TIFR |= 1 << TOV0;  /* clear pending flag */
		}
		if(received != DONE && received != INVALID) {
			if(received == 32 || received == 33) {
				lcd_set_cursor(1,LINE0);
				lcd_putc(symbols2, received);
			} else if (received == 34 || received == 35) {
				lcd_set_cursor(12, LINE0);
				lcd_putc(symbols2, received);
			}
		} else if(received == DONE) {
			received = INVALID;
			return 1;
		}
		received = INVALID;
	}
	lcd_draw_line(11,0,11,128,BLACK);
	return 0;
}

int waitForHost(void) {
    while (1) {                 /* main event loop */
        usbPoll();
        if (TIFR & (1 << TOV0)) {
            TIFR |= 1 << TOV0;  /* clear pending flag */
		}
		if (received == READY) {
			return 1;
		}
	}
}

uchar usbFunctionSetup(uchar data[8]) {
    static uchar replyBuffer[8];
    uchar replyLength;

    replyBuffer[0] = msgOK;
    switch (data[1]) {
    case CMD_ECHO:
        replyBuffer[0] = data[2];
        replyBuffer[1] = data[3];
        replyLength = 2;
        break;
    case CMD_GET:
        replyLength = 1;
        break;
    case CMD_SET:
		received = data[2];
        replyLength = 0;
        break;
    default:
        replyBuffer[0] = msgErr;
        replyLength = 1;
        break;
    }
    usbMsgPtr = replyBuffer;
    return replyLength;
}

int main (void) {
    uchar i, j;
    DDRD = ~0;                  /* output SE0 for USB reset */
    PORTD = 0x00;               /* no pullups on USB pins */

	led_init();
	lcd_init();
	lcd_clear();
	
	led_green();

	lcd_draw_fullscreen_bmp(finger);

    for (i = 0; i < 50; i++) {
        _delay_ms(100);
    }
	
	lcd_clear();

	lcd_set_cursor(1,LINE0);
	lcd_puts(arial8,"Starting USB... ");

    j = 0;
    while (--j) {               /* USB Reset by device only required on
                                   Watchdog Reset */
        i = 0;
        while (--i);            /* delay >10ms for USB reset */
    }
    DDRD = ~USBMASK;            /* all outputs except USB data */
    TCCR0 = 5;                  /* set prescaler to 1/1024 */
    usbInit();
    sei();

	received = INVALID;

    for (i = 0; i < 10; i++) {  /* wait one second to prevent strange
                                   effects when the USB-bus isn't
                                   initialized (e. g. when the host system
                                   is on standby. */
        _delay_ms(100);
    }

	lcd_puts(arial8, "OK.");

	lcd_set_cursor(1, LINE1);

	lcd_puts(arial8, "Waiting for host...");
	if (waitForHost() == 1) {
		lcd_puts(arial8, "OK.");
	}

    while (1) {
        usbPoll();
        if (TIFR & (1 << TOV0)) {
            TIFR |= 1 << TOV0;  /* clear pending flag */
		}
		switch (received) {
		case SETLINE:
			received = INVALID;
			setLine();
			break;
		case STRING:
			received = INVALID;
			setString();
			break;
		case SYMBOL:
			received = INVALID;
			setSymbol();
			break;			
		case CLEAR:
			received = INVALID;
			lcd_clear();
			break;
		default:
			received = INVALID;
			break;
		}
		received = INVALID;	
    }
    return 0;
}
