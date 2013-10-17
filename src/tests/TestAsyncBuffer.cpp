/*
 * TestAsyncBuffer.cpp
 *
 *  Created on: Mon Sep 23 10:44:11 2013
 *      Author: Johannes Unger
 */

#include "TestAsyncBuffer.hpp"
#include <cppunit/config/SourcePrefix.h>
#include <processing/AsyncBuffer.hpp>
#include <sambag/com/Interprocess.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( tests::TestAsyncBuffer );

using sambag::com::interprocess::Integer;
using sambag::com::interprocess::UInteger;

namespace {
    double ** createTestBuffer(UInteger blockSize, UInteger numChannels) {
        double **data = new double*[numChannels];
        for (UInteger i=0; i<numChannels; ++i) {
            data[i] = new double[blockSize];
        }
        return data;
    }
    void fillTestBuffer(UInteger blockSize, UInteger numChannels, double **data) {
        for (UInteger i=0; i<numChannels; ++i) {
            for (UInteger j=0; j<blockSize; ++j ) {
                data[i][j] = (double)(i+1)*j;
            }
        }
    }
    bool compare(UInteger blockSize, UInteger numChannels, double **a, double **b) {
        for (UInteger i=0; i<numChannels; ++i) {
            for (UInteger j=0; j<blockSize; ++j ) {
                //std::cout<<a[i][j]<<", "<<b[i][j]<<std::endl<<std::flush;
                if ( a[i][j] != b[i][j] ) {
                    return false;
                }
            }
        }
        return true;
    }
    template <typename T>
    std::string toString(UInteger blockSize, UInteger numChannels, const T &a)
    {
        std::stringstream ss;
        ss<<"{";
        for (UInteger i=0; i<numChannels; ++i) {
            ss<<"{";
            for (UInteger j=0; j<blockSize; ++j ) {
                ss<<a[i][j]<<", ";
            }
            ss<<"}, ";
        }
        ss<<"}"<<std::endl;
        return ss.str();
    }
    template <class Buffer>
    std::string toString(const Buffer &bff)
    {
        return toString(bff.getSize(), bff.getNumChannels(), bff);
    }

    void freeTestBuffer(double **data, UInteger numChannels) {
        for (UInteger i=0; i<numChannels; ++i) {
            delete[] data[i];
        }
        delete[] data;
    }
}

namespace tests {
//=============================================================================
//  Class TestAsyncBuffer
//=============================================================================
//-----------------------------------------------------------------------------
void TestAsyncBuffer::testAsyncBufferAlloc() {
    using namespace frx::processing;
    {
        AsyncBuffer<double> bff;
        bff.allocate(512);
        CPPUNIT_ASSERT_EQUAL((UInteger)512*2, bff.getSize());
    }
    {
        AsyncBuffer<double,3> bff;
        bff.allocate(512);
        CPPUNIT_ASSERT_EQUAL((UInteger)512*3, bff.getSize());
    }
    {
        // Sould occur compiler error:
        // AsyncBuffer<double, 1> bff;
    }
}
//-----------------------------------------------------------------------------
void TestAsyncBuffer::testAsyncBufferCyclicWriting() {
    using namespace frx::processing;
    AsyncBuffer<double> bff;
    bff.allocate(10);
    
      CPPUNIT_ASSERT_EQUAL(
    std::string("{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, }, {0\
, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, }, }\n"), toString(bff));
    
    
    double **data = createTestBuffer(30, 2);
    fillTestBuffer(30, 2, data);
    double *it[] = {data[0], data[1]};
    bff.writeBlock(&it[0]);
    CPPUNIT_ASSERT_EQUAL(
    std::string("{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, }, {0\
, 2, 4, 6, 8, 10, 12, 14, 16, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, }, }\n"), toString(bff));
    it[0]+=10;
    it[1]+=10;
    bff.writeBlock(&it[0]);
    CPPUNIT_ASSERT_EQUAL(
    std::string("{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,\
 19, }, {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36,\
 38, }, }\n"),
    toString(bff));
    it[0]+=10;
    it[1]+=10;
    bff.writeBlock(&it[0]);
    CPPUNIT_ASSERT_EQUAL(
    std::string("{{20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 10, 11, 12, 13, 14, 15, 1\
6, 17, 18, 19, }, {40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 20, 22, 24, 26, 28, \
30, 32, 34, 36, 38, }, }\n"),
    toString(bff));
    
    freeTestBuffer(data, 2);
}
//-----------------------------------------------------------------------------
void TestAsyncBuffer::testAsyncBufferCyclicWriting2() {
    using namespace frx::processing;
    AsyncBuffer<float, 3> bff; //btw. test conversion float/double
    bff.allocate(10);
    
      CPPUNIT_ASSERT_EQUAL(
    std::string("{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,\
 0, 0, 0, 0, 0, 0, 0, 0, }, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,\
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, }, }\n"), toString(bff));
    
    
    double **data = createTestBuffer(40, 2);
    fillTestBuffer(40, 2, data);
    double *it[] = {data[0], data[1]};
    bff.writeBlock(&it[0]);
    CPPUNIT_ASSERT_EQUAL(
    std::string("{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,\
 0, 0, 0, 0, 0, 0, 0, 0, }, {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 0, 0, 0, 0, 0, \
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, }, }\n"), toString(bff));
    it[0]+=10;
    it[1]+=10;
    bff.writeBlock(&it[0]);
    CPPUNIT_ASSERT_EQUAL(
    std::string("{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,\
 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, }, {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 2\
2, 24, 26, 28, 30, 32, 34, 36, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, }, }\n"),
    toString(bff));
    it[0]+=10;
    it[1]+=10;
    bff.writeBlock(&it[0]);
    CPPUNIT_ASSERT_EQUAL(
    std::string("{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,\
 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, }, {0, 2, 4, 6, 8, 10, 12, 14, 16,\
 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56\
, 58, }, }\n"),
    toString(bff));

    it[0]+=10;
    it[1]+=10;
    bff.writeBlock(&it[0]);
    CPPUNIT_ASSERT_EQUAL(
    std::string("{{30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 10, 11, 12, 13, 14, 15, 1\
6, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, }, {60, 62, 64, 66, 68, \
70, 72, 74, 76, 78, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48,\
 50, 52, 54, 56, 58, }, }\n"),
    toString(bff));
    

    
    freeTestBuffer(data, 2);
}
//-----------------------------------------------------------------------------
void TestAsyncBuffer::testMissingBlocks() {
    using namespace frx::processing;
    {
        typedef AsyncBuffer<double> Buffer;
        Buffer bff;
        bff.allocate(10);
        double **data = createTestBuffer(40, 2);
        CPPUNIT_ASSERT_EQUAL(1, bff.missingBlocks(0));
        CPPUNIT_ASSERT_EQUAL(12, bff.missingBlocks(11));
        bff.writeBlock(data); //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<write 1
        CPPUNIT_ASSERT_EQUAL(0, bff.missingBlocks(0));
        bff.writeBlock(data); //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<write 2
        CPPUNIT_ASSERT_EQUAL(0, bff.missingBlocks(0));
        bff.writeBlock(data); //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<write 3
        CPPUNIT_ASSERT_EQUAL(-1, bff.missingBlocks(0));
        bff.writeBlock(data); //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<write 4
        CPPUNIT_ASSERT_EQUAL(-2, bff.missingBlocks(0));
        freeTestBuffer(data, 2);
    }
    {
        typedef AsyncBuffer<double,3> Buffer;
        Buffer bff;
        bff.allocate(10);
        double **data = createTestBuffer(40, 2);
        CPPUNIT_ASSERT_EQUAL(1, bff.missingBlocks(0));
        CPPUNIT_ASSERT_EQUAL(12, bff.missingBlocks(11));
        bff.writeBlock(data); //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<write 1
        CPPUNIT_ASSERT_EQUAL(0, bff.missingBlocks(0));
        bff.writeBlock(data); //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<write 2
        CPPUNIT_ASSERT_EQUAL(0, bff.missingBlocks(0));
        bff.writeBlock(data); //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<write 3
        CPPUNIT_ASSERT_EQUAL(0, bff.missingBlocks(0));
        bff.writeBlock(data); //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<write 4
        CPPUNIT_ASSERT_EQUAL(-1, bff.missingBlocks(0));
        bff.writeBlock(data); //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<write 5
        CPPUNIT_ASSERT_EQUAL(-2, bff.missingBlocks(0));

        freeTestBuffer(data, 2);
    }
    {
        typedef AsyncBuffer<double,4> Buffer;
        Buffer bff;
        bff.allocate(10);
        double **data = createTestBuffer(40, 2);
        CPPUNIT_ASSERT_EQUAL(1, bff.missingBlocks(0));
        CPPUNIT_ASSERT_EQUAL(12, bff.missingBlocks(11));
        bff.writeBlock(data); //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<write 1
        CPPUNIT_ASSERT_EQUAL(0, bff.missingBlocks(0));
        bff.writeBlock(data); //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<write 2
        CPPUNIT_ASSERT_EQUAL(0, bff.missingBlocks(0));
        bff.writeBlock(data); //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<write 3
        CPPUNIT_ASSERT_EQUAL(0, bff.missingBlocks(0));
        bff.writeBlock(data); //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<write 4
        CPPUNIT_ASSERT_EQUAL(0, bff.missingBlocks(0));
        bff.writeBlock(data); //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<write 5
        CPPUNIT_ASSERT_EQUAL(-1, bff.missingBlocks(0));
        bff.writeBlock(data); //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<write 6
        CPPUNIT_ASSERT_EQUAL(-2, bff.missingBlocks(0));

        freeTestBuffer(data, 2);
    }
}
//-----------------------------------------------------------------------------
void TestAsyncBuffer::testReading() {
    using namespace frx::processing;
    typedef AsyncBuffer<double, 4> Buffer;
    Buffer bff;
    bff.allocate(10);
    
    double **data = createTestBuffer(50, 2);
    double **res = createTestBuffer(10, 29);
    fillTestBuffer(50, 2, data);
    double *it[] = {data[0], data[1]};
    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<1
    bff.writeBlock(&it[0]);
    UInteger blocksRead=Buffer::UndefinedNumBlocks;
    CPPUNIT_ASSERT_EQUAL(0, bff.readBlock(res, blocksRead));
    CPPUNIT_ASSERT_EQUAL((UInteger)1, blocksRead);
    CPPUNIT_ASSERT_EQUAL(
    std::string("{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, }, {0, 2, 4, 6, 8, 10, 12, 14, 16, \
18, }, }\n"),
    toString(10, 2, res));
    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<2
    it[0]+=10;
    it[1]+=10;
    bff.writeBlock(&it[0]);
    CPPUNIT_ASSERT_EQUAL(0, bff.readBlock(res, blocksRead));
    CPPUNIT_ASSERT_EQUAL((UInteger)2, blocksRead);
    CPPUNIT_ASSERT_EQUAL(
    std::string("{{10, 11, 12, 13, 14, 15, 16, 17, 18, 19, }, {20, 22, 24, 26, 28, 3\
0, 32, 34, 36, 38, }, }\n"),
    toString(10, 2, res));
    
    { //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< read again
        UInteger blocksRead = 1;
        CPPUNIT_ASSERT_EQUAL(0, bff.readBlock(res, blocksRead));
        CPPUNIT_ASSERT_EQUAL((UInteger)2, blocksRead);
        CPPUNIT_ASSERT_EQUAL(
        std::string("{{10, 11, 12, 13, 14, 15, 16, 17, 18, 19, }, {20, 22, 24, 26, 28, 3\
0, 32, 34, 36, 38, }, }\n"),
        toString(10, 2, res));

    }
    { //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< read older block again
        UInteger blocksRead = 0;
        CPPUNIT_ASSERT_EQUAL(0, bff.readBlock(res, blocksRead));
        CPPUNIT_ASSERT_EQUAL((UInteger)1, blocksRead);
        CPPUNIT_ASSERT_EQUAL(
        std::string("{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, }, {0, 2, 4, 6, 8, 10, 12, 14, 16, \
18, }, }\n"),
        toString(10, 2, res));

    }

    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<3
    it[0]+=10;
    it[1]+=10;
    bff.writeBlock(&it[0]);
    CPPUNIT_ASSERT_EQUAL(0, bff.readBlock(res, blocksRead));
    CPPUNIT_ASSERT_EQUAL((UInteger)3, blocksRead);
    CPPUNIT_ASSERT_EQUAL(
    std::string("{{20, 21, 22, 23, 24, 25, 26, 27, 28, 29, }, {40, 42, 44, 46, 48, 5\
0, 52, 54, 56, 58, }, }\n"),
    toString(10, 2, res));
    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<4
    it[0]+=10;
    it[1]+=10;
    bff.writeBlock(&it[0]);
    CPPUNIT_ASSERT_EQUAL(0, bff.readBlock(res, blocksRead));
    CPPUNIT_ASSERT_EQUAL((UInteger)4, blocksRead);
    CPPUNIT_ASSERT_EQUAL(
    std::string("{{30, 31, 32, 33, 34, 35, 36, 37, 38, 39, }, {60, 62, 64, 66, 68, 7\
0, 72, 74, 76, 78, }, }\n"),
    toString(10, 2, res));
    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<5
    it[0]+=10;
    it[1]+=10;
    bff.writeBlock(&it[0]);
    CPPUNIT_ASSERT_EQUAL(0, bff.readBlock(res, blocksRead));
    CPPUNIT_ASSERT_EQUAL((UInteger)5, blocksRead);
    CPPUNIT_ASSERT_EQUAL(
    std::string("{{40, 41, 42, 43, 44, 45, 46, 47, 48, 49, }, {80, 82, 84, 86, 88, 9\
0, 92, 94, 96, 98, }, }\n"),
    toString(10, 2, res));
    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<undefined blocks
    {
        UInteger blocksRead = Buffer::UndefinedNumBlocks;
        CPPUNIT_ASSERT_EQUAL(0, bff.readBlock(res, blocksRead));
        CPPUNIT_ASSERT_EQUAL((UInteger)5, blocksRead);
        CPPUNIT_ASSERT_EQUAL(
        std::string("{{40, 41, 42, 43, 44, 45, 46, 47, 48, 49, }, {80, 82, 84, 86, 88, 9\
0, 92, 94, 96, 98, }, }\n"),
        toString(10, 2, res));
    }
    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< read older blocks
    {
        UInteger blocksRead = 1;
        CPPUNIT_ASSERT_EQUAL(0, bff.readBlock(res, blocksRead));
        CPPUNIT_ASSERT_EQUAL((UInteger)2, blocksRead);
        CPPUNIT_ASSERT_EQUAL(
        std::string("{{10, 11, 12, 13, 14, 15, 16, 17, 18, 19, }, {20, 22, 24, 26, 28, 3\
0, 32, 34, 36, 38, }, }\n"),
        toString(10, 2, res));

    }
    { 
        UInteger blocksRead = 2;
        CPPUNIT_ASSERT_EQUAL(0, bff.readBlock(res, blocksRead));
        CPPUNIT_ASSERT_EQUAL((UInteger)3, blocksRead);
        CPPUNIT_ASSERT_EQUAL(
        std::string("{{20, 21, 22, 23, 24, 25, 26, 27, 28, 29, }, {40, 42, 44, 46, 48, 5\
0, 52, 54, 56, 58, }, }\n"),
        toString(10, 2, res));

    }
    { 
        UInteger blocksRead = 3;
        CPPUNIT_ASSERT_EQUAL(0, bff.readBlock(res, blocksRead));
        CPPUNIT_ASSERT_EQUAL((UInteger)4, blocksRead);
        CPPUNIT_ASSERT_EQUAL(
        std::string("{{30, 31, 32, 33, 34, 35, 36, 37, 38, 39, }, {60, 62, 64, 66, 68, 7\
0, 72, 74, 76, 78, }, }\n"),
        toString(10, 2, res));

    }
    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< try to access, and fail
    {
        UInteger blocksRead = 0;
        CPPUNIT_ASSERT_EQUAL(-1, bff.readBlock(res, blocksRead));
        CPPUNIT_ASSERT_EQUAL((UInteger)0, blocksRead);
    }
    {
        UInteger blocksRead = 5;
        CPPUNIT_ASSERT_EQUAL(1, bff.readBlock(res, blocksRead));
        CPPUNIT_ASSERT_EQUAL((UInteger)5, blocksRead);
    }

}


} //namespace
