/*
 * Stream.hpp
 *
 *  Created on: Thu Sep 12 12:18:46 2013
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_INTERPROCESS_STREAM_H
#define SAMBAG_INTERPROCESS_STREAM_H

#include <boost/shared_ptr.hpp>
#include <string>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/interprocess_upgradable_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/sharable_lock.hpp>
#include <boost/interprocess/offset_ptr.hpp>
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
    //-------------------------------------------------------------------------
    typedef float ValueType;
    //-------------------------------------------------------------------------
    static const size_t UndefinedNumBlocks = UINT_MAX;
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
    size_t *blockSize_ist, *numChannels_ist, *blocksWritten;
    //-------------------------------------------------------------------------
    int *num_references;
    //-------------------------------------------------------------------------
    typedef boost::interprocess::offset_ptr<ValueType> ValuePtr;
    //-------------------------------------------------------------------------
    ValuePtr *buffer;
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
    virtual ~Stream();
    //-------------------------------------------------------------------------
    static Ptr create(const std::string &id, size_t blockSize, size_t numChannels);
    //-------------------------------------------------------------------------
    static Ptr open(const std::string &id);
    //-------------------------------------------------------------------------
    template <typename T>
    void write(T **data);
    //-------------------------------------------------------------------------
    /**
     * @param the allocated out container
     * @param the number of the already read blocks, increments value when
     *        reading was successfull
     * @return 0 when reading was successfull, otherwise the number of 
     *         missing blocks. (blocksWritten-blocksRead)+1
     */
    template <typename T>
    int read(T **out, size_t &inoutReadBlocks);
    //-------------------------------------------------------------------------
    void resize(size_t blockSize, size_t numChannels);
    //-------------------------------------------------------------------------
    ValueType * operator[](size_t channel) const;
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
    using namespace boost::interprocess;
    scoped_lock<Mutex> lock(*mutex);
    size_t nc = getNumChannels();
    size_t bs = getBlockSize();
    for (size_t i=0; i<nc; ++i) {
        for (size_t j=0; j<bs; ++j) {
            buffer[i][j] = (T)data[i][j];
        }
    }
    ++(*blocksWritten);
}
//-----------------------------------------------------------------------------
namespace {
    inline int missingBlocks(size_t wr, size_t rd) {
        return rd-wr+1;
    }
}
template <typename T>
int Stream::read(T **out, size_t &blocksRead) {
    using namespace boost::interprocess;
    if (blocksRead==UndefinedNumBlocks) {
        if ((*blocksWritten)>0) {
            // set blocksread
            blocksRead=(*blocksWritten)-1;
        } else {
            return 1;
        }
    }
    if (missingBlocks(*blocksWritten, blocksRead)!=0) {
        // out of sync
        return missingBlocks(*blocksWritten, blocksRead);
    }
    sharable_lock<Mutex> lock(*mutex);
    size_t nc = getNumChannels();
    size_t bs = getBlockSize();
    for (size_t i=0; i<nc; ++i) {
        for (size_t j=0; j<bs; ++j) {
            out[i][j] = (T)buffer[i][j];
        }
    }
    ++blocksRead;
    return 0;
}

}}} // namespace(s)

#endif /* SAMBAG_INTERPROCESS_STREAM_H */
