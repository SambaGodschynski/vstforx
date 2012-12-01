/*
 * ============================================================================
 * IMidiEventProcessor.h
 *      Author: Johannes Unger
 * ============================================================================
 */

#ifndef IVSTEVENTPROCESSOR_H_
#define IVSTEVENTPROCESSOR_H_

#include <sambag/dsp/IMidiEvents.hpp>

//=============================================================================
// Schnittstelle IMidiEventProcessor
//=============================================================================
class IMidiEventProcessor {
public:
	//-------------------------------------------------------------------------
	virtual void processEvents(sambag::dsp::IMidiEvents *events) = 0;
};

#endif