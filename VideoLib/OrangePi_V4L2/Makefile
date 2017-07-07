CC=gcc
FLAGS= -g -I./include -ljpeg
TARGET=OrangePi_Capture

OBJ=OrangePi_V4L2API.o OrangePi_video.o OrangePi_Covlib.o

${TARGET}:${OBJ}
	@${CC} ${OBJ} ${FLAGS} -o ${TARGET}
	@rm *.o

LIB_FILE= OrangePi_video.c OrangePi_V4L2API.c OrangePi_Covlib.c 
lib: ${OBJ}
	@${CC} ${LIB_FILE} ${FLAGS} -fPIC -shared -o libOrangePi_video.so
	@rm *.o

OrangePi_video.o: OrangePi_video.c
	@${CC} OrangePi_video.c ${FLAGS} -c
OrangePi_V4L2API.o: OrangePi_V4L2API.c
	@${CC} OrangePi_V4L2API.c ${FLAGS} -c
OrangePi_Covlib.o: OrangePi_Covlib.c
	@${CC} OrangePi_Covlib.c ${FLAGS} -c

install_jpeg:
	@apt-get install libjpeg8-dev 

.PHONY: clean
clean:
	@rm ${TARGET} *.bmp
