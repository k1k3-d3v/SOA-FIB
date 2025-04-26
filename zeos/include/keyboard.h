#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#define NUM_KEYS      128        /* 0-127: scancodes que manejamos            */
#define TICK_MS       10         /* 1 tick de ZeOS = 10 ms  (re-usa en pause) */

extern unsigned char key_state[NUM_KEYS];   /* 1=pressed, 0=released           */

/* prototipos internos */
int  sys_get_keyboard_state(char *user_buf);
int  sys_pause(int ms);

#endif  /* __KEYBOARD_H__ */
