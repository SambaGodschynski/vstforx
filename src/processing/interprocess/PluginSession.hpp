/*
 * PluginSession.hpp
 *
 *  Created on: Tue Dec  3 13:30:20 2013
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_PLUGINSESSION_H
#define SAMBAG_PLUGINSESSION_H

#include <boost/shared_ptr.hpp>
#include "Session.hpp"
#include "ShmCom.hpp"

namespace frx { namespace processing { namespace interprocess {
//=============================================================================
/** 
  * @class PluginSession.
  */
class PluginSessionHost : public Session {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<PluginSessionHost> Ptr;
protected:
    //-------------------------------------------------------------------------
    struct Operations {
        struct Close {
            enum { OpCode = 0 };
            typedef struct Arg {} *ArgPtr;
            typedef struct Ret {} *RetPtr;
        };
        struct SetPluginLocation {
            enum { OpCode = 1 };
            typedef struct Arg { char *path; } *ArgPtr;
            typedef struct Ret {} *RetPtr;
        };
        struct GetPluginLocation {
            enum { OpCode = 2 };
            typedef struct Arg {} *ArgPtr;
            typedef struct Ret { char *path; } *RetPtr;
        };
    };
    //-------------------------------------------------------------------------
    PluginSessionHost(const std::string &id, Integer blockSize);
   //--------------------------------------------------------------------------
    PluginSessionHost(const std::string &id);
private:
public:
    //-------------------------------------------------------------------------
    /**
     * creates new session
     */
    static Ptr create(const std::string &id, Integer blockSize);
   //--------------------------------------------------------------------------
    /**
     * open exsiting session
     */
    static Ptr open(const std::string &id);

    
}; // PluginSession
}}} // namespace(s)

#endif /* SAMBAG_PLUGINSESSION_H */
