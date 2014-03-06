/*
 * IAdapter.hpp
 *
 *  Created on: Thu Mar  6 10:52:49 2014
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_IADAPTER_H
#define SAMBAG_IADAPTER_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <loki/NullType.h>

namespace com {
//=============================================================================
/** 
  * @interface IAdapter.
  */
template <
    typename _AdapteeType,
    typename _AdapteeTypePtr = boost::shared_ptr<_AdapteeType>
>
class IAdapter {
//=============================================================================
public:
    typedef _AdapteeType AdapteeType;
    typedef _AdapteeTypePtr AdapteeTypePtr;
    typedef IAdapter<AdapteeType, AdapteeTypePtr> ThisClass;
    typedef boost::shared_ptr<ThisClass> Ptr;
    typedef boost::weak_ptr<ThisClass> WPtr;
    virtual AdapteeTypePtr getAdaptee() const = 0;
    virtual void setAdaptee(AdapteeTypePtr) = 0;
}; // IAdapter


} // namespace(s)

#endif /* SAMBAG_IADAPTER_H */
