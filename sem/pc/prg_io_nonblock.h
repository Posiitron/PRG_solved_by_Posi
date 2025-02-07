/*
 * Author Amir Akrami
 * Author: Prof. Ing. Jan Faigl, Ph.D.
 * Info: This project was inspired by the coding video tutorials provided by
 * CTU, course B3B36PRG, thanks to Prof. Ing. Jan Faigl, Ph.D. Source:
 * https://cw.fel.cvut.cz/wiki/courses/b3b36prg/resources/prgsem
 */
#ifndef __PRG_IO_NONBLOCK_H__
#define __PRG_IO_NONBLOCK_H__

/// ----------------------------------------------------------------------------
/// @brief io_open_read
///
/// @param fname  -- device name
///
/// @return
/// ----------------------------------------------------------------------------
int io_open_read(const char *fname);

/// ----------------------------------------------------------------------------
/// @brief io_open_write
///
/// @param fname  -- device name
///
/// @return
///
/// Be aware that the opening named pipe for writing is blocked until the pipe
/// is opened for reading. Thus, run a program that opens the pipe or call,
/// e.g., 'tail -f fname', where 'fname' is the filename name of the named pipe
/// being opened for writing.
/// ----------------------------------------------------------------------------
int io_open_write(const char *fname);

/// ----------------------------------------------------------------------------
/// @brief io_close
///
/// @param fd
///
/// @return
/// ----------------------------------------------------------------------------
int io_close(int fd);

/// ----------------------------------------------------------------------------
/// @brief io_putc
///
/// @param fd
/// @param c
///
/// @return
/// ----------------------------------------------------------------------------
int io_putc(int fd, char c);

/// ----------------------------------------------------------------------------
/// @brief io_getc
///
/// @param fd
///
/// @return
/// ----------------------------------------------------------------------------
int io_getc(int fd);

/// ----------------------------------------------------------------------------
/// @brief io_getc_timeout
///
/// @param fd
///
/// @return -1 on error, 0 no byte ready within the timeout, 1 one byte read
/// ----------------------------------------------------------------------------
int io_getc_timeout(int fd, int timeout_ms, unsigned char *c);

#endif

/* end of prg_io_nonblock.h */
