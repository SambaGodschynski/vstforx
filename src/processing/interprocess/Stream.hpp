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
private:
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
    virtual ~Stream();
    //-------------------------------------------------------------------------
    static Ptr create(const std::string &id, size_t blockSize, size_t numChannels);
    //-------------------------------------------------------------------------
    static Ptr open(const std::string &id);
    //-------------------------------------------------------------------------
    void write(double **data);
    //-------------------------------------------------------------------------
    void read(double **data);
    //-------------------------------------------------------------------------
    void resize(size_t blockSize, size_t numChannels);
    //-------------------------------------------------------------------------
    double ** getBuffer() const { return buffer; }
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
}}} // namespace(s)

#endif /* SAMBAG_STREAM_H */
