/*
 * ===========================================================================================================
 * SystemCommand.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#include "SystemCommand.h"
#include "Settings.h"


namespace com{
//============================================================================================================
//	Klasse CmdUpdatePluginCollection:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void CmdUpdatePluginCollection::_execute() {
	// be sure that doset't called at twice
	boost::unique_lock<boost::timed_mutex> lock(dontCallAtTwice, boost::try_to_lock);
	if (!lock.owns_lock())
		return;
	pC->update( graph.get() );
}
} // com