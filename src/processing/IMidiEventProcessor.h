/*
 * ============================================================================
 * IMidiEventProcessor.h
 *      Author: Johannes Unger
 * ============================================================================
 */

#ifndef IVSTEVENTPROCESSOR_H_
#define IVSTEVENTPROCESSOR_H_

#include <sambag/dsp/IMidiEvents.hpp>
#include <sambag/com/events/Events.hpp>
#include <boost/weak_ptr.hpp>

namespace processing {
//=============================================================================
/**
 * @class IMidiEventProcessor
 * @brief a MidiEventProcessor is able to recive and send MidiEvents.
 */
class IMidiEventProcessor {
//=============================================================================
public:
    //-------------------------------------------------------------------------
    typedef sambag::com::events::EventSender<sambag::dsp::IMidiEvents::Ptr>
        EventSender;
    typedef EventSender::EventFunction EventFunction;
    typedef EventSender::Connection Connection;
    typedef boost::weak_ptr<void> AnyWPtr;
	//-------------------------------------------------------------------------
    /**
     * @brief processes the midi event
     */
	virtual void processEvents(sambag::dsp::IMidiEvents::Ptr events) = 0;
    //-------------------------------------------------------------------------
    /**
     * @brief adds a listener for outgoing midi signals
     */
    virtual Connection addListener(const EventFunction &f) = 0;
    //-------------------------------------------------------------------------
    /**
     * @brief adds a tracked (@see boost::Signals2) 
     * listener for outgoing midi signals
     */
    virtual Connection addTrackedListener(const EventFunction &f,
        AnyWPtr trackingObject) = 0;

};

} // namespace(s)
#endif