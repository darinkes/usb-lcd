#ifndef	MYLCD_H
#define MYLCD_H
#include <avr/pgmspace.h>

	//###############################################################################################
	//# 																							#
	//# User Configurations																			#
	//#																								#
	//###############################################################################################
	
	// Ports
	#define LCD_CMD_PORT		PORTA		// Command Output Register
	#define LCD_CMD_DDR			DDRA		// Data Direction Register for Command Port

	#define LCD_DATA_PIN		PINC		// Data Input Register
	#define LCD_DATA_PORT		PORTC		// Data Output Register
	#define LCD_DATA_DDR		DDRC		// Data Direction Register for Data Port

	// Command Port Bits
	#define CD					PA0		// Data/Control (RS) PinNumber
	#define RW					PA1		// R/W PinNumber
	#define EN					PA4		// EN  PinNumber
	#define CS1					PA3		// CS1 PinNumber
	#define CS2					PA2		// CS2 PinNumber	
	//#define LCD_NO_RES					// uncomment this line if you have an pullup on reset
	
	#ifndef LCD_NO_RES
		#define RES				PA5		// Reset Pin
	#endif



	//###############################################################################################
	//# 																							#
	//# Extra configuration																			#
	//#																								#
	//###############################################################################################

	#define FASTTEXT 			// comment this if you get bad text output
								// draws Text much faster (writes whole pages without using set_pixel)	
								// only recommended to use if font has an height of 8 (or multipel)
								// using y-positions,0,8,16,24... lcd_set_line(uint8_t line)	

	//#define NO_BMPS			// if you do not need bitmaps
	//#define NO_GRAPHICS		// if you do not need lines/rects/circels
	//#define NO_BRESENHAM		// if you only use horizontal or vertical lines (or rects) (~264 Bytes)
	//#define NO_TEXT			// if you do not need text output

	//###############################################################################################
	//# 																							#
	//# Defines																						#
	//#																								#
	//###############################################################################################

	// Chips
	#define CHIP1				0x01
	#define CHIP2				0x02

	// Commands
	#define LCD_ON				0x3F
	#define LCD_OFF				0x3E
	#define LCD_SET_ADD			0x40
	#define LCD_SET_PAGE		0xB8
	#define LCD_DISP_START		0xC0
	#define LCD_BUSY			0x80

	// Pixel Colors
	#define BLACK				0x01
	#define WHITE				0x00

	// Lines

	#define LINE0				0x00
	#define LINE1				0x08
	#define LINE2				0x10
	#define LINE3				0x18
	#define LINE4				0x20
	#define LINE5				0x28
	#define LINE6				0x30
	#define LINE7				0x38


	//###############################################################################################
	//# 																							#
	//# Prototyes																					#
	//#																								#
	//###############################################################################################

	#define max(a, b) ((a)>(b)?(a):(b)) 
	#define min(a, b) ((a)>(b)?(b):(a))


	// low level lcd	
	void lcd_chip_select(uint8_t);
	void lcd_write_cmd(uint8_t,uint8_t);
	void lcd_write_data(uint8_t,uint8_t);
	void lcd_write(uint8_t,uint8_t);
	uint8_t lcd_read_data(uint8_t);
	void wait_while_chip_is_busy();
	
	// lcd
	void lcd_init();
	void lcd_clear();
	void lcd_set_pixel(uint8_t,uint8_t,uint8_t);
	void lcd_invert();
	#ifndef NO_BMP
	void lcd_draw_fullscreen_bmp(uint8_t*);
	#endif



	// low level font
	#ifndef NO_TEXT
	uint8_t font_number_of_chars(uint8_t*);
	uint8_t font_first_char(uint8_t*);
	uint8_t font_char_width(uint8_t*,uint8_t);
	uint8_t font_char_height();
	uint16_t font_start_offset(uint8_t*,uint8_t);
	uint8_t font_char_present(uint8_t*,uint8_t);	

	// font
	void lcd_set_cursor(uint8_t, uint8_t);	
	void lcd_putc(uint8_t*,uint8_t);
	void lcd_puts(uint8_t*, char* );
	void lcd_puts_p(uint8_t*, char* PROGMEM );
	#endif



	// graphics
	#ifndef NO_GRAPHICS
	void lcd_draw_line(uint8_t,uint8_t,uint8_t,uint8_t,uint8_t);
	void lcd_draw_rect(uint8_t,uint8_t,uint8_t,uint8_t,uint8_t);
	void lcd_fill_rect(uint8_t,uint8_t,uint8_t,uint8_t,uint8_t);
	#endif

#endif
