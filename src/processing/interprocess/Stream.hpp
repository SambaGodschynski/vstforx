/*
 * Stream.hpp
 *
 *  Created on: Thu Sep 12 12:18:46 2013
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_STREAM_H
#define SAMBAG_STREAM_H

#include <boost/shared_ptr.hpp>
#include <string>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/interprocess_upgradable_mutex.hpp>
#include <processing/Frames.h>
#include <sambag/com/Interprocess.hpp>

typedef boost::interprocess::shared_memory_object SharedMemoryObject;
typedef boost::interprocess::mapped_region MappedRegion;

namespace frx { namespace processing { namespace interprocess {
//=============================================================================
/** 
  * @class Stream.
  * Interprocess audio stream
  */
class Stream {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<Stream> Ptr;
protected:
    //-------------------------------------------------------------------------
    Stream();
    //-------------------------------------------------------------------------
    size_t getNeededSize(size_t blockSize, size_t numChannel) const;
    //-------------------------------------------------------------------------
    void assignMemory(sambag::com::interprocess::PointerIterator &pIt,
                      size_t numChannel=0, size_t numBlockSize=0);
private:
    //-------------------------------------------------------------------------
    void *memory_ptr;
    //-------------------------------------------------------------------------
    size_t memorySize;
    //-------------------------------------------------------------------------
    sambag::com::interprocess::PointerIterator pIt;
    //-------------------------------------------------------------------------
    typedef boost::interprocess::interprocess_upgradable_mutex Mutex;
    //-------------------------------------------------------------------------
    Mutex *mutex;
    //-------------------------------------------------------------------------
    size_t *blockSize_ist, *numChannels_ist;
    //-------------------------------------------------------------------------
    int *num_references;
    //-------------------------------------------------------------------------
    double **buffer;
    //-------------------------------------------------------------------------
    void createBuffer(size_t blockSize_soll, size_t numChannels_soll);
    //-------------------------------------------------------------------------
    void openBuffer();
    //-------------------------------------------------------------------------
    std::string id;
    //-------------------------------------------------------------------------
    SharedMemoryObject shm;
    MappedRegion mapped_region;
public:
    //-------------------------------------------------------------------------
    /**
     * @return checksum of the whole shared memory.
     * @note stream has to be opened or created befores
     */
    size_t getMemoryChecksum();
    //-------------------------------------------------------------------------
    void lockToWrite();
    //-------------------------------------------------------------------------
    void lockToRead();
    //-------------------------------------------------------------------------
    void unlockWrite();
    //-------------------------------------------------------------------------
    void unlockRead();
    //-------------------------------------------------------------------------
    virtual ~Stream();
    //-------------------------------------------------------------------------
    static Ptr create(const std::string &id, size_t blockSize, size_t numChannels);
    //-------------------------------------------------------------------------
    static Ptr open(const std::string &id);
    //-------------------------------------------------------------------------
    template <typename T>
    void write(T **data);
    //-------------------------------------------------------------------------
    template <typename T>
    void read(T **out);
    //-------------------------------------------------------------------------
    void resize(size_t blockSize, size_t numChannels);
    //-------------------------------------------------------------------------
    double ** getBuffer() const;
    //-------------------------------------------------------------------------
    size_t getBlockSize() const {
        if (!blockSize_ist) {
            return 0;
        }
        return *blockSize_ist;
    }
    //-------------------------------------------------------------------------
    size_t getNumChannels() const {
        if (!numChannels_ist) {
            return 0;
        }
        return *numChannels_ist;
    }
    //-------------------------------------------------------------------------
    const std::string & getId() const { return id; }
}; // Stream
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
template <typename T>
void Stream::write(T **data) {
    lockToWrite();
    size_t nc = getNumChannels();
    size_t bs = getBlockSize();
    for (size_t i=0; i<nc; ++i) {
        for (size_t j=0; j<bs; ++j) {
            buffer[i][j] = (T)data[i][j];
        }
    }
    unlockWrite();
}
//-----------------------------------------------------------------------------
template <typename T>
void Stream::read(T **out) {
    lockToRead();
    size_t nc = getNumChannels();
    size_t bs = getBlockSize();
    for (size_t i=0; i<nc; ++i) {
        for (size_t j=0; j<bs; ++j) {
            out[i][j] = (T)buffer[i][j];
        }
    }
    unlockRead();
}

}}} // namespace(s)

#endif /* SAMBAG_STREAM_H */
