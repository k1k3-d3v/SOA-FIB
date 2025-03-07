/*
 * libc.h - macros per fer els traps amb diferents arguments
 *          definició de les crides a sistema
 */
 
#ifndef __LIBC_H__
#define __LIBC_H__

#include <stats.h>

int write(int fd, char *buffer, int size);

void itoa(int a, char *b);

int gettime();

int strlen(char *a);

void perror();

void zeos_strcpy(char *dest, const char *src);

int getpid();

int fork();

void exit();

#endif  /* __LIBC_H__ */
