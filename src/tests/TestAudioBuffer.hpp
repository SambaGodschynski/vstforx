/*
 * TestAudioBuffer.hpp
 *
 *  Created on: Tue Nov 12 10:32:22 2013
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_TESTAUDIOBUFFER_H
#define SAMBAG_TESTAUDIOBUFFER_H

#include <cppunit/extensions/HelperMacros.h>

namespace tests {
//=============================================================================
class TestAudioBuffer : public CPPUNIT_NS::TestFixture {
//=============================================================================
private:
	CPPUNIT_TEST_SUITE( TestAudioBuffer );
	CPPUNIT_TEST( testReadWrite );
	CPPUNIT_TEST_SUITE_END();
public:
	void testReadWrite();
};

} // namespace

#endif /* SAMBAG_TESTAUDIOBUFFER_H */
