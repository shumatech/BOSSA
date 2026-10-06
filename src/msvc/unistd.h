// File unistd.h for MSVC

#include <cstdint>
#include <windows.h>

inline int usleep(uint32_t usecs)
{
	Sleep(usecs/1000);
	return 0;
}

inline unsigned int sleep(unsigned int seconds)
{
	Sleep((uint32_t)seconds * 1000);
	return 0;
}

// End
