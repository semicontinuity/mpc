rm *.o

arm-linux-gcc \
 -c \
 -std=gnu99 \
 -I. \
 -I ..\
 -I$(LIBRARIES)/arm-libmpdclient/arm-linux/include \
 *.c

arm-linux-gcc \
 -o mpc \
 *.o \
 -L$(LIBRARIES)/arm-libmpdclient/arm-linux/lib -lmpdclient -lpthread
