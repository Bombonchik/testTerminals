
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <pty.h>
#endif

