Music player client for NP210 IP Phone (or other embedded linux device)
=======================================================================

Building embedded MPC
=====================

Configure
---------
LIBMPDCLIENT_CFLAGS="-I/usr/local/stow/arm-libmpdclient/arm-linux/include" \
LIBMPDCLIENT_LIBS="-L/usr/local/stow/arm-libmpdclient/arm-linux/lib -lmpdclient" \
./configure --prefix=/usr --host=arm-linux


Building embedded MPD
=====================

Toolchain: from ftp.arm.linux.org.uk/pub/linux/arm/toolchain/


Configure
---------
ID3TAG_CFLAGS="-I/usr/local/stow/arm-libid3tag/arm-linux/include" \
ID3TAG_LIBS="-L/usr/local/stow/arm-libid3tag/arm-linux/lib -lid3tag" \
CURL_CFLAGS="-I/usr/local/stow/arm-libcurl/arm-linux/include" \
CURL_LIBS="-L/usr/local/stow/arm-libcurl/arm-linux/lib -lcurl" \
MAD_CFLAGS="-I/usr/local/stow/arm-libmad/arm-linux/include" \
MAD_LIBS="-L/usr/local/stow/arm-libmad/arm-linux/lib -lmad" \
GLIB_CFLAGS="-I/usr/local/stow/arm-glib/arm-linux/include/glib-2.0 -I/usr/local/stow/arm-glib/arm-linux/lib/glib-2.0/include" \
GLIB_LIBS="-L/usr/local/stow/arm-glib/arm-linux/lib -lglib-2.0 -lgthread-2.0" \
CFLAGS="" \
LDFLAGS="-L/usr/local/stow/arm-zlib/arm-linux/lib" \
LIBS="-lpthread -lz" \
./configure \
  --prefix=/mnt/sdcard/mpd \
  --host=arm-linux \
  --disable-un \
  --enable-tcp \
  --enable-oss \
  --disable-flac \
  --disable-oggflac \
  --disable-audiofile \
  --disable-wavpack \
  --disable-ffmpeg \
  --disable-lsr \
  --with-zeroconf=no \
  --enable-curl \
  --disable-alsa \
  --disable-jack \
  --enable-id3 \
  --disable-pulse \
  --disable-fifo


Build
-----
CFLAGS="-I/usr/local/arm/include -I/usr/local/stow/arm-libid3tag/arm-linux/include -I/usr/local/stow/arm-libmad/arm-linux/include" \
LDFLAGS="-L/usr/local/arm/lib -L/usr/local/stow/arm-libid3tag/arm-linux/lib -L/usr/local/stow/arm-libmad/arm-linux/lib" \
CC=arm-linux-gcc ./autogen.sh --host=arm-linux
