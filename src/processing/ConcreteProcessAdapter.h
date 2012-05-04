/*
 * ===========================================================================================================
 * ConcreteProcessAdapter.h
 *      Author: Johannes Unger
 * To add a new adapter-type do:
 *   - implement new processing::ProcessAdapter 
 *   - implement new ppiGui::GProcessorNode and its load_construct_data function
 *   - make a icon-rep. for new GProcessorNode and add it to Resources.h and PPIVst::loadResources
 *   - register new type for boost archive. see RegisterBoostTypes.h
 *   - register new GProcessorNode in FrontController
 *   - additional: implement a new Controller for new GProcessorNode type
 *   - implement create command
 *   - add command to menu
 * ===========================================================================================================
 */
#ifndef CONCRETE_PROCESS_ADAPTER
#define CONCRETE_PROCESS_ADAPTER

//============================================================================================================
//	#including farm
//============================================================================================================
#include "concreteAdapter/Volume.h"
#include "concreteAdapter/Pan.h"
#include "concreteAdapter/PeakTracker.h"
#include "concreteAdapter/ADSRTrigger.h"
#include "concreteAdapter/MidiProcessor.h"
#include "concreteAdapter/InputSwitch.h"
#include "concreteAdapter/InputStep.h"
#include "concreteAdapter/OutputSwitch.h"
#include "concreteAdapter/OutputStep.h"

#endif


