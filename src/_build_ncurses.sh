rm *.o

gcc  \
 -c \
 -std=gnu99 \
 -I. \
 -I ..\
 -I$(LIBRARIES)/libmpdclient-2.4/include \
 *.c

gcc \
 -o mpc \
 *.o \
 -L$(LIBRARIES)/libmpdclient-2.4/src/.libs -lmpdclient \
 -lcurses -lpthread
