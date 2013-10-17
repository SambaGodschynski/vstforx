#include "FrxPlugSettings.hpp"
#include <iostream>
#include <sstream>
#include <com/FrxConfig.h>
#include <sambag/com/Config.h>
#include <stdexcept>

namespace {
  template <int Int>
  struct Int2Type {
    enum { Value=Int };
  };
}


bool globFrxIsDemo() {
  return (int)PlugSettings::Version == (int)PlugSettings::Demo;
}


//------------------------------------------------------------------------------
namespace {
std::ostream & operator<<(std::ostream &os, Int2Type<PlugSettings::Normal>) {
  return os;
}
std::ostream & operator<<(std::ostream &os, Int2Type<PlugSettings::Demo>) {
  os<<"DEMO";
  return os;
}
std::ostream & operator<<(std::ostream &os, Int2Type<PlugSettings::CM>) {
  os<<"CM";
  return os;
}
} // namespace
std::string globVersionStr() {
  std::stringstream ss;
  ss<<FRX_VERSION_MAJOR<<"."<<FRX_VERSION_MINOR<<"."<<FRX_VERSION_MICRO;
  ss<<" "<<Int2Type<PlugSettings::Version>();
#ifdef SAMBAG_64
  ss<<" - "<<"x86_64";
#endif
  ss<<" - "<<FRX_VERSION_BUILD<<":"<<SAMBAG_VERSION_BUILD<<" ";
  return ss.str();
}

//------------------------------------------------------------------------------
namespace {
typedef boost::shared_ptr<void> AnyPtr;
typedef boost::weak_ptr<void> AnyWPtr;
template <int Max>
void checkConstraints(AnyPtr object) {
  static const int numMax = Max;
  static AnyWPtr slots[numMax];
  // check for free slot
  for (int i=0; i<numMax; ++i) {
    AnyPtr p = slots[i].lock();
    if (p) {
      continue;
    }
    slots[i] = object; // free slot found
    return;
  }
  std::stringstream ss;
  ss<<"LIMITATION: you can't add more than "<<numMax<<" modules per session.";
  throw(std::runtime_error(ss.str()));
}
template <>
void checkConstraints<0>(AnyPtr object) {}

void addProcessor(boost::shared_ptr<void> obj, Int2Type<PlugSettings::Normal>) 
{
}
void addProcessor(boost::shared_ptr<void> obj, Int2Type<PlugSettings::Demo>) 
{
  checkConstraints<FRX_MAX_DEMO_MODULES>(obj);
}
void addProcessor(boost::shared_ptr<void> obj, Int2Type<PlugSettings::CM>) 
{
  checkConstraints<FRX_MAX_CM_MODULES>(obj);
}
void addPlugin(boost::shared_ptr<void> obj, Int2Type<PlugSettings::Normal>) 
{
}
void addPlugin(boost::shared_ptr<void> obj, Int2Type<PlugSettings::Demo>) 
{
  checkConstraints<FRX_MAX_DEMO_MODULES>(obj);
}
void addPlugin(boost::shared_ptr<void> obj, Int2Type<PlugSettings::CM>) 
{
}

} // namespace

void globAddProcessor( boost::shared_ptr<void> obj ) {
  addProcessor( obj, Int2Type<PlugSettings::Version>() );
}
void globAddPlugin( boost::shared_ptr<void> obj ) {
  addPlugin( obj, Int2Type<PlugSettings::Version>() );
}

namespace {
    const char * getLogoPath(Int2Type<PlugSettings::Normal>) {
        return "";
    }
    const char * getLogoPath(Int2Type<PlugSettings::Demo>) {
        return "";
    }
    const char * getLogoPath(Int2Type<PlugSettings::CM>) {
        return "images/cmlogo.png";
    }
}
const char * globGetLogoPath() {
    return getLogoPath(Int2Type<PlugSettings::Version>());
}


namespace {
    const char * getProductName(Int2Type<PlugSettings::Normal>) {
        return "VSTForx";
    }
    const char * getProductName(Int2Type<PlugSettings::Demo>) {
        return "VSTForx-Demo";
    }
    const char * getProductName(Int2Type<PlugSettings::CM>) {
        return "VSTForx-CM";
    }
}
const char * globGetProductName() {
    return getProductName(Int2Type<PlugSettings::Version>());
}



