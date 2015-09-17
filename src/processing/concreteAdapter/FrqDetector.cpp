/*
 * ============================================================================
 * FrqDetector.cpp
 *      Author: Johannes Unger
 * ============================================================================
 */
#include "FrqDetector.hpp"
#include <processing/ModelFactory.hpp>

namespace processing{
//=============================================================================
//FrqDetector
//=============================================================================
//-----------------------------------------------------------------------------
const int FrqDetector::WindowSize = 2048;
//-----------------------------------------------------------------------------
void FrqDetector::hostBaseConfigChanged() {
}
//-----------------------------------------------------------------------------
void FrqDetector::serialize ( ::com::iArchive &ar, const unsigned int version ){
	ar >> boost::serialization::base_object < ProcessAdapter > ( *this );
	ar >> frq;
}
//-----------------------------------------------------------------------------
void FrqDetector::serialize ( ::com::oArchive &ar, const unsigned int version ){
	ar << boost::serialization::base_object < ProcessAdapter > ( *this );
	ar << frq;
}
//-----------------------------------------------------------------------------
FrqDetector::FrqDetector( frx::processing::IHostInfo::Ptr hostInfo ) :
    ProcessAdapter(hostInfo), sampleCounter(0)
{
	setName ( "FrqDetector" );
	inputNodes[0]->setName  ( getName() + " InputNode");
	outputNodes[0]->setName ( getName() + " OutputNode");
	frq = Parameter::create();
	frq->setReadOnly(true);
	*frq = 0.0f;
	frq->setName ("Frq");
}
//-----------------------------------------------------------------------------
void FrqDetector::processAdapter( Processor::Int numSamples ) {
	Frames *frame = getInputNode(0)->popFrame();
	// outputnode
	outputNodes[0]->pushAndCopy(frame, numSamples);
}
//-----------------------------------------------------------------------------
FrqDetector::~FrqDetector () { 
}
}// namespace processing
