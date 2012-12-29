/*
 * ===========================================================================================================
 * OS_VSTPlugin2x.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#ifdef FRX_OS_LINUX

#include "OS_VSTPlugin2x.h" 

namespace processing {

OS_VSTPlugNode2x::HostCallBackOnInit OS_VSTPlugNode2x::callBkOnInit;
int OS_VSTPlugNode2x::shellPlugIdOnInit = 0;

} // namespace(s)

#endif //#ifdef FRX_OS_LINUX


