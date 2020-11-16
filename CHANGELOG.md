# Changelog

- [Changelog](#changelog)
  - [1.2.1 (16.11.2020)](#121-16112020)
  - [1.2.0 (21.12.2019)](#120-21122019)

## 1.2.1 (16.11.2020)

- termios.h: include termiwin only on Windows
- Changed some function names
  - `openSerial` => `open_serial`
  - `writeToSerial` => `write_serial`
  - `readFromSerial` => `read_serial`
  - `closeSerial` => `close_serial`
  - `selectSerial` => `select_serial`
- `open_serial` use `const char*` device name
- cmake build

## 1.2.0 (21.12.2019)

- Don't build on Linux/MacOS
- Termios header
- Fixed ifndef in termiWin.h
- Added strncat_s to build on Visual Studio without enabling unsafe code
- Added termiWin version macros (TERMIWIN_VERSION, TERMIWIN_MAJOR_VERSION, TERMIWIN_MINOR_VERSION)
- Added travis-CI.yml to project
