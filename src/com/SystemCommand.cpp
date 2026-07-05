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
	std::unique_lock<std::timed_mutex> lock(dontCallAtTwice, std::try_to_lock);
	if (!lock.owns_lock())
		return;
	pC->update( graph.get() );
}
} // com