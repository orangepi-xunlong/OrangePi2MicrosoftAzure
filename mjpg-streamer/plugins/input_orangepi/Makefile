###############################################################
#
# Purpose: Makefile for "M-JPEG Streamer"
# Author.: Tom Stoeveken (TST)
# Version: 0.3
# License: GPL
#
###############################################################

CC = gcc

OTHER_HEADERS = ../../mjpg_streamer.h ../../utils.h ../output.h ../input.h

CFLAGS += -O1 -DLINUX -D_GNU_SOURCE -Wall -shared -fPIC -I./include

LFLAGS += -ljpeg -lpthread

SRC_FILE = OrangePi_video.c OrangePi_V4L2.c jpeg_utils.c dynctrl.c

all: input_orangepi.so 

clean:
	rm -f *.a *.o core *~ *.so *.lo

input_orangepi.so: $(SRC_FILE)
	$(CC) $^ $(CFLAGS) $(LFLAGS) -o $@

#input_orangepi.so: OrangePi_video.c OrangePi_V4L2.lo jpeg_utils.lo dynctrl.lo
#	$(CC) $(CFLAGS) $(LFLAGS) -o $@ $^ 

OrangePi_V4L2.lo: OrangePi_V4L2.c 
	$(CC) -c $(CFLAGS) -o $@ $^

jpeg_utils.lo: jpeg_utils.c 
	$(CC) -c $(CFLAGS) -o $@ $^

dynctrl.lo: dynctrl.c 
	$(CC) -c $(CFLAGS) -o $@ $^

.PHONY: clean all
