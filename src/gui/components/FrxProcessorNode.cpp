/*
 * FrxProcessorNode.cpp
 *
 *  Created on: Mon Aug 20 10:45:09 2012
 *      Author: Johannes Unger
 */

#include "FrxProcessorNode.hpp"
#include <sambag/disco/components/ui/ALookAndFeel.hpp>
#include "FrxConcreteConnections.hpp"
#include "FrxConcreteIO.hpp"
#include "FrxCircuidView.hpp"
#include <sambag/com/Exception.hpp>
#include <sambag/com/exceptions/IllegalStateException.hpp>
#include <sstream>

namespace frx { namespace gui { namespace components {
//=============================================================================
//  Class FrxProcessorNode
//=============================================================================
//-----------------------------------------------------------------------------
FrxProcessorNode::FrxProcessorNode() {
	setName("FrxProcessorNode");
}
//-----------------------------------------------------------------------------
FrxNode::Ptr FrxProcessorNode::addInputNode() {
	FrxInputNode::Ptr in = FrxInputNode::create();
	std::stringstream ss;
	ss<<getName()<<"_input_"<<inputs.size()+1;
	in->setName(ss.str());
	inputs.push_back(in);
	return in;
}
//-----------------------------------------------------------------------------
FrxNode::Ptr FrxProcessorNode::addOutputNode() {
	FrxOutputNode::Ptr out = FrxOutputNode::create();
	std::stringstream ss;
	ss<<getName()<<"_output_"<<outputs.size()+1;
	out->setName(ss.str());
	outputs.push_back(out);
	return out;
}
//-----------------------------------------------------------------------------
void FrxProcessorNode::resetIOLocation() const {
	namespace geom = boost::geometry;
	namespace trans = boost::geometry::strategy::transform;
	typedef trans::rotate_transformer<sd::Point2D, sd::Point2D, geom::degree>
		Rotate;
	// ### Input 
	sd::Point2D origin = getLocation();
	geom::add_point(origin, getPivot());
	int numInputs = (int)inputs.size();
	int numOutputs = (int)outputs.size();
	float d = 100.0/(float)numInputs;
	const float gap = 45.;
	int numRotate = 1, i=0;
	for (; i<numInputs/2; ++i ) { // spread left
		sd::Point2D p(0, 0);
		Rotate rotate((numRotate++)*-d);
		geom::transform(sd::Point2D(0, -gap), p, rotate);
		geom::add_point(p, origin);
		geom::subtract_point(p, inputs[i]->getPivot());
		inputs[i]->setLocation(p);
	}
	if (numInputs%2==1) {
		sd::Point2D p(origin.x(), origin.y() - gap);
		geom::subtract_point(p, inputs[i]->getPivot());
		inputs[i++]->setLocation(p); // origin
	}
	numRotate = 1;
	for (; i<numInputs; i++ ) { // spread right
		sd::Point2D p(0, 0);
		Rotate rotate((numRotate++)*d);
		geom::transform(sd::Point2D(0, -gap), p, rotate);
		geom::add_point(p, origin);
		geom::subtract_point(p, inputs[i]->getPivot());
		inputs[i]->setLocation(p);
	}
	// ### Output
	d = 100.0/(float)numOutputs;
	numRotate = 1;
	for (i=0; i<numOutputs/2; ++i ) { // spread right
		sd::Point2D p(0, 0);
		Rotate rotate((numRotate++)*d);
		geom::transform(sd::Point2D(0, +gap), p, rotate);
		geom::add_point(p, origin);
		geom::subtract_point(p, outputs[i]->getPivot());
		outputs[i]->setLocation(p);
	}
	if (numOutputs%2==1) {
		sd::Point2D p(origin.x(), origin.y() + gap);
		geom::subtract_point(p, outputs[i]->getPivot());
		outputs[i++]->setLocation(p); // origin
	}
	numRotate = 1;
	for (; i<numOutputs; ++i ) { // spread left
		sd::Point2D p(0, 0);
		Rotate rotate((numRotate++)*-d);
		geom::transform(sd::Point2D(0, +gap), p, rotate);
		geom::add_point(p, origin);
		geom::subtract_point(p, outputs[i]->getPivot());
		outputs[i]->setLocation(p);
	}
}
//-----------------------------------------------------------------------------
void FrxProcessorNode::configIO(int numInputs, int numOutputs) {
	FrxCircuidView::Ptr circ = getFirstContainer<FrxCircuidView>();
	if (!circ)
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException,
		"ProcessorNode has no circview anchestor.");
	inputs.reserve(numInputs);
	outputs.reserve(numOutputs);
	for (int i = 0; i<numInputs; ++i) {
		FrxNode::Ptr in = addInputNode();
		ProcessorInputCn::Ptr inc = ProcessorInputCn::create();
		inc->setComponentA(in);
		inc->setComponentB(getPtr());
		circ->add(in, FrxCircuidView::Z_IO);
		circ->add(inc, FrxCircuidView::Z_Wires);
	}
	for (int i = 0; i<numOutputs; ++i) {
		FrxNode::Ptr out = addOutputNode();
		ProcessorOutputCn::Ptr outc = ProcessorOutputCn::create();
		outc->setComponentA(getPtr());
		outc->setComponentB(out);
		circ->add(out, FrxCircuidView::Z_IO);
		circ->add(outc, FrxCircuidView::Z_Wires);
	}
	resetIOLocation();

}
}}} // namespace(s)
