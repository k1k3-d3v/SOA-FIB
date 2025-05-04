/* user.c — pruebas de pause y teclado */

#include <libc.h>

#define NUM_KEYS 128

int __attribute__((__section__(".text.main")))
main(void)
{
    char buf[16];
    int t0, t1, dt, len;
    unsigned char keyboard[NUM_KEYS];

    /* 1) Medición de pause(1000) */
    write(1, "=== Test pause(1000) ===\n", 24);
    t0 = gettime();
    pause(1000);
    t1 = gettime();
    dt = t1 - t0;
    itoa(dt, buf);
    for (len = 0; buf[len]; len++);
    write(1, "Ticks avanzados: ", 17);
    write(1, buf, len);
    write(1, "\n\n", 2);

    /* 2) Test “Tic–Tac” 5 veces con pause(5000) */
    write(1, "=== Tic–Tac x5 (5000 ms) ===\n", 28);
    for (int i = 0; i < 5; i++) {
        write(1, "Tic ", 4);
        pause(5000);
        write(1, "Tac\n", 4);
        pause(5000);
    }
    write(1, "\n", 1);

    /* 3) Lectura contínua del teclado */
    write(1, "=== Test teclado (GetKeyboardState) ===\n", 39);
    while (1) {
        if (GetKeyboardState((char*)keyboard) == 0) {
            /* Recorremos todos los scancodes y mostramos los pulsados */
            for (int sc = 0; sc < NUM_KEYS; sc++) {
                if (keyboard[sc]) {
                    /* Convertir scancode a string */
                    itoa(sc, buf);
                    for (len = 0; buf[len]; len++);
                    write(1, "Scancode pressed: ", 18);
                    write(1, buf, len);
                    write(1, "\n", 1);
                }
            }
        } else {
            write(1, "Error leyendo teclado\n", 22);
        }
        /* pequeño retardo entre lecturas */
        pause(200);
    }

    return 0; /* nunca llega */
}
