/*
 * CocoaImpl.hpp
 *
 *  Created on: Thu May 8 13:33:51 2013
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_COCOAIMPL_H
#define SAMBAG_COCOAIMPL_H

#include <string>

namespace frx { namespace com { 
//=============================================================================
/** 
  * @class CocoaHelper.
  */
class CocoaImpl {
//=============================================================================
public:
    //-------------------------------------------------------------------------
    /**
     * @return directory string
     */
    static std::string selectDirectory(const std::string &wndTitle,
                                       const std::string &startPath="");
    //-------------------------------------------------------------------------
    static void startProcess(const char *path, int argc, const char **argv);
}; // CocoaHelper
}} // namespace(s)

#endif /* SAMBAG_COCOAIMPL_H */
