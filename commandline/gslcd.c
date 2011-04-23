#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/time.h>
#include <tzfile.h>

#include <math.h>

#include <ifaddrs.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifdef USB
#include <usb.h>                /* this is libusb, see
                                   http://libusb.sourceforge.net/ */
#include "usbdrv.h"
#endif

#include "gslcd.h"

#define LOG1024		10
#define pagetok(size) ((size) << pageshift)

static void usage (void) {
	fprintf(stderr,
"usage: gslcd -i interface\n"
	);
	exit(1);
}

int
main(int argc, char **argv) {
	const char	*namep;
	struct ifaddrs	*ifa, *ifap, *ifas, *ifasp;
	struct if_data	*ifd, *ifds;
	struct sockaddr	*sa;
	struct timeval	newtime, oldtime, boottime;
	struct loadavg	sysload;
	struct vmtotal	vmtotal;
	long		timediff = 0;
	double		speedin, speedout,
			output, input;
	size_t		len;
	int		mib[6], cpuspeed, pagesize,
			ch, memtotal, memused, memfree,
			iflag = 0,
			days, hrs, mins,
			matchcount = 0;
	static int	pageshift;
	char		hostname[256],
			ifname[IFNAMSIZ],
			*arg = NULL,
			strspin[20], strspout[20],
			load[40],
			strsumout[20], strsumin[20],
			strmemtot[20], strmemused[20],
			strrec[40], strtra[40],
			memory[40], strup[40];
	time_t		uptime;
#ifdef DEBUG
	char		ipstr[64];
#endif
#ifdef USB
	usb_dev_handle *handle = NULL;
	char		ifinfo[20];
	int		j;
#endif

	pagesize = sysconf(_SC_PAGESIZE);
	pageshift = 0;
	while (pagesize > 1) {
		pageshift++;
		pagesize >>= 1;
	}

	pageshift -= LOG1024;

#ifdef USB
	usb_init();

	if (usbOpenDevice(&handle, USBDEV_SHARED_VENDOR, "www.sri-dev.de",
	    USBDEV_SHARED_PRODUCT, "USB-Dingens") != 0) {
		fprintf(stderr,
			"Could not find USB device \"USB-Dingens\" with vid=0x%x pid=0x%x\n",
			USBDEV_SHARED_VENDOR, USBDEV_SHARED_PRODUCT);
		exit(1);
	}
	usbSend(handle, DONE);
	usbSend(handle, READY);
	usbSend(handle, CLEAR);
#endif /* USB */

	while ((ch = getopt(argc, argv, "hi:")) != -1) {
		switch (ch) {
		case 'h':
			usage();
			break;
		case 'i':
			iflag = 1;
			arg = optarg;
			break;
		default:
			usage();
			break;
		}
	}

	if (iflag) {
		if (strlcpy(ifname, arg, sizeof(ifname)) >= IFNAMSIZ) {
			fprintf(stderr, "Error: interface name '%s' too long\n", *argv);
			usage();
		}
	} else {
		fprintf(stderr, "Error: please give an interface\n");
		usage();
	}

	while(1) {

		if (gettimeofday(&oldtime, NULL) == -1) {
			fprintf(stderr,"unable to determine time\n");
			exit(1);
		}

		if (getifaddrs(&ifa) == -1) {
			fprintf(stderr,"no interface found\n");
			exit(1);
		}

		sleep(1);

		if (gettimeofday(&newtime, NULL) == -1) {
			fprintf(stderr,"unable to determine time\n");
			exit(1);
		}

		if (getifaddrs(&ifas) == -1) {
			fprintf(stderr,"no interface found\n");
			exit(1);
		}

		timediff = newtime.tv_sec - oldtime.tv_sec;

		mib[0] = CTL_HW;
		mib[1] = HW_CPUSPEED;
		len = sizeof(cpuspeed);
		if (sysctl(mib, 2, &cpuspeed, &len, NULL, 0) < 0) {
			fprintf(stderr, "unable to get cpuspeed");
			exit(1);
		}

		mib[0] = CTL_KERN;
		mib[1] = KERN_HOSTNAME;
		len = sizeof(hostname);
		if (sysctl(mib, 2, &hostname, &len, NULL, 0) < 0) {
			fprintf(stderr, "unable to get hostname");
			exit(1);
		}

		mib[0] = CTL_VM;
		mib[1] = VM_LOADAVG;
		len = sizeof(sysload);
		if (sysctl(mib, 2, &sysload, &len, NULL, 0) < 0) {
			fprintf(stderr, "unable to get loadavg");
			exit(1);
		}

		snprintf(load, sizeof(load), "Load: %.2f, %.2f, %.2f     ",
			    ((double)sysload.ldavg[0]) / sysload.fscale,
			    ((double)sysload.ldavg[1]) / sysload.fscale,
			    ((double)sysload.ldavg[2])/ sysload.fscale
			);

		// Memory Usage in KByte
		mib[0] = CTL_VM;
		mib[1] = VM_METER;
		len = sizeof(vmtotal);
		if (sysctl(mib, 2, &vmtotal, &len, NULL, 0) < 0) {
			fprintf(stderr, "unable to get vmtotal");
			exit(1);
		}

		memtotal = pagetok(vmtotal.t_rm);
		memused = pagetok(vmtotal.t_arm);
		memfree = pagetok(vmtotal.t_free);

		memtotal = memtotal + memfree;

		if (memtotal > 1048576) {
			memtotal = memtotal/1048576;
			snprintf(strmemtot, sizeof(strmemtot), "%iGB", memtotal);
		} else if ( memtotal > 1024) {
			memtotal = memtotal/1024;
			snprintf(strmemtot, sizeof(strmemtot), "%iMB", memtotal);
		} else {
			snprintf(strmemtot, sizeof(strmemtot), "%iKB", memtotal);
		}

		if (memused > 1048576) {
			memused = memused/1048576;
			snprintf(strmemused, sizeof(strmemused), "%iGB", memused);
		} else if (memused > 1024) {
			memused = memused/1024;
			snprintf(strmemused, sizeof(strmemused), "%iMB", memused);
		} else {
			snprintf(strmemused, sizeof(strmemused), "%iKB", memused);
		}

		snprintf(memory, sizeof(memory), "Mem: %s/%s Used  ",
			strmemused, strmemtot
			);

		// uptime
		mib[0] = CTL_KERN;
		mib[1] = KERN_BOOTTIME;
		len = sizeof(boottime);
		if (sysctl(mib, 2, &boottime, &len, NULL, 0) < 0) {
			fprintf(stderr, "unable to get boottime");
			exit(1);
		}
		uptime = newtime.tv_sec - boottime.tv_sec;
		snprintf(strup, sizeof(strup), "uptime:");
		if (uptime > 59) {
			uptime += 30;
			days = uptime / SECSPERDAY;
			uptime %= SECSPERDAY;
			hrs = uptime / SECSPERHOUR;
			uptime %= SECSPERHOUR;
			mins = uptime / SECSPERMIN;
			if (days > 0)
				snprintf(strup, sizeof(strup), "%s %d day%s", strup, days, days > 1 ? "s" : "");
			if (hrs > 0 && mins > 0)
				snprintf(strup, sizeof(strup), "%s %2d:%02d,", strup, hrs, mins);
			else {
				if (hrs > 0)
					snprintf(strup, sizeof(strup), "%s %d hr%s,", strup, hrs, hrs > 1 ? "s" : "");
				if (mins > 0 || (days == 0 && hrs == 0))
					snprintf(strup, sizeof(strup), "%s %d min%s", strup, mins, mins != 1 ? "s" : "");
			}
			snprintf(strup, sizeof(strup), "%s\n", strup);
		} else
			snprintf(strup, sizeof(strup), "%s %d secs\n", strup, uptime);
#ifdef DEBUG
		printf("%s", strup);
#endif

#ifdef USB
		// send hostname
		usbSend(handle, SETLINE);
		usbSend(handle, LINE0);
		usbSend(handle, DONE);
		usbSend(handle, STRING);
		for(j=0; j < strlen(hostname); j++) {
			usbSend(handle, transform(hostname[j]));
		}
		usbSend(handle, DONE);

		// send loadavg
		usbSend(handle, SETLINE);
		usbSend(handle, LINE1);
		usbSend(handle, DONE);
		usbSend(handle, STRING);
		for(j=0; j < strlen(load); j++) {
			usbSend(handle, transform(load[j]));
		}
		usbSend(handle, DONE);

		// send memory usage
		usbSend(handle, SETLINE);
		usbSend(handle, LINE2);
		usbSend(handle, DONE);
		usbSend(handle, STRING);
		for(j=0; j < strlen(memory); j++) {
			usbSend(handle, transform(memory[j]));
		}
		usbSend(handle, DONE);

		// send watched interface
		snprintf(ifinfo, sizeof(ifinfo), "Interface: %s", ifname);
		usbSend(handle, SETLINE);
		usbSend(handle, LINE3);
		usbSend(handle, DONE);
		usbSend(handle, STRING);
		for(j=0; j < strlen(ifinfo); j++) {
			usbSend(handle, transform(ifinfo[j]));
		}
		usbSend(handle, DONE);

		// send uptime
		usbSend(handle, SETLINE);
		usbSend(handle, LINE4);
		usbSend(handle, DONE);
		usbSend(handle, STRING);
		for(j=0; j < strlen(strup); j++) {
			usbSend(handle, transform(strup[j]));
		}
		usbSend(handle, DONE);
#endif /* USB */

#ifdef DEBUG
		fprintf(stderr, "Hostname: %s\n", hostname);
		fprintf(stderr, "CPUSpeed: %i MHz\n", cpuspeed);
		fprintf(stderr, "%s\n", load);
		fprintf(stderr, "%s\n", memory);
		fprintf(stderr,"Interface: %s\n", ifname);
#endif /* DEBUG */


		namep = NULL;
		ifasp = ifas;
		for (ifap = ifa; ifap != NULL; ifap = ifap->ifa_next) {
			sa = ifap->ifa_addr;

			if (sa == NULL)
				continue;
			if (SA_LEN(sa) == 0)
				continue;

			if ( strcmp(ifasp->ifa_name, ifap->ifa_name) != 0 )
				fprintf(stderr, "Names should be equal: %s -> %s",
				    ifasp->ifa_name, ifap->ifa_name);

			if ( iflag && strcmp(ifname, ifap->ifa_name) == 0  && timediff > 0) {
				matchcount++;

				if ( ( (ifap->ifa_flags & IFF_UP) != 0 &&
				     (ifap->ifa_flags & IFF_LOOPBACK) == 0 ) ) {

					if (namep != NULL) {

						if ( strcmp(namep, ifap->ifa_name) != 0 ) {

							ifd = ifap->ifa_data;
							ifds = ifasp->ifa_data;
							if ( ifd != NULL && timediff > 0 ) {
								output = ifd->ifi_obytes;
								input = ifd->ifi_ibytes;

								if (output >= 1073741824 ) {
									output = output / 1073741824;
									snprintf(strsumout, sizeof(strsumout),
									    "T: %7.2fGB", output);
								} else if (output >= 1048576) {
									output = output / 1048576;
									snprintf(strsumout, sizeof(strsumout),
									    "T: %7.2fMB", output);
								} else if (output >= 1024) {
									output = output / 1024;
									snprintf(strsumout, sizeof(strsumout),
									    "T: %7.2fKB", output);
								} else {
									snprintf(strsumout, sizeof(strsumout),
									    "T: %7.2fB", output);
								}

								if (input >= 1073741824 ) {
									input = input / 1073741824;
									snprintf(strsumin, sizeof(strsumin),
									    "R: %7.2fGB", input);
								} else if (input >= 1048576) {
									input = input / 1048576;
									snprintf(strsumin, sizeof(strsumin),
									    "R: %7.2fMB", input);
								} else if (input >= 1024) {
									input = input / 1024;
									snprintf(strsumin, sizeof(strsumin),
									    "R: %7.2fKB", input);
								} else {
									snprintf(strsumin, sizeof(strsumin),
									    "R: %7.2fB", input);
								}

								speedin  = (((ifds->ifi_ibytes - ifd->ifi_ibytes) ) / timediff);
								speedout = (((ifds->ifi_obytes - ifd->ifi_obytes) ) / timediff);

								if (speedin >= 1048576) {
									speedin = speedin / 1048576 ;
									snprintf(strspin, sizeof(strspin),
									    "D: %7.2fMB/s", speedin);
								} else if (speedin >= 1024) {
									speedin = speedin / 1024;
									snprintf(strspin, sizeof(strspin),
									    "D: %7.2fKB/s", speedin);
								} else {
									snprintf(strspin, sizeof(strspin),
									    "D: %7.2fB/s", speedin);
								}

								if (speedout >= 1048576) {
									speedout = speedout / 1048576;
									snprintf(strspout, sizeof(strspout),
									    "U: %7.2fMB/s", speedout);
								} else if (speedout >= 1024) {
									speedout = speedout / 1024;
									snprintf(strspout, sizeof(strspout),
									    "U: %7.2fKB/s", speedout);
								} else {
									snprintf(strspout, sizeof(strspout),
									    "U: %7.2fB/s", speedout);
								}
								snprintf(strtra, sizeof(strtra), "%s  %s     ",
								    strsumout, strspout);
								snprintf(strrec, sizeof(strrec), "%s  %s     ",
								    strsumin, strspin);

#ifdef USB
								// send receive
								usbSend(handle, SETLINE);
								usbSend(handle, LINE5);
								usbSend(handle, DONE);
								usbSend(handle, STRING);
								for(j=0; j < strlen(strrec); j++) {
									usbSend(handle, transform(strrec[j]));
								}
								usbSend(handle, DONE);

								// send transmit
								usbSend(handle, SETLINE);
								usbSend(handle, LINE6);
								usbSend(handle, DONE);
								usbSend(handle, STRING);
								for(j=0; j < strlen(strtra); j++) {
									usbSend(handle, transform(strtra[j]));
								}
								usbSend(handle, DONE);
#endif /* USB */
#ifdef DEBUG
								fprintf(stderr,"%s\n", strrec);
								fprintf(stderr,"%s\n", strtra);
#endif /* DEBUG */
							}
						}

#ifdef DEBUG
						if (sa->sa_family == AF_INET) {
							struct sockaddr_in *ipv4 = (struct sockaddr_in *)sa;
							if (inet_ntop(sa->sa_family, &ipv4->sin_addr, ipstr, sizeof(ipstr)) == NULL)
								fprintf(stderr, "unable to get IP for %s\n",
								    ifap->ifa_name);
							fprintf(stderr,"IPv4: %s\n", ipstr);
						} else if (sa->sa_family == AF_INET6) {
							struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)sa;
							if (inet_ntop(sa->sa_family, &ipv6->sin6_addr, ipstr, sizeof(ipstr)) == NULL)
								fprintf(stderr, "unable to get IP for %s\n",
								    ifap->ifa_name);
							fprintf(stderr,"IPv6: %s\n", ipstr);
						}
#endif /* DEBUG */
					}
				}
			}
			namep = ifap->ifa_name;
			ifasp = ifasp->ifa_next;
		}
		printf("\n");
		if (matchcount == 0) {
			fprintf(stderr, "%s: no such interface\n", ifname);
			exit(1);
		}
#ifdef USB
		usbSend(handle, DONE);
#endif /* USB */
		freeifaddrs(ifa);
		freeifaddrs(ifas);
	}

	return 0;
}
