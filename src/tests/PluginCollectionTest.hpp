
#ifndef PLUGIN_COLLECTION_TEST_H
#define PLUGIN_COLLECTION_TEST_H

#include <cppunit/extensions/HelperMacros.h>
#include "audioeffectx.h"
#include "com/PPIError.h"
#include "processing/processing.h"
#include "com/Settings.h"
#include "com/PluginCollection.h"
#include "com/Events.h"
#include "DummyFX.h"

namespace tests {
//=============================================================================
class PluginCollectionTest : public CPPUNIT_NS::TestFixture,
	public com::events::EventListener<com::ScanComplete>,
	public com::events::EventListener<com::OnLoadFile> {
//=============================================================================
	CPPUNIT_TEST_SUITE( PluginCollectionTest );
	CPPUNIT_TEST( testConstructor );
	CPPUNIT_TEST( testScan );
	CPPUNIT_TEST( testFastScan );
	CPPUNIT_TEST( testFolderIntegrity1 );
	CPPUNIT_TEST( testFolderIntegrity2 );
	CPPUNIT_TEST( testPortability );
	CPPUNIT_TEST( testMultipleDirectories );
	CPPUNIT_TEST_SUITE_END();
private:
	processing::DummyFX::Ptr dummyFX;
	processing::Graph::Ptr createGraph( int blockSize, float samplerate );
	com::Settings *settings;
	void eventHandler ( void *src, const com::OnLoadFile &ev );
	void eventHandler ( void *src, const com::ScanComplete &ev );
	int numHandlerCalled;
public:
	PluginCollectionTest();
	~PluginCollectionTest();
	void setUp(){}
	void tearDown(){}
	void testScan();
	void testFastScan();
	void testFolderIntegrity1();
	void testFolderIntegrity2();
	void testMultipleDirectories();
	void testPortability();
	void testConstructor();
};
} // namespace tests
#endif 
