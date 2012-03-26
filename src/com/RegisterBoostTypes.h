/*
 * ===========================================================================================================
 * RegisterBoostTypes.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#ifndef _REGISTER_BOOST_TYPES
#define _REGISTER_BOOST_TYPES

#include "processing/processing.h"
#include "processing/parameter/parameter.h"
#include "processing/parameter/ConnectionOperators.h"
#include "processing/ConcreteProcessAdapter.h"
#include "gui/ConcreteGObjects.h"
#include "processing/dspTools.h"
//---------------------------------------------------------------------------------------
template < typename A >
void register_types( A &ar ){
	//graph
	ar.template register_type<parameter::Parameter>();
	ar.template register_type<parameter::InverseConnection>();
	ar.template register_type<parameter::ExpConnection>();
	ar.template register_type<parameter::LogConnection>();
	ar.template register_type<parameter::OffsetConnection>();
	ar.template register_type<NOPNode>();
	ar.template register_type<ProcessAdapter::OutputNode>();
	ar.template register_type<ProcessAdapter::InputNode>();
	ar.template register_type<StartNode>();
	ar.template register_type<EndNode>();
	ar.template register_type<ProcessAdapterNode>();
	ar.template register_type<Volume>();
	ar.template register_type<VSTPlugin>();
	ar.template register_type<Pan>();
	ar.template register_type<OutputStep>();
	ar.template register_type<InputStep>();
	ar.template register_type<OutputSwitch>();
	ar.template register_type<InputSwitch>();
	ar.template register_type<PeakTracker>();
	ar.template register_type<ADSRTrigger>();
	ar.template register_type <MidiProcessor>();
	ar.template register_type <FadeValue>();
	ar.template register_type <LuaProcessor>();
	//view
	ar.template register_type<ppiGui::GStdKnob>();
	ar.template register_type<ppiGui::GPassiveKnob>();
	ar.template register_type<ppiGui::GConnectionPaPa>();
	ar.template register_type<ppiGui::GConnectionIO>();
	ar.template register_type<ppiGui::GConnectionPrIn>();
	ar.template register_type<ppiGui::GConnectionPrOut>();
	ar.template register_type<ppiGui::GConnectionPrPa>();
	ar.template register_type<ppiGui::GConnectionCoPa>();
	ar.template register_type<ppiGui::GVolumeNode>();
	ar.template register_type<ppiGui::GVSTPlugNode>();
	ar.template register_type<ppiGui::GPanAdapter>();
	ar.template register_type<ppiGui::GPeakTracker>();
	ar.template register_type<ppiGui::GADSRTrigger>();
	ar.template register_type<ppiGui::GMidiProcessor>();
	ar.template register_type<ppiGui::GOutputStepNode>();
	ar.template register_type<ppiGui::GInputStepNode>();
	ar.template register_type<ppiGui::GOutputSwitch>();
	ar.template register_type<ppiGui::GInputSwitch>();
	ar.template register_type<ppiGui::GInputNode>();
	ar.template register_type<ppiGui::GOutputNode>();
	ar.template register_type<ppiGui::GLuaProcessor>();
}
#endif