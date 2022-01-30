/* termiWin.c
*
*   Copyright (C) 2017 Christian Visintin - christian.visintin1997@gmail.com
*
*   This file is part of "termiWin: a termios porting for Windows"
*
*   termiWin is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   termiWin is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with termiWin.  If not, see <http://www.gnu.org/licenses/>.
*
*/

#include "termiWin.h"

#ifdef _WIN32

#ifdef __cplusplus > 201711L
  #define TERMIWIN_MAYBE_UNUSED [[maybe_unused]]
#else
  #ifdef __GNUC__
    #define TERMIWIN_MAYBE_UNUSED __attribute__((unused))
  #else
    #define TERMIWIN_MAYBE_UNUSED
  #endif
#endif

#include <fcntl.h>
#include <stdlib.h>

typedef struct COM {
  HANDLE hComm;
  int fd; //Actually it's completely useless
  char port[128];
} COM;

DCB SerialParams = { 0 }; //Initializing DCB structure
struct COM com;
COMMTIMEOUTS timeouts = { 0 }; //Initializing COMMTIMEOUTS structure

//LOCAL functions

//nbyte 0->7

int getByte(tcflag_t flag, int nbyte, int nibble) {

  int byte;
  if (nibble == 1)
    byte = (flag >> (8 * (nbyte)) & 0x0f);
  else
    byte = (flag >> (8 * (nbyte)) & 0xf0);
  return byte;
}

//INPUT FUNCTIONS

enum{
  i_IXOFF = 0x01,
  i_IXON = 0x02,
  i_IXOFF_IXON = 0x03,
  i_PARMRK = 0x04,
  i_PARMRK_IXOFF = 0x05,
  i_PARMRK_IXON = 0x06,
  i_PARMRK_IXON_IXOFF = 0x07
};

int getIXOptions(tcflag_t flag) {
  int byte = getByte(flag, 1, 1);

  return byte;
}

//LOCALOPT FUNCTIONS

enum{
  l_NOECHO = 0x00,
  l_ECHO = 0x01,
  l_ECHO_ECHOE = 0x03,
  l_ECHO_ECHOK = 0x05,
  l_ECHO_ECHONL = 0x09,
  l_ECHO_ECHOE_ECHOK = 0x07,
  l_ECHO_ECHOE_ECHONL = 0x0b,
  l_ECHO_ECHOE_ECHOK_ECHONL = 0x0f,
  l_ECHO_ECHOK_ECHONL = 0x0d,
  l_ECHOE = 0x02,
  l_ECHOE_ECHOK = 0x06,
  l_ECHOE_ECHONL = 0x0a,
  l_ECHOE_ECHOK_ECHONL = 0x0e,
  l_ECHOK = 0x04,
  l_ECHOK_ECHONL = 0x0c,
  l_ECHONL = 0x08
};

int getEchoOptions(tcflag_t flag) {
  int byte = getByte(flag, 1, 1);
  return byte;
}

enum{
  l_ICANON = 0x10,
  l_ICANON_ISIG = 0x50,
  l_ICANON_IEXTEN = 0x30,
  l_ICANON_NOFLSH = 0x90,
  l_ICANON_ISIG_IEXTEN = 0x70,
  l_ICANON_ISIG_NOFLSH = 0xd0,
  l_ICANON_IEXTEN_NOFLSH = 0xb0,
  l_ICANON_ISIG_IEXTEN_NOFLSH = 0xf0,
  l_ISIG = 0x40,
  l_ISIG_IEXTEN = 0x60,
  l_ISIG_NOFLSH = 0xc0,
  l_ISIG_IEXTEN_NOFLSH = 0xe0,
  l_IEXTEN = 0x20,
  l_IEXTEN_NOFLSH = 0xa0,
  l_NOFLSH = 0x80,
};

int getLocalOptions(tcflag_t flag) {
  int byte = getByte(flag, 1, 0);
  return byte;
}

enum{
  l_TOSTOP = 0x01
};

int getToStop(tcflag_t flag) {
  int byte = getByte(flag, 1, 1);
  return byte;
}

//CONTROLOPT FUNCTIONS

int getCharSet(tcflag_t flag) {

  //FLAG IS MADE UP OF 8 BYTES, A FLAG IS MADE UP OF A NIBBLE -> 4 BITS, WE NEED TO EXTRACT THE SECOND NIBBLE (1st) FROM THE FIFTH BYTE (6th).
  int byte = getByte(flag, 1, 1);

  switch (byte) {

  case 0X0:
    return CS5;
    break;

  case 0X4:
    return CS6;
    break;

  case 0X8:
    return CS7;
    break;

  case 0Xc:
    return CS8;
    break;

  default:
    return CS8;
    break;
  }
}

enum{
  c_ALL_ENABLED = 0xd0,
  c_PAREVEN_CSTOPB = 0x50,
  c_PAREVEN_NOCSTOPB = 0x40,
  c_PARODD_NOCSTOPB = 0xc0,
  c_NOPARENB_CSTOPB = 0x10,
  c_ALL_DISABLED = 0x00,
};

int getControlOptions(tcflag_t flag) {
  int byte = getByte(flag, 1, 0);
  return byte;
}

//LIBFUNCTIONS

int tcgetattr(int fd, struct termios* TERMIWIN_MAYBE_UNUSED termios_p) {

  if (fd != com.fd) return -1;
  int TERMIWIN_MAYBE_UNUSED ret = 0;

  ret = GetCommState(com.hComm, &SerialParams);

  return 0;
}

int tcsetattr(int fd, int TERMIWIN_MAYBE_UNUSED optional_actions, const struct termios* termios_p) {

  if (fd != com.fd) return -1;
  int ret = 0;

  //Store flags into local variables
  tcflag_t iflag = termios_p->c_iflag;
  tcflag_t lflag = termios_p->c_lflag;
  tcflag_t cflag = termios_p->c_cflag;
  tcflag_t TERMIWIN_MAYBE_UNUSED oflag = termios_p->c_oflag;

  //iflag

  int IX = getIXOptions(iflag);

  if ((IX == i_IXOFF_IXON) || (IX == i_PARMRK_IXON_IXOFF)) {

    SerialParams.fOutX = TRUE;
    SerialParams.fInX = TRUE;
    SerialParams.fTXContinueOnXoff = TRUE;
  }

  //lflag
  int TERMIWIN_MAYBE_UNUSED EchoOpt = getEchoOptions(lflag);
  int TERMIWIN_MAYBE_UNUSED l_opt = getLocalOptions(lflag);
  int TERMIWIN_MAYBE_UNUSED tostop = getToStop(lflag);

  //Missing parameters...

  //cflags

  int CharSet = getCharSet(cflag);
  int c_opt = getControlOptions(cflag);

  switch (CharSet) {

  case CS5:
    SerialParams.ByteSize = 5;
    break;

  case CS6:
    SerialParams.ByteSize = 6;
    break;

  case CS7:
    SerialParams.ByteSize = 7;
    break;

  case CS8:
    SerialParams.ByteSize = 8;
    break;
  }

  switch (c_opt) {

  case c_ALL_ENABLED:
    SerialParams.Parity = ODDPARITY;
    SerialParams.StopBits = TWOSTOPBITS;
    break;

  case c_ALL_DISABLED:
    SerialParams.Parity = NOPARITY;
    SerialParams.StopBits = ONESTOPBIT;
    break;

  case c_PAREVEN_CSTOPB:
    SerialParams.Parity = EVENPARITY;
    SerialParams.StopBits = TWOSTOPBITS;
    break;

  case c_PAREVEN_NOCSTOPB:
    SerialParams.Parity = EVENPARITY;
    SerialParams.StopBits = ONESTOPBIT;
    break;

  case c_PARODD_NOCSTOPB:
    SerialParams.Parity = ODDPARITY;
    SerialParams.StopBits = ONESTOPBIT;
    break;

  case c_NOPARENB_CSTOPB:
    SerialParams.Parity = NOPARITY;
    SerialParams.StopBits = TWOSTOPBITS;
    break;
  }

  //aflags

  /*
  int OP;
  if(oflag == OPOST)
  else ...
  */
  //Missing parameters...

  //special characters

  if (termios_p->c_cc[VEOF] != 0) SerialParams.EofChar = (char)termios_p->c_cc[VEOF];
  if (termios_p->c_cc[VINTR] != 0) SerialParams.EvtChar = (char)termios_p->c_cc[VINTR];

  if (termios_p->c_cc[VMIN] == 1) { //Blocking

    timeouts.ReadIntervalTimeout = 0;         // in milliseconds
    timeouts.ReadTotalTimeoutConstant = 0;    // in milliseconds
    timeouts.ReadTotalTimeoutMultiplier = 0;  // in milliseconds
    timeouts.WriteTotalTimeoutConstant = 0;   // in milliseconds
    timeouts.WriteTotalTimeoutMultiplier = 0; // in milliseconds

  } else { //Non blocking

    timeouts.ReadIntervalTimeout = termios_p->c_cc[VTIME] * 100;         // in milliseconds
    timeouts.ReadTotalTimeoutConstant = termios_p->c_cc[VTIME] * 100;    // in milliseconds
    timeouts.ReadTotalTimeoutMultiplier = termios_p->c_cc[VTIME] * 100;  // in milliseconds
    timeouts.WriteTotalTimeoutConstant = termios_p->c_cc[VTIME] * 100;   // in milliseconds
    timeouts.WriteTotalTimeoutMultiplier = termios_p->c_cc[VTIME] * 100; // in milliseconds
  }

  SetCommTimeouts(com.hComm, &timeouts);

  //EOF

  ret = SetCommState(com.hComm, &SerialParams);
  if (ret != 0)
    return 0;
  else
    return -1;
}

int tcsendbreak(int fd, int TERMIWIN_MAYBE_UNUSED duration) {

  if (fd != com.fd) return -1;

  int ret = 0;
  ret = TransmitCommChar(com.hComm, '\x00');
  if (ret != 0)
    return 0;
  else
    return -1;
}

int tcdrain(int fd) {

  if (fd != com.fd) return -1;
  return FlushFileBuffers(com.hComm);
}

int tcflush(int fd, int queue_selector) {

  if (fd != com.fd) return -1;
  int rc = 0;

  switch (queue_selector) {

  case TCIFLUSH:
    rc = PurgeComm(com.hComm, PURGE_RXCLEAR);
    break;

  case TCOFLUSH:
    rc = PurgeComm(com.hComm, PURGE_TXCLEAR);
    break;

  case TCIOFLUSH:
    rc = PurgeComm(com.hComm, PURGE_RXCLEAR);
    rc *= PurgeComm(com.hComm, PURGE_TXCLEAR);
    break;

  default:
    rc = 0;
    break;
  }

  if (rc != 0)
    return 0;
  else
    return -1;
}

int tcflow(int fd, int action) {

  if (fd != com.fd) return -1;
  int rc = 0;

  switch (action) {

  case TCOOFF:
    rc = PurgeComm(com.hComm, PURGE_TXABORT);
    break;

  case TCOON:
    rc = ClearCommBreak(com.hComm);
    break;

  case TCIOFF:
    rc = PurgeComm(com.hComm, PURGE_RXABORT);
    break;

  case TCION:
    rc = ClearCommBreak(com.hComm);
    break;

  default:
    rc = 0;
    break;
  }

  if (rc != 0)
    return 0;
  else
    return -1;
}

void cfmakeraw(struct termios* TERMIWIN_MAYBE_UNUSED termios_p) {

  SerialParams.ByteSize = 8;
  SerialParams.StopBits = ONESTOPBIT;
  SerialParams.Parity = NOPARITY;
}

speed_t cfgetispeed(const struct termios* TERMIWIN_MAYBE_UNUSED termios_p) {

  return SerialParams.BaudRate;
}

speed_t cfgetospeed(const struct termios* TERMIWIN_MAYBE_UNUSED termios_p) {

  return SerialParams.BaudRate;
}

int cfsetispeed(struct termios* TERMIWIN_MAYBE_UNUSED termios_p, speed_t speed) {

  SerialParams.BaudRate = speed;
  return 0;
}

int cfsetospeed(struct termios* TERMIWIN_MAYBE_UNUSED termios_p, speed_t speed) {

  SerialParams.BaudRate = speed;
  return 0;
}

int cfsetspeed(struct termios* TERMIWIN_MAYBE_UNUSED termios_p, speed_t speed) {

  SerialParams.BaudRate = speed;
  return 0;
}

ssize_t read_serial(int fd, void* buffer, size_t count) {

  if (fd != com.fd) return -1;
  int rc = 0;
  int ret;

  ret = ReadFile(com.hComm, buffer, count, &rc, NULL);

  if (ret == 0)
    return -1;
  else
    return rc;
}

ssize_t write_serial(int fd, const void* buffer, size_t count) {

  if (fd != com.fd) return -1;
  int rc = 0;
  int ret;

  ret = WriteFile(com.hComm, buffer, count, &rc, NULL);

  if (ret == 0)
    return -1;
  else
    return rc;
}

int open_serial(const char* portname, int opt) {

  if (strlen(portname) < 4) return -1;

  // Set to zero
  memset(com.port, 0x00, 128);

  //COMxx
  size_t portSize = 0;
  if (strlen(portname) > 4) {
    portSize = sizeof(char) * strlen("\\\\.\\COM10") + 1;
#ifdef _MSC_VER
    strncat_s(com.port, portSize, "\\\\.\\", strlen("\\\\.\\"));
#else
    strncat(com.port, "\\\\.\\", strlen("\\\\.\\"));
#endif
  }
  //COMx
  else {
    portSize = sizeof(char) * 5;
  }

#ifdef _MSC_VER
  strncat_s(com.port, portSize, portname, 4);
#else
  strncat(com.port, portname, 4);
#endif
  com.port[portSize] = 0x00;

  switch (opt) {

  case O_RDWR:
    com.hComm = CreateFile(com.port, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    break;

  case O_RDONLY:
    com.hComm = CreateFile(com.port, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    break;

  case O_WRONLY:
    com.hComm = CreateFile(com.port, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    break;
  }

  if (com.hComm == INVALID_HANDLE_VALUE) {
    return -1;
  }
  com.fd = atoi(portname + 3); // COMx and COMxx
  SerialParams.DCBlength = sizeof(SerialParams);
  return com.fd;
}

int close_serial(int TERMIWIN_MAYBE_UNUSED fd) {

  int ret = CloseHandle(com.hComm);
  if (ret != 0)
    return 0;
  else
    return -1;
}

int select_serial(int TERMIWIN_MAYBE_UNUSED nfds, fd_set* readfds, fd_set* TERMIWIN_MAYBE_UNUSED writefds, fd_set* TERMIWIN_MAYBE_UNUSED exceptfds, struct timeval* TERMIWIN_MAYBE_UNUSED timeout) {

  SetCommMask(com.hComm, EV_RXCHAR);
  DWORD dwEventMask;
  if (WaitCommEvent(com.hComm, &dwEventMask, NULL) == 0) {
    return -1; // Return -1 if failed
  }
  if (dwEventMask == EV_RXCHAR) {
    return com.fd;
  } else {
    if (readfds) {
      // Clear file descriptor if event is not RXCHAR
      FD_CLR(com.fd, readfds);
    }
  }
  // NOTE: write event not detectable!
  // NOTE: no timeout
  return 0; // No data
}

//Returns hComm from the COM structure
HANDLE getHandle() {
  return com.hComm;
}

#endif
