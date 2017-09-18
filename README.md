# termiWin
### ~ A termios porting for Windows ~

## Introduction

termiWin is a library which purpose is to allow you to use on a Windows system, the same code used in Linux to communicate with a device through a serial port.
This is possible because termios’s functions have been rewritten to be compatible with Windows’s COM functions.

## Library Architecture

### The termios Structure 

This is the main structure of the library and it’s often passed as argument to the functions, it has the following members:
  tcflag_t c_iflag; /*input modes*/</br>
  tcflag_t c_oflag; /*output modes*/</br>
  tcflag_t c_cflag; /*control modes*/</br>
  tcflag_t c_lflag; /*local modes*/</br>
  cc_t c_cc[NCCS]; /*special character*/</br>
where tcflag_t is defined as an unsigned integer.</br>

The members of termios structure are used to set and retrieve the serial port configuration parameters. 
There five types of flags, sorted by mode; they are implemented in the same way as they are in termios, except for some, which  are not used.

#### Input modes flags

  INPCK – Not implemented, use PARENB instead. <br />
  IGNPAR – Not implemented, disable PARENB instead. <br />
  PARMRK – Not implemented, use PARENB instead.<br />
  ISTRIP – Not implemented, use CS7 instead.<br />
  IGNBRK – Not implemented.<br />
  IGNCR – Not implemented.<br />
  ICRNL – Not implemented.<br />
  INLCR – Not implemented.<br />
  IXOFF - If this bit is set, start/stop control on input is enabled. In other words, the computer sends STOP and START            characters as necessary to prevent input from coming in faster than programs are reading it. The idea is that the actual terminal hardware that is generating the input data responds to a STOP character by suspending transmission, and to a START character by  resuming transmission.<br />
  IXON - If this bit is set, start/stop control on output is enabled. In other words, if the computer receives a STOP character, it suspends output until a START character is received. In this case, the STOP and START characters are never passed to the application program. If this bit is not set, then START and STOP can be read as ordinary characters.<br />

#### Local modes flags

Since there’s no way to implement them in Windows, they have asbolutely no effect using termiWin, but you can keep the same you use in Linux for compatibilty.

#### Control modes flags

  CSTOPB - If this bit is set, two stop bits are used. Otherwise, only one stop bit is used.<br />
  PARENB - If this bit is set, generation and detection of a parity bit are enabled.<br />
  PARODD - This bit is only useful if PARENB is set. If PARODD is set, odd parity is used, otherwise even parity is used.<br />
  CSIZE - This is a mask for the number of bits per character.<br />
  CS5 - This specifies five bits per byte.<br />
  CS6 – This specifies six bits per byte.<br />
  CS7 – This specifies seven bits per byte.<br />
  CS8 – This specifies eight bits per byte.<br />
  
#### Output modes flags

Since there’s no way to implement them in Windows, they have asbolutely no effect using termiWin, but you can keep the same you use in Linux for compatibilty.

#### Special character array

  VEOF – Is the EOF character to be used during the communication.  <br />
  VEOL – Not implemented.<br />
  VERASE – Not implemented.<br />
  VINTR – Interrupt character.<br />
  VKILL – Not implemented.<br />
  VMIN – If set to 0, the port is set to not-blocking, otherwise to blocking.<br />
  VQUIT – Not implemented.<br />
  VSTART – Not implemented.<br />
  VSTOP – Not implemented.<br />
  VSUSP – Not implemented.<br />
  VTIME – Timeout for reading operations when COM is set to not-blocking.<br />
  
### Serial configuration functions

#### Int tcgetattr(int fd, struct termios *termios_p);
	
Sets in the internal DCB structures the current serial port parameters, it 
always has to be invoked before using tcsetattr. 
<br />Returns 0 if succeded, otherwise -1.

#### int tcsetattr(int fd, int optional_actions, struct termios *termios_p);

 Reads the flags set in the termios structure and sets the properly 		
parameters in the DCB structure and eventually it applies the parameters to the serial port. <br />
Returns 0 if succeded, otherwise -1.

#### int tcsendbreak(int fd, int duration);

Sends a break character to the serial port; duration is not implemented. <br />
Returns 0 if succeded, otherwise -1.

#### int tcdrain(int fd);

Waits until all output written to the serial port has been transmitted.<br />
Returns 0 if succeded, otherwise -1.

#### Int tcflush(int fd, int queue_selector);

  Discards data on serial port. queue_selector can assume the following values: <br />
    TCIFLUSH 		(discards data received but still not read). <br />
    TCOFLUSH 	(discards data written but still not transmitted), <br />
    TCIOFLUSH 	(discards both data received but still not read and data written but still not transmitted).<br />

Returns 0 if succeded, otherwise -1.<br />

#### Int tcflow(int fd, int action);

  Suspends transmission or receptions of data on serial port based on action. Action can assume the following values: 	<br />
    TCOON 	restarts suspended output. <br />
    TCIOFF	transmits a STOP character.      	<br />
    TCION 	transmits a START character.<br />
    TCOOF 	suspends output.<br />
	
Returns 0 if succeded, otherwise -1.<br />

#### Void cfmakeraw(struct termios *termios_p);

  Sets but doesn’t commit the following options for the serial port:<br />
  Charset: 8 bits<br />
  StopBits: one stop bit<br />
  Parity: no parity<br />

Use tcsetattr to commit them.

#### speed_t cfgetispeed(const struct termios *termios_p);
	
  Returns the input speed, speed can assume the same values of termios 	(B9600, B115200, …).<br />

#### speed_t cfgetospeed(const struct termios *termios_p);
	
  returns the output speed, speed can assume the same values of termios (B9600, B115200, …).<br />

#### int cfsetispeed(struct termios *termios_p, speed_t speed);

  Sets, but doesn’t commits the parameter of  speed for the serial port (in 	Windows there’s no distinction between 
  <br />input/output /control), speed can assume the same values of termios (B9600, B115200, …). 	<br />	
  Returns 0 if succeded, otherwise -1.

#### int cfsetospeed(struct termios *termios_p, speed_t speed);

  Sets, but doesn’t commits the parameter of speed for the serial port (in Windows there’s no distinction between input/output
  /control), speed can assume the same values of termios (B9600, B115200, ...). <br />		
  Returns 0 if succeded, otherwise -1.<br />

#### int cfsetsspeed(struct termios *termios_p, speed_t speed);

  Sets, but doesn’t commits the parameter of speed for the serial port (in Windows there’s no distinction between input/output
  /control), speed 	can assume the same values of termios (B9600, B115200, ...). 		
  Returns 0 if succeded, otherwise -1.
  
### Serial transmission/receiving - open/close Functions
  
#### Int openSerial(char* portname, int opt);

Open the serial port which name is portname with opt set for the port to be read only, write only or both read/write (O_RDONLY, 
O_WRONLY, O_RDWR). Returns the file descriptor (fd is actually useless in Windows with serial ports, but is set for 
compatibilty). The function can be called using open instead of openSerial (for termios compatibilty).<br />

#### Int closeSerial(int fd);

Closes the serial port. <br />
Returns 0 if succeded, otherwise -1. The function can be called using close instead of closeSerial<br />
(for termios compatibilty).

#### Int writeToSerial(int fd, char* buffer, int count);

Writes to serial “count” characters contained in buffer.<br />
Returns the number of transmitted characters or -1 if transmission  failed. <br />
The function can be called using write instead of writeToSerial (for termios compatibilty).

#### Int readFromSerial(int fd, char* buffer, int count);
	
Reads “count” bytes from serial port and put them into buffer. <br />
Returns the number of read bytes or -1 if read failed.<br />
The function can be called using read instead of readFromSerial (for termios compatibilty).

#### Int selectSerial(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

It behaves as termios select. <br />
Returns the file descriptor ready for the chosen operation or -1 if failed.<br /> 
The function can be called using select instead of selectSerial (for termios compatibility).

## License

termiWin is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as 
published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
termiWin is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with termiWin.  
If not, see <http://www.gnu.org/licenses/>.
