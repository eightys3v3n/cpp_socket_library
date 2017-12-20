#if not defined WINDOWS and not defined LINUX

#if defined __WIN32
  #define WINDOWS
  //#warning Detected __WIN32: Windows OS
#elif defined __linux__
  #define LINUX
  //#warning Detected __linux__: Linux OS
#else
  #warning Unable to detect operating system. Please define WINDOWS, or LINUX.
#endif // OS detection
#endif // Whether OS is already defined