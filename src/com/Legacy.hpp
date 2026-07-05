/*
 * Legacy.hpp
 *
 *  Created on: Wed Mar  5 14:11:09 2014
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_LEGACY_H
#define SAMBAG_LEGACY_H

#include <memory>

namespace com {
//=============================================================================
/** 
  * @class Legacy.
  * @brief interface for legacy classes. Offers a method to
  * update a legacy object into an appropriate new version.
  */
template <class _CommonBase,
    typename _CommonBasePtr = std::shared_ptr<_CommonBase>
>
class Legacy {
//=============================================================================
public:
    //-------------------------------------------------------------------------
    typedef _CommonBase CommonBase;
    typedef _CommonBasePtr CommonBasePtr;
    typedef Legacy<CommonBase, CommonBasePtr> ThisClass;
    //-------------------------------------------------------------------------
    typedef std::shared_ptr<ThisClass> Ptr;
    //-------------------------------------------------------------------------
    virtual ~Legacy() {}
    //-------------------------------------------------------------------------
    /**
     * @return a appropriate new version of this legacy object 
     */
    virtual CommonBasePtr updateLegacy() = 0;
}; // Legacy
} // namespace(s)

#endif /* SAMBAG_LEGACY_H */
