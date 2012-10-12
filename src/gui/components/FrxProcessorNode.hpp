/*
 * FrxProcessorNode.hpp
 *
 *  Created on: Mon Aug 20 10:45:09 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXPROCESSORNODE_H
#define SAMBAG_FRXPROCESSORNODE_H

#include <boost/shared_ptr.hpp>
#include "FrxNode.hpp"
#include <vector>

namespace frx { namespace gui { namespace components {
namespace sdc = sambag::disco::components;
namespace sdcu = sdc::ui;
//=============================================================================
/** 
  * @class FrxProcessorNode.
  */
class FrxProcessorNode : public FrxNode {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef FrxNode Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxProcessorNode> Ptr;
	//-------------------------------------------------------------------------
	typedef std::vector<FrxNode::Ptr> IOContainer;
protected:
	//-------------------------------------------------------------------------
	IOContainer inputs;
	//-------------------------------------------------------------------------
	IOContainer outputs;
	//-------------------------------------------------------------------------
	FrxProcessorNode();
	//-------------------------------------------------------------------------
	FrxNode::Ptr addInputNode();
	//-------------------------------------------------------------------------
	FrxNode::Ptr addOutputNode();
private:
	///////////////////////////////////////////////////////////////////////////
	// Archive:
	//-------------------------------------------------------------------------
	friend class boost::serialization::access;
	//-------------------------------------------------------------------------
	template <typename Archive> 
	void serialize(Archive &ar, const unsigned int version) { 
		ar & boost::serialization::base_object<Super>(*this); 
	} 
public:
	//-------------------------------------------------------------------------
	const IOContainer & getInputs() const { return inputs; }
	//-------------------------------------------------------------------------
	const IOContainer & getOutputs() const { return outputs; }
	//-------------------------------------------------------------------------
	void resetIOLocation() const;
	//-------------------------------------------------------------------------
	/**
	 * adds and places io-nodes. Assumes that processor already added on 
	 * FrxCircuidView.
	 */
	void configIO(int numInputs, int numOutputs);
}; // FrxProcessorNode
}}} // namespace(s)

#endif /* SAMBAG_FRXPROCESSORNODE_H */
