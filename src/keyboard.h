#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

void keyboard_init(void);
void keyboard_deinit(void);
int read_key();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if !defined(__arm__)

#define VK_UP           (0xFF)
#define VK_DOWN         (0xFE)
#define VK_NEXT         (0xFD)
#define VK_PREVIOUS     (0xFC)
#define VK_VOLUME_UP    ']'
#define VK_VOLUME_DOWN  '['
#define VK_STATUS       's'
#define VK_TOGGLE       ' '
#define VK_QUIT         'x'
#define VK_1            '1'
#define VK_2            '2'
#define VK_3            '3'
#define VK_4            '4'
#define VK_5            '5'
#define VK_6            '6'
#define VK_7            '7'
#define VK_8            '8'
#define VK_9            '9'
#define VK_QUEUE        'q'
#define VK_NOW_PLAYING  'n'
#define VK_LEFT_SELECT  ','
#define VK_RIGHT_SELECT '.'
#define VK_PLAYLISTS    'l'

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#else // __arm__

// NP 210 keycodes: for code c, the phone sends '0x4B 0x80|code' on press, '0x4B code' on release
#define NP210_KEY_FUNCTION	(0x21)
#define NP210_KEY_DELETE	(0x22)
#define NP210_KEY_LEFT_SELECT	(0x23)
#define NP210_KEY_UP            (0x24)
#define NP210_KEY_DOWN          (0x25)
#define NP210_KEY_RIGHT_SELECT  (0x26)
#define NP210_KEY_HOLD          (0x27)
#define NP210_KEY_RECORD        (0x28)
#define NP210_KEY_ALT           (0x29)
#define NP210_KEY_HANGUP        (0x2A)
#define NP210_KEY_1             (0x2B)
#define NP210_KEY_4             (0x2C)
#define NP210_KEY_7             (0x2D)
#define NP210_KEY_ASTERISK      (0x2E)
#define NP210_KEY_2             (0x2F)
#define NP210_KEY_5             (0x30)
#define NP210_KEY_8             (0x31)
#define NP210_KEY_0             (0x32)
#define NP210_KEY_3             (0x33)
#define NP210_KEY_6             (0x34)
#define NP210_KEY_9             (0x35)
#define NP210_KEY_POUND         (0x36)
#define NP210_KEY_SPEAKER       (0x37)
#define NP210_KEY_VOLUME_UP     (0x38)
#define NP210_KEY_VOLUME_DOWN   (0x39)
#define NP210_KEY_MIC_OFF       (0x3A)
#define NP210_KEY_HOOK          (0x3B)
#define NP210_KEY_DIRECTORY     (0x3C)
#define NP210_KEY_MAIL          (0x3D)
#define NP210_KEY_SETTINGS	(0x3E)

// virtual key codes (mapping)
#define VK_VOLUME_UP            NP210_KEY_VOLUME_UP
#define VK_VOLUME_DOWN          NP210_KEY_VOLUME_DOWN
#define VK_NEXT                 NP210_KEY_POUND
#define VK_PREVIOUS             NP210_KEY_ASTERISK
#define VK_STATUS               NP210_KEY_ALT
#define VK_TOGGLE               NP210_KEY_0
#define VK_QUIT                 NP210_KEY_HANGUP
#define VK_1                    NP210_KEY_1
#define VK_2                    NP210_KEY_2
#define VK_3                    NP210_KEY_3
#define VK_4                    NP210_KEY_4
#define VK_5                    NP210_KEY_5
#define VK_6                    NP210_KEY_6
#define VK_7                    NP210_KEY_7
#define VK_8                    NP210_KEY_8
#define VK_9                    NP210_KEY_9
#define VK_UP                   NP210_KEY_UP
#define VK_DOWN                 NP210_KEY_DOWN
#define VK_QUEUE                NP210_KEY_MIC_OFF
#define VK_NOW_PLAYING          NP210_KEY_SPEAKER
#define VK_LEFT_SELECT          NP210_KEY_LEFT_SELECT
#define VK_RIGHT_SELECT         NP210_KEY_RIGHT_SELECT
#define VK_PLAYLISTS            NP210_KEY_HOLD

#endif

#endif
