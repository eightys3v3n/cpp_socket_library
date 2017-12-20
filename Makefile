CC_FLAGS=-Wall -Wextra --std=c++17 -DVERBOSITY=0
LINUX_CC=g++
LINUX_CC_FLAGS=-DLINUX
WINDOWS_CC=x86_64-w64-mingw32-g++-posix
WINDOWS_CC_FLAGS=-DWINDOWS -static
WINDOWS_LIBRARIES=-lws2_32 -lwsock32

all: tests tests.exe


tests: socket_linux.o socket_tests_linux.o
	$(LINUX_CC) $(CC_FLAGS) $(LINUX_CC_FLAGS) $^ -o $@

tests.exe: socket_windows.o socket_tests_windows.o
	$(WINDOWS_CC) $(CC_FLAGS) $(WINDOWS_CC_FLAGS) $^ $(WINDOWS_LIBRARIES) -o $@
#-ladvapi32


socket_linux.o: socket_linux.cpp socket.hpp
	$(LINUX_CC) $(CC_FLAGS) $(LINUX_CC_FLAGS) -c $< -o $@

socket_windows.o: socket_windows.cpp socket.hpp
	$(WINDOWS_CC) $(CC_FLAGS) $(WINDOWS_CC_FLAGS) -c $< -o $@


 %_linux.o: %.cpp %.hpp
	$(LINUX_CC) $(CC_FLAGS) $(LINUX_CC_FLAGS) -c $< -o $@

%_linux.o: %.cpp
	$(LINUX_CC) $(CC_FLAGS) $(LINUX_CC_FLAGS) -c $^ -o $@

%_windows.o: %.cpp %.hpp
	$(WINDOWS_CC) $(CC_FLAGS) $(WINDOWS_CC_FLAGS) -c $< -o $@

%_windows.o: %.cpp
	$(WINDOWS_CC) $(CC_FLAGS) $(WINDOWS_CC_FLAGS) -c $^ -o $@


clean:
	rm -rf *.o socket_tests socket_tests.exe