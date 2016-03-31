
CC=arm-linux-gnueabihf-g++

all:
	$(CC) main.cpp v4l2_dev.cpp v4l2_frame.cpp -o nv12stream

#	$(CC) main.cpp v4l2_dev.cpp v4l2_frame.cpp \
#		-D_REENTERANT \
#		-L/usr/lib/arm-linux-gnueabihf -lpthread \
#		-o nv12stream

clean:
	rm nv12stream
