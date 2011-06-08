#ifndef OS_SPECIFIC_H
#define OS_SPECIFIC_H


#if _WIN32
#define OS_WINDOWS

#elif TARGET_API_MAC_CARBON
#define OS_MAC
#endif


#endif 