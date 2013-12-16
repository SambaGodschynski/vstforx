/*
 * BridgePluginDelegate.hpp
 *
 *  Created on: Fri Dec 13 13:46:37 2013
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_BRIDGEPLUGINDELEGATE_H
#define SAMBAG_BRIDGEPLUGINDELEGATE_H

#include <boost/shared_ptr.hpp>
#include <string>

namespace frx { namespace processing { namespace interprocess {
//=============================================================================
/** 
  * @class BridgePluginDelegate.
  */
class BridgePluginDelegate {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<BridgePluginDelegate> Ptr;
protected:
private:
public:
    //-------------------------------------------------------------------------
    /**
     * @brief loads plugin
     * @throws
     */
    static Ptr create(size_t blockSize, float sampleRate, const std::string &location);
    //-------------------------------------------------------------------------
    size_t getBlockSize() const;
    //-------------------------------------------------------------------------
    float getSampleRate() const;
    //-------------------------------------------------------------------------
    size_t getNumChannels() const;
    //-------------------------------------------------------------------------
    const std::string & getLocation() const;
}; // BridgePluginDelegate
}}} // namespace(s)

#endif /* SAMBAG_BRIDGEPLUGINDELEGATE_H */
