all: socket.o socket_test.exe

# GCC ORDER - libraries must be AFTER the files that use them
socket_test.exe: socket.cpp socket_unix.cpp socket_windows.cpp
	$(gcc) -DSA socket.cpp -o socket_test.exe -lws2_32 -lmswsock -ladvapi32

socket.o: socket.cpp socket_unix.cpp socket_windows.cpp
	$(gcc) -c socket.cpp -o socket.o -lws2_32 -lmswsock -ladvapi32

include ../common.make