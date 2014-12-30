peliCAN
=======

peliCAN is designed as a simple visualization software for hobby projects running under Linux and Windows. I tested it with the USB to CAN converter by LAWICE AB and my own USB to CAN converter with SocketCAN and UART under Linux and UART only under Windows 7.

For further informations please see http://www.hans-dampf.org

Build instructions:

Required libs
- Working Qt dev installation
- Working Qwt dev installation in the version 6.1.2 or above
- Working Qt serial port installation

If your kernel does not support SocketCAN, edit src/peliCAN.pro and remove the USE_SOCKET_CAN define.

- Change to src.
- qmake
- make

The application should build now.

Version History:

v0.0.1: 

- Initial version

v0.0.5: 

- Support for LAWICE loopback mode added
- Sending of CAN Frames
- Enumeration of COM ports
- Several bugfixes
