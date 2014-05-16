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
    /**
     * @return directory string
     */
    static std::string selectFile(const std::string &wndTitle,
                                       const std::string &startPath="");
    //-------------------------------------------------------------------------
    /**
     * @return directory string
     */
    static std::string saveFile(const std::string &wndTitle,
                                       const std::string &startPath="");
    //-------------------------------------------------------------------------
    static void startProcess(const char *path, int argc, const char **argv);
    //-------------------------------------------------------------------------
    static void openLink(const std::string &url);
    //-------------------------------------------------------------------------
    static void showInputTextDlg(const std::string &title, std::string &inOutTxt);

}; // CocoaHelper
}} // namespace(s)

#endif /* SAMBAG_COCOAIMPL_H */
