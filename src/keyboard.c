#include "keyboard.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if !defined(__arm__)

#include <termios.h>
#include <sys/param.h>
static struct termios old_terminfo;
static struct termios new_terminfo;


void keyboard_init(void) {
    /* get the terminal settings for stdin */
    tcgetattr(STDIN_FILENO, &old_terminfo);

    /* we want to keep the old setting to restore them a the end */
    memcpy (&new_terminfo, &old_terminfo, sizeof(struct termios));

    /* disable canonical mode (buffered i/o) and local echo */
    new_terminfo.c_lflag &=(~ICANON & ~ECHO);

    /* set the new settings immediately */
    tcsetattr(STDIN_FILENO, TCSANOW, &new_terminfo);
}


void keyboard_deinit(void) {
    /* restore the former settings */
    tcsetattr(STDIN_FILENO, TCSANOW, &old_terminfo);
}

int read_key() {
    unsigned char c = (unsigned char) getchar();
    if (c == 27) {
        getchar();
        c = (unsigned char) getchar();
        switch (c) {
        case 65: return VK_UP;
        case 66: return VK_DOWN;
        case 68: return VK_PREVIOUS;
        case 67: return VK_NEXT;
        default: return c;
        }
    }
    else return c;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#else // __arm__

#include <termios.h>
#include <sys/param.h>
static struct termios old_terminfo;
static struct termios new_terminfo;


static FILE* file;
static int fildes;

void keyboard_init(void) {
printf("opening /dev/ttyS1\n");

    file = fopen("/dev/ttyS1","r");
    fildes = fileno(file);

printf("file #%d\n", fildes);
    tcgetattr(fildes, &old_terminfo);

    memcpy(&new_terminfo, &old_terminfo, sizeof(struct termios));
    speed_t speed = cfgetispeed(&old_terminfo);

    new_terminfo.c_lflag &= ~(ICANON|ECHO|ISIG);
    new_terminfo.c_cc[VMIN]  = 0;
    new_terminfo.c_cc[VTIME] = 10;
    cfsetispeed(&new_terminfo, B57600);
    tcsetattr(fildes, 0, &new_terminfo);
}


void keyboard_deinit(void) {
    /* restore the former settings */
    tcsetattr(fildes, 0, &old_terminfo);
    fclose(file);
}


int read_key() {
    while (1) {
        unsigned char c1 = fgetc(file);
        printf("%x\n", c1);
        if (c1 == (unsigned char)0xFF) continue; // keep alive?
        if (c1 != (unsigned char)0x4B) continue; // must be 0x4B
        unsigned char c2 = fgetc(file);
        printf("%x\n", c2);
        if (c2 >= 0x80) return c2 & 0x7F; // < 0x80 => key pressed, >=0x80 => key released (swallow)
    }
}

#endif
