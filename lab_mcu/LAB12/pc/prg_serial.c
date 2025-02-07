/*
 * File name: prg_serial.c
 * Date:      2017/04/07 22:34
 * Author:    Jan Faigl
 */

#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <poll.h>

#include "prg_serial.h"

#ifndef BAUD_RATE
#define BAUD_RATE B115200
#endif

/// ----------------------------------------------------------------------------
void my_assert(int r, const char *fcname, int line, const char *fname)
{
   if (!(r)) {
      fprintf(stderr, "ERROR: my_assert FAIL: %s() line %d in %s\n", fcname, line, fname);
      exit(-1);
   }
}

/// ----------------------------------------------------------------------------
int serial_open(const char *fname)
{
   int fd = open(fname, O_RDWR | O_NOCTTY | O_SYNC);
   my_assert(fd != -1, __func__, __LINE__, __FILE__);
   struct termios term;
   my_assert(tcgetattr(fd, &term) >= 0, __func__, __LINE__, __FILE__);
   cfmakeraw(&term);
   term.c_cc[VTIME] = 2; //set vtime 
   term.c_cc[VMIN] = 0;
   cfsetispeed(&term, BAUD_RATE);
   cfsetospeed(&term, BAUD_RATE);
   my_assert(tcsetattr(fd, TCSADRAIN, &term) >= 0, __func__, __LINE__, __FILE__);
   my_assert(fcntl(fd, F_GETFL) >= 0, __func__, __LINE__, __FILE__);
   my_assert(tcsetattr(fd, TCSADRAIN, &term) >= 0, __func__, __LINE__, __FILE__);
   my_assert(fcntl(fd, F_GETFL) >= 0, __func__, __LINE__, __FILE__);
   tcflush(fd, TCIFLUSH);
   tcsetattr(fd, TCSANOW, &term);

   /* Set the serial port to non block mode
   int flags = fcntl(fd, F_GETFL);
   flags &= ~O_NONBLOCK;
   my_assert(fcntl(fd, F_SETFL, flags) >= 0, __func__, __LINE__, __FILE__);
   */
   return fd;
}

/// ----------------------------------------------------------------------------
int serial_close(int fd)
{
   return close(fd);
}

/// ----------------------------------------------------------------------------
int serial_putc(int fd, char c)
{
   return write(fd, &c, 1);
}

/// ----------------------------------------------------------------------------
int serial_getc(int fd)
{
   char c;
   int r = read(fd, &c, 1);
   return r == 1 ? c : -1;
}

/// ----------------------------------------------------------------------------
int serial_getc_timeout(int fd, int timeout_ms, unsigned char *c)
{
   struct pollfd ufdr[1];
   int r = 0;
   ufdr[0].fd = fd;
   ufdr[0].events = POLLIN | POLLRDNORM;
   if ((poll(&ufdr[0], 1, timeout_ms) > 0) && (ufdr[0].revents & (POLLIN | POLLRDNORM))) {
      r = read(fd, c, 1);
   }
   return r;
}

/* end of prg_serial.c */
