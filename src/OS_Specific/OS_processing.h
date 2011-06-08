#ifndef OS_PROCESSING_H
#define OS_PROCESSING_H

#include "OS_Specific/OS_Specific.h"

#ifdef OS_WINDOWS
#include "windows/processing/OS_VSTPlugNode2x.h"
#else ifdef OS_MAC
#include "mac/processing/OS_VSTPlugNode2x.h"
#endif


#endif 