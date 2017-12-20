l=g++ -Wall -Wextra --std=c++17 -DLINUX
w=x86_64-w64-mingw32-g++-posix -Wall -Wextra --std=c++17 -static -DWINDOWS

all: socket_tests socket_tests.exe

socket_tests: socket_tests_linux.o
	$l -DSA -o $@ $^

socket_tests_linux.o: socket.cpp socket_unix.cpp
	$l -DSA -c -o $@ socket.cpp

socket_tests.exe: socket_tests_windows.o
	$w -DSA -o $@ $^ -lws2_32 -lmswsock -ladvapi32

socket_tests_windows.o: socket.cpp socket_windows.cpp
	$w -DSA -c -o $@ socket.cpp

clean:
	if [[ -n *.o ]]; then rm *.o; fi
	if [ -f socket_tests ]; then rm socket_tests; fi
	if [ -f socket_tests.exe ]; then rm socket_tests.exe; fi