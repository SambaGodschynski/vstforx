/**
 * PlugSettings.hpp
 */
#include <boost/weak_ptr.hpp>

#ifndef _PLUG_SETTINGS_HPP
#define _PLUG_SETTINGS_HPP

#include <string>

struct PlugSettings 
{

#ifdef FRX_IS_INSTRUMENT
	enum { IsInstrument = 1 };
#else
	enum { IsInstrument = 0 };
#endif

enum VersionTypes { Normal, Demo, CM };

#if defined FRX_IS_DEMO && defined FRX_IS_CM
  #error Demo and CM defined
#endif

#if defined FRX_IS_DEMO || defined FRX_IS_CM
    #ifdef FRX_IS_DEMO
        enum { Version = Demo };
    #else
        enum { Version = CM };
    #endif
#else 
  enum {Version=Normal};
#endif

enum { FRX_UID = '_frx' + (IsInstrument*2) + (Version*3) };

};

bool globFrxIsDemo();
std::string globVersionStr();

void globAddProcessor( boost::shared_ptr<void> obj );
void globAddPlugin( boost::shared_ptr<void> obj );

#endif
