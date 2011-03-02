CC		= gcc

LIBUSB_CONFIG	= libusb-config

ALL_TARGETS	= usblcd gslcd

ALIX_TARGETS	= gslcd.o
ALIX_CFLAGS	= -Wall -Werror -Wno-uninitialized -Wno-main -Wmissing-prototypes

.if defined(USB)
ALIX_CFLAGS	+= `$(LIBUSB_CONFIG) --cflags` -O2
ALIX_LIBS	+= `$(LIBUSB_CONFIG) --libs`
ALIX_TARGETS	+= usbdrv.o
ALIX_ARGS	=  -DUSB
ALL_TARGETS	+= usblcd
.else
ALIX_ARGS	=  -DDEBUG
.endif

CFLAGS		= `$(LIBUSB_CONFIG) --cflags` -O -Wall
USBLIBS		= `$(LIBUSB_CONFIG) --libs` -static
ALIX_LIBS	+= -static

all: ${ALL_TARGETS}

usblcd: usbdrv.o main.o
	$(CC) $(CFLAGS) -o usblcd main.o usbdrv.o $(USBLIBS)
	strip -o usblcd.strip usblcd

gslcd: ${ALIX_TARGETS}
	$(CC) $(ALIX_CFLAGS) ${ALIX_ARGS} -o gslcd ${ALIX_TARGETS} $(ALIX_LIBS)
	strip -o gslcd.strip gslcd

gslcd.o:
	$(CC) $(ALIX_CFLAGS) ${ALIX_ARGS} -c gslcd.c -o gslcd.o

.c.o:
	$(CC) $(CFLAGS) -c $*.c -o $*.o

clean:
	rm -f usblcd gslcd leak_test *.o *.strip
