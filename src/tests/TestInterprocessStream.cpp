/*
 * TestInterprocessStream.cpp
 *
 *  Created on: Thu Sep 12 12:19:08 2013
 *      Author: Johannes Unger
 */

#include "TestInterprocessStream.hpp"
#include <cppunit/config/SourcePrefix.h>
#include <processing/interprocess/Stream.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( tests::TestInterprocessStream );


namespace {
    
    double ** createTestBuffer(size_t blockSize, size_t numChannels) {
        double **data = new double*[numChannels];
        for (size_t i=0; i<numChannels; ++i) {
            data[i] = new double[blockSize];
        }
        return data;
    }

    void fillTestBuffer(size_t blockSize, size_t numChannels, double **data) {
        for (size_t i=0; i<numChannels; ++i) {
            for (size_t j=0; j<blockSize; ++j ) {
                data[i][j] = (double)(i+1)*j;
            }
        }
    }

    bool compare(size_t blockSize, size_t numChannels, double **a, double **b) {
        for (size_t i=0; i<numChannels; ++i) {
            for (size_t j=0; j<blockSize; ++j ) {
                //std::cout<<a[i][j]<<", "<<b[i][j]<<std::endl<<std::flush;
                if ( a[i][j] != b[i][j] ) {
                    return false;
                }
            }
        }
        return true;
    }
    
    std::string toString(size_t blockSize, size_t numChannels, double **a)
    {
        std::stringstream ss;
        ss<<"{";
        for (size_t i=0; i<numChannels; ++i) {
            ss<<"{";
            for (size_t j=0; j<blockSize; ++j ) {
                ss<<a[i][j]<<", ";
            }
            ss<<"}, ";
        }
        ss<<"}"<<std::endl;
        return ss.str();
    }
    
    void freeTestBuffer(double **data, size_t numChannels) {
        for (size_t i=0; i<numChannels; ++i) {
            delete[] data[i];
        }
        delete[] data;
    }

}


namespace tests {
//=============================================================================
//  Class TestInterprocessStream
//=============================================================================
//-----------------------------------------------------------------------------
void TestInterprocessStream::testStreamConstruction() {
    using namespace frx::processing::interprocess;
    Stream::Ptr stream = Stream::create("ts1", 512, 2);
    {
        Stream::Ptr stream2 = Stream::create("ts1", 512, 2);
    }
}
//-----------------------------------------------------------------------------
void TestInterprocessStream::testChecksum() {
    using namespace frx::processing::interprocess;
    static const size_t BLOCK_SIZE = 512,
                        NUM_CHANNELS = 2;
    
    Stream::Ptr stream = Stream::create("ts1", BLOCK_SIZE, NUM_CHANNELS);
    double **data = createTestBuffer(BLOCK_SIZE, NUM_CHANNELS);
    fillTestBuffer(BLOCK_SIZE, NUM_CHANNELS, data);
    stream->write(data);
    
    size_t sum1 = stream->getMemoryChecksum();
    size_t sum2 = stream->getMemoryChecksum();

    CPPUNIT_ASSERT( sum1 == sum2 );
    
    stream->getBuffer()[0][0] = 1;
    
    sum2 = stream->getMemoryChecksum();
    CPPUNIT_ASSERT( sum1 != sum2 );
    
    stream->read(data);
    
    sum1 = stream->getMemoryChecksum();
    CPPUNIT_ASSERT( sum1 == sum2 );
    
    freeTestBuffer(data, NUM_CHANNELS);
}

//-----------------------------------------------------------------------------
void TestInterprocessStream::testReadWrite() {
    using namespace frx::processing::interprocess;
    static const size_t BLOCK_SIZE = 512,
                        NUM_CHANNELS = 2;
    
    Stream::Ptr stream = Stream::create("ts1", BLOCK_SIZE, NUM_CHANNELS);
    CPPUNIT_ASSERT_EQUAL(BLOCK_SIZE, stream->getBlockSize());
    CPPUNIT_ASSERT_EQUAL(NUM_CHANNELS, stream->getNumChannels());
   
    double **data = createTestBuffer(BLOCK_SIZE, NUM_CHANNELS);
    fillTestBuffer(BLOCK_SIZE, NUM_CHANNELS, data);
    double **check = createTestBuffer(BLOCK_SIZE, NUM_CHANNELS);
    stream->write(data);
    std::cout<<"!"<<toString(10, 2, stream->getBuffer())<<std::endl;
    stream->read(check);
    CPPUNIT_ASSERT(compare(BLOCK_SIZE, NUM_CHANNELS, data, check));
    {
        Stream::Ptr stream2 = Stream::open("ts1");
        CPPUNIT_ASSERT_EQUAL(BLOCK_SIZE, stream2->getBlockSize());
        CPPUNIT_ASSERT_EQUAL(NUM_CHANNELS, stream2->getNumChannels());
    
        double **res = createTestBuffer(BLOCK_SIZE, NUM_CHANNELS);
        
        CPPUNIT_ASSERT(!compare(BLOCK_SIZE, NUM_CHANNELS, data, res));
        stream2->read(res);
        stream2->read(res);
        CPPUNIT_ASSERT(compare(BLOCK_SIZE, NUM_CHANNELS, data, res));
        
        freeTestBuffer(res, NUM_CHANNELS);
    }
    // TODO: chrash here: std::cout<<toString(10, 2, stream->getBuffer())<<std::endl;
    {
        Stream::Ptr stream2 = Stream::create("ts1", BLOCK_SIZE, NUM_CHANNELS);
        double **res = createTestBuffer(BLOCK_SIZE, NUM_CHANNELS);
        
        CPPUNIT_ASSERT(!compare(BLOCK_SIZE, NUM_CHANNELS, data, res));
        stream2->read(res);
        std::cout<<toString(10, 2, res)<<std::endl;
        CPPUNIT_ASSERT(compare(BLOCK_SIZE, NUM_CHANNELS, data, res));
        
        freeTestBuffer(res, NUM_CHANNELS);
    }
    
    // open empty stream
    CPPUNIT_ASSERT( !Stream::open("NULL-STREAM") );
    
    freeTestBuffer(check, NUM_CHANNELS);
    freeTestBuffer(data, NUM_CHANNELS);
}
} //namespace
