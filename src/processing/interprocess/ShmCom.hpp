/*
 * ShmCom.hpp
 *
 *  Created on: Thu Nov 28 14:57:41 2013
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_SHMCOM_H
#define SAMBAG_SHMCOM_H

#include <sambag/com/SharedMemory.hpp> 
#include <boost/interprocess/sync/interprocess_upgradable_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/sharable_lock.hpp>
#include <boost/tuple/tuple.hpp>
#include <sambag/com/SharedMemory.hpp>
#include <com/FrxConfig.h>

namespace frx { namespace processing { namespace interprocess {

namespace si = sambag::com::interprocess;
typedef boost::shared_ptr<si::SharedMemoryObject> SharedMemoryObjectPtr;
typedef boost::shared_ptr<si::MappedRegion> MappedRegionPtr;
using sambag::com::interprocess::Integer;
using sambag::com::interprocess::UInteger;

SharedMemoryObjectPtr createSharedMemoryObject(const char * name, size_t maxMemory) ;
SharedMemoryObjectPtr findSharedMemoryObject(const char * name);
boost::tuple<void*, UInteger, MappedRegionPtr>
ipMalloc(SharedMemoryObjectPtr shm, UInteger size);
boost::tuple<void*, UInteger, MappedRegionPtr>
ipOpen(SharedMemoryObjectPtr shm);
void ipFree(const char *name);
UInteger checksum(void *ptr, UInteger bytesize);
/**
 * @brief dependent on which OS is used some names
 * can fail the shm creation.
 */
std::string normalizeStringForShmId(const std::string &id);

/**
 * @brief use instead of strcpy.
 * check length against FRX_SHMSESS_MAX_PATH_LENGTH
 * throws if src.length() > max
 */
void shm_cpypath(char *dst, const std::string &src);
/**
 * @brief use instead of strcpy.
 * check length against max
 * truncates if src.length() > max
 */
void shm_cpystr(char *dst, const std::string &src,
    size_t max=FRX_SHMSESS_MAX_STR_LENGTH);
/**
 * @brief use instead of strcpy.
 * check length against max
 * throws if src.length() > max
 */
void shm_cpystrex(char *dst, const std::string &src,
    size_t max=FRX_SHMSESS_MAX_STR_LENGTH);
}}} // namespace(s)



#endif /* SAMBAG_SHMCOM_H */
