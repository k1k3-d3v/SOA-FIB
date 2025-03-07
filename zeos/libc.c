/*
 * libc.c 
 */

#include <libc.h>
#include <types.h>
#include <errno.h>


int errno;

void itoa(int a, char *b)
{
  int i, i1;
  char c;
  
  if (a==0) { b[0]='0'; b[1]=0; return ;}
  
  i=0;
  while (a>0)
  {
    b[i]=(a%10)+'0';
    a=a/10;
    i++;
  }
  
  for (i1=0; i1<i/2; i1++)
  {
    c=b[i1];
    b[i1]=b[i-i1-1];
    b[i-i1-1]=c;
  }
  b[i]=0;
}

int strlen(char *a)
{
  int i;
  
  i=0;
  
  while (a[i]!=0) i++;
  
  return i;
}

void zeos_strcpy(char *dest, const char *src) {
    while (*src) {
        *dest = *src;
        dest++;
        src++;
    }

    *dest = '\0';
}


void perror() {
    char buff[256];

    switch(errno) {
        case 0: // No error
            zeos_strcpy(buff, "\nNo error\n");
            break;
        case EIO:
            zeos_strcpy(buff, "\nInput/output error\n");
            break;
        case EBADF:
            zeos_strcpy(buff, "\nBad file descriptor\n");
            break;
        case EACCES:
            zeos_strcpy(buff, "\nPermission denied\n");
            break;
        case ENOSYS:
            zeos_strcpy(buff, "\nFunction not implemented\n");
            break;
        case EINVAL:
            zeos_strcpy(buff, "\nInvalid argument\n");
            break;
        case EFAULT:
            zeos_strcpy(buff, "\nBad address\n");
            break;

    }

    write(1, buff, strlen(buff));
}






