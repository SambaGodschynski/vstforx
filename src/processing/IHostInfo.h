/*
 * ============================================================================
 * IHostInfo.h
 *      Author: Johannes Unger
 * ============================================================================
 */

#ifndef IHOST_INFO_H
#define IHOST_INFO_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <sambag/com/events/Events.hpp>

namespace frx { namespace processing {
namespace sce = sambag::com::events;
//=============================================================================
// Event HostChanged
//=============================================================================
//-----------------------------------------------------------------------------
struct HostChanged {
};
//=============================================================================
// Event HostIOChanged
//=============================================================================
//-----------------------------------------------------------------------------
struct HostIOChanged {
};
//=============================================================================
// TimeInfo
//=============================================================================
//-----------------------------------------------------------------------------
struct TimeInfo {
	enum Filter {
		FrxTempo = 1,
		FrxPpqPos = 1<<1,
	};
	double tempo;
	double sampleRate; // always valid
	double ppqPos;
	bool transportIsPlaying;
};
//=============================================================================
// Schnittstelle IHostInfo
//=============================================================================
struct IHostInfo {
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<IHostInfo> Ptr;
	//-------------------------------------------------------------------------
	typedef boost::weak_ptr<IHostInfo> WPtr;
	//-------------------------------------------------------------------------
	template< typename Archive >
	void serialize ( Archive &ar, const unsigned int version ) {} 
	//-------------------------------------------------------------------------
	/**
	 * @return current sample rate
	 */
	virtual float getSampleRate() const = 0;
	//-------------------------------------------------------------------------
	/**
	 * @return current block size
	 */
	virtual int getBlockSize() const = 0;
	//-------------------------------------------------------------------------
	virtual TimeInfo * getHostTimeInfo (int filter) = 0;
	//-------------------------------------------------------------------------
	/**
	 * @return target dependent audio effect ptr. 
	 * In case of:
	 *             - Vst2x: AudioEffectX
	 * @note: avoid using this function.
	 */
	virtual void * getEffectPtr() = 0;
	//-------------------------------------------------------------------------
	/**
	 * @return target dependent  callback function ptr. 
	 * In case of:
	 *             - Vst2x: audioMasterCallback
	 * @note: avoid using this function.
	 */
	virtual void * getMasterCallback() = 0;
	//-------------------------------------------------------------------------
	virtual ~IHostInfo(){}
	//-------------------------------------------------------------------------
	/**
	 * something changed, update needed.
	 * @return value is given by vst-sdk but not documented. So I don't know what
	 *         the return value means.
	 */
	virtual bool ioChanged() = 0;
	///////////////////////////////////////////////////////////////////////////
	// Events
	//-------------------------------------------------------------------------
	typedef sce::EventSender<HostIOChanged>::EventFunction HostIOChangedFunction;
	//-------------------------------------------------------------------------
	typedef sce::EventSender<HostIOChanged>::Connection HostIOChangedConnection;
	//-------------------------------------------------------------------------
	typedef boost::weak_ptr<void> AnyWPtr;
	//-------------------------------------------------------------------------
	virtual HostIOChangedConnection 
	addHostChangedListener(const HostIOChangedFunction &f) = 0;
	//-------------------------------------------------------------------------
	virtual HostIOChangedConnection 
	addTrackedHostChangedListener(const HostIOChangedFunction &f, AnyWPtr wptr) = 0;
};
}} // namepsace processing
#endif