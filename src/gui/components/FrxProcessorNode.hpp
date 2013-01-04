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
#include <gui/HandyNamespaces.hpp>

namespace frx { namespace gui { namespace components {
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
	typedef boost::weak_ptr<FrxProcessorNode> WPtr;
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
	FrxNode::Ptr createInputNode();
	//-------------------------------------------------------------------------
	FrxNode::Ptr createOutputNode();
	//-------------------------------------------------------------------------
	void addInputNodeToView(FrxCircuidViewPtr view, FrxNode::Ptr);
	//-------------------------------------------------------------------------
	void addOutputNodeToView(FrxCircuidViewPtr view, FrxNode::Ptr);
private:
	///////////////////////////////////////////////////////////////////////////
	// Archive:
	//-------------------------------------------------------------------------
	friend class boost::serialization::access;
	//-------------------------------------------------------------------------
	template <typename Archive> 
	void serialize(Archive &ar, const unsigned int version) { 
		ar & boost::serialization::base_object<Super>(*this); 
		ar & inputs;
		ar & outputs;
	} 
public:
	//-------------------------------------------------------------------------
	void addInputNode(FrxCircuidViewPtr view, FrxNode::Ptr);
	//-------------------------------------------------------------------------
	void addOutputNode(FrxCircuidViewPtr view, FrxNode::Ptr);
	//-------------------------------------------------------------------------
	const IOContainer & getInputs() const { return inputs; }
	//-------------------------------------------------------------------------
	const IOContainer & getOutputs() const { return outputs; }
	//-------------------------------------------------------------------------
	void resetIOLocation();
	//-------------------------------------------------------------------------
	void configIO(int numInputs, int numOutputs);
}; // FrxProcessorNode
}}} // namespace(s)

#endif /* SAMBAG_FRXPROCESSORNODE_H */
