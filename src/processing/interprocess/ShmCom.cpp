/*
 * ShmCom.cpp
 *
 *  Created on: Thu Nov 28 14:57:41 2013
 *      Author: Johannes Unger
 */

#include "ShmCom.hpp"
#include <sambag/com/SharedMemoryImpl.hpp> 
#include <boost/functional/hash.hpp>
#include <boost/regex.hpp>
#include <sambag/com/exceptions/IllegalArgumentException.hpp>

namespace frx { namespace processing { namespace interprocess {

SharedMemoryObjectPtr createSharedMemoryObject(const char * name, size_t maxMemory) {
    using namespace boost::interprocess;
    return SharedMemoryObjectPtr(
		new SAMBAG_SHARED_MEMORY_OBJECT_CREATE(open_or_create, name, read_write, maxMemory)
    );
}
SharedMemoryObjectPtr findSharedMemoryObject(const char * name) {
    using namespace boost::interprocess;
    return SharedMemoryObjectPtr(
		new si::SharedMemoryObject(open_only, name, read_write)
    );
}
boost::tuple<void*, UInteger, MappedRegionPtr>
ipMalloc(SharedMemoryObjectPtr shm, UInteger size)
{
    using namespace boost::interprocess;
    if (size==0) {
        return NULL;
    }
    SAMBAG_SHARED_MEMORY_TRUNC(*(shm.get()), size+sizeof(int));
    MappedRegionPtr mp = MappedRegionPtr(
		new si::MappedRegion(*(shm.get()), read_write)
    );
    void *res = mp->get_address();
    
    UInteger *memorySize = (UInteger*)res;
    *memorySize = size;
    res = memorySize+1;
    return boost::make_tuple(res, size, mp);
}

boost::tuple<void*, UInteger, MappedRegionPtr>
ipOpen(SharedMemoryObjectPtr shm)
{
    using namespace boost::interprocess;
    MappedRegionPtr mp = MappedRegionPtr(
		new si::MappedRegion(*(shm.get()), read_write)
    );
    void *res =  mp->get_address();
    UInteger *memorySize = (UInteger*)res;
    res = memorySize+1;
    return boost::make_tuple(res, *memorySize, mp);
}

void ipFree(const char *name)
{
    SAMBAG_SHARED_MEMORY_REMOVE(name);
}

UInteger checksum(void *ptr, UInteger bytesize) {
    unsigned char *c = (unsigned char*)ptr;
    std::stringstream ss;
    while (bytesize-- > 0) {
        ss<<*c;
        ++c;
    }
    boost::hash<std::string> stringHash;
    return stringHash(ss.str());
}


std::string normalizeStringForShmId(const std::string &_id) {
    return boost::regex_replace(_id, boost::regex("[^\\w]"), "");
}


void shm_cpypath(char *dst, const std::string &src) {
    shm_cpystrex(dst, src, FRX_SHMSESS_MAX_PATH_LENGTH);
}
void shm_cpystr(char *dst, const std::string &src, size_t max) {
    strncpy(dst, src.c_str(), max);
}
void shm_cpystrex(char *dst, const std::string &src, size_t max) {
    if (src.length() > max) {
        using sambag::com::exceptions::IllegalArgumentException;
        SAMBAG_THROW(IllegalArgumentException, "'" + src + "' cpystrex length out of bounds");
    }
    strcpy(dst, src.c_str());
}
}}}

