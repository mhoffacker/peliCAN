peliCAN
=======

Build instructions:

Required libs
- Working Qt dev installation
- Working Qwt dev installation

If your kernel does not support SocketCAN, edit src/peliCAN.pro and remove the USE_SOCKET_CAN define.

- Change to src.
- qmake
- make

The application should build now.
