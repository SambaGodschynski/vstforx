/*
 * IProcessor.hpp
 *
 *  Created on: Sat Oct  6 13:43:59 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_IPROCESSOR_H
#define SAMBAG_IPROCESSOR_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include "ModelObject.hpp"
#include "INode.hpp"
#include "IParameter.hpp"
#include <sambag/com/events/Events.hpp>
#include <sambag/com/events/PropertyChanged.hpp>
#include <vector>
#include <set>

namespace frx { namespace processing {
//=============================================================================
/** 
  * @class IProcessor.
  */
class IProcessor : public ModelObject {
//=============================================================================
private:
	///////////////////////////////////////////////////////////////////////////
	// Archive:
	//-------------------------------------------------------------------------
	friend class boost::serialization::access;
	//-------------------------------------------------------------------------
	template <typename Archive> 
	void serialize(Archive &ar, const unsigned int version) {
		ar & boost::serialization::base_object<ModelObject> ( *this );
	}
public:
	//-------------------------------------------------------------------------
	typedef ModelObject Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<IProcessor> Ptr;
	//-------------------------------------------------------------------------
	typedef boost::weak_ptr<IProcessor> WPtr;
	//-------------------------------------------------------------------------
	virtual Ptr getPtr() const = 0;
	//-------------------------------------------------------------------------
	virtual size_t getNumInputs() const = 0;
	//-------------------------------------------------------------------------
	virtual size_t getNumOutputs() const = 0;
	//-------------------------------------------------------------------------
	virtual INode::Ptr getInput(size_t nr) const = 0;
	//-------------------------------------------------------------------------
	virtual INode::Ptr getOutput(size_t nr) const = 0;
	//-------------------------------------------------------------------------
	/**
	 * @return true if processor is able to add/remove input
	 */
	virtual bool hasMultipleInputs() const = 0;
	//-------------------------------------------------------------------------
	/**
	 * @return true if processor is able to add/remove output
	 */
	virtual bool hasMultipleOutputs() const = 0;
	//-------------------------------------------------------------------------
	/**
	 * Creates output and adds to processor.
	 * @return created output or null when failed.
	 */
	virtual INode::Ptr addOutput() = 0;
	//-------------------------------------------------------------------------
	/**
	 * Creates input and adds to processor.
	 * @return created input or null when failed.
	 */
	virtual INode::Ptr addInput() = 0;
	//-------------------------------------------------------------------------
	/**
	 * @return the number of presets
	 */
	virtual size_t getNumPresets() const = 0;
	//-------------------------------------------------------------------------
	/**
	 * @return the preset name on index i.
	 */
	virtual std::string getPresetName(size_t i) const = 0;
	//-------------------------------------------------------------------------
	/**
	 * set the preset on index i.
	 */
	virtual void setPreset(int i) = 0;
	//-------------------------------------------------------------------------
	///////////////////////////////////////////////////////////////////////////
	// Events
	struct IOChangedEvent{
		Ptr src;
		IOChangedEvent(Ptr src) : src(src) {}
	};
	typedef boost::weak_ptr<void> AnyWPtr;
	typedef sambag::com::events::EventSender<IOChangedEvent> IOChangedEventSender;
	//-------------------------------------------------------------------------
	virtual IOChangedEventSender::Connection 
	addIOChangedListener(const IOChangedEventSender::EventFunction &) = 0;
	//-------------------------------------------------------------------------
	virtual IOChangedEventSender::Connection 
	addTrackedIOChangedListener(const IOChangedEventSender::EventFunction &, 
		AnyWPtr holder) = 0;
	//-------------------------------------------------------------------------
	typedef sambag::com::events::PropertyChanged PropertyChangedEvent;
	typedef sambag::com::events::EventSender<PropertyChangedEvent> PropertyChangedSender;
	virtual PropertyChangedSender::Connection
	addPropertyChangedListener(const PropertyChangedSender::EventFunction &) = 0;
	//-------------------------------------------------------------------------
	virtual PropertyChangedSender::Connection
	addTrackedPropertyChangedListener(const PropertyChangedSender::EventFunction &, 
		AnyWPtr holder) = 0;
}; // IProcessor
}} // namespace(s)

#endif /* SAMBAG_IPROCESSOR_H */
