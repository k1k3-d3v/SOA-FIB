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

    /* 3) Lectura continua del teclado */
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

    /* 4) Test pantalla compartida */
    write(1, "=== Test pantalla compartida ===\n", 33);
    void *screen = StartScreen();
    if (screen == (void *)-1) {
        write(1, "Error: No se pudo asignar la pantalla compartida.\n", 51);
    } else {
        write(1, "Pantalla asignada correctamente. Escribiendo en la pantalla...\n", 63);

        /* Escribir texto en la pantalla compartida */
        char *video_mem = (char *)screen;
        video_mem[0] = 'H';
        video_mem[1] = 0x0F;  // color blanco sobre negro
        video_mem[2] = 'e';
        video_mem[3] = 0x0F;
        video_mem[4] = 'l';
        video_mem[5] = 0x0F;
        video_mem[6] = 'l';
        video_mem[7] = 0x0F;
        video_mem[8] = 'o';
        video_mem[9] = 0x0F;

        /* Más texto */
        video_mem[10] = ' ';
        video_mem[11] = 0x0F;
        video_mem[12] = 'W';
        video_mem[13] = 0x0F;
        video_mem[14] = 'o';
        video_mem[15] = 0x0F;
        video_mem[16] = 'r';
        video_mem[17] = 0x0F;
        video_mem[18] = 'l';
        video_mem[19] = 0x0F;
        video_mem[20] = 'd';
        video_mem[21] = 0x0F;
    }

    return 0; /* nunca llega */
}