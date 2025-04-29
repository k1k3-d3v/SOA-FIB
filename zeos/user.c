#include <libc.h>

char keymap[128];

int __attribute__((__section__(".text.main"))) main(void)
{
    write(1, "Esperando pulsaciones de teclado...\n", 36);

    while (1) {
      GetKeyboardState(keymap);

      for (int i = 0; i < 128; ++i) {
        if (keymap[i] == 1) {
          write(1, "Se ha detectado una pulsacion\n", 31);
        }
      }

      pause(50);
    }

    return 0;
}
