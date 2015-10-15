peliCAN
=======

peliCAN is designed as a simple visualization software for hobby projects running under Linux and Windows. 
It supports SocketCAN and the SLCAN protocol defined by LAWICEL

Tested under Gentoo Linux with SocketCAN and SLCAN over UART as well as under Windows 7/10 with SLCAN over UART

For further informations please see http://www.hans-dampf.org or contact mhoffacker3@googlemail.com

Build instructions:

Required libs
- Working Qt dev installation
- Working Qt serial port installation

If your kernel does not support SocketCAN or you are using Windows, edit src/peliCAN.pro and remove the USE_SOCKET_CAN define.

- cd ./src
- qmake
- make

The application should build now.

Version History:

v0.0.1: 
- Initial version

v0.0.5: 
- Support for LAWICEL loopback mode added
- Sending of CAN Frames
- Enumeration of COM ports
- Several bugfixes

v0.0.9: 
- Change from custom .xml signal definition to .dbc file
- Replaced Qwt with QCustomPlot
- Added QuadI interface as an example
- Removed conversion to human readable file. Replaced by an example Python script
- Several bugfixes
- Open Issue: Add sending (un)signed integers, float32, float64
- Open Issue: Add conversion for litte/big endian
- Open Issue: Add colors to graph
