#ifndef _VST3PARAMVALUEQUEUE_H
#define _VST3PARAMVALUEQUEUE_H

#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "pluginterfaces/base/funknown.h"
#include <boost/unordered_map.hpp>
#include <boost/tuple/tuple.hpp>
#include <vector>

namespace frx { namespace processing {
//=============================================================================
class VST3ParamValueQueue : public Steinberg::Vst::IParamValueQueue
{
//=============================================================================
public:
    virtual ~VST3ParamValueQueue() {}
    typedef Steinberg::int32 int32;
    typedef Steinberg::Vst::ParamID ParamID;
    typedef Steinberg::Vst::ParamValue ParamValue;
    VST3ParamValueQueue(ParamID id) : __funknownRefCount(0), _paramId(id) {}
protected:
    VST3ParamValueQueue(const VST3ParamValueQueue&) : __funknownRefCount(0){}
    VST3ParamValueQueue & operator=(const VST3ParamValueQueue&) {return *this;}
    typedef boost::tuple<int32, ParamValue> Point;
    typedef std::vector<Point> Points;
    Points _points;
private:
    ParamID _paramId;
public:
    virtual ParamID PLUGIN_API getParameterId ();
    virtual int32 PLUGIN_API getPointCount ();
    void clear();
    virtual ::Steinberg::tresult PLUGIN_API getPoint (int32 index, int32 &sampleOffset, ParamValue &value);
    virtual ::Steinberg::tresult PLUGIN_API addPoint (int32 sampleOffset, ParamValue value, int32 &index);
    ///////////////////////////////////////////////////////////////////////////
    virtual ::Steinberg::tresult PLUGIN_API queryInterface (const ::Steinberg::TUID iid, void** obj);
	virtual ::Steinberg::uint32 PLUGIN_API addRef ();
	virtual ::Steinberg::uint32 PLUGIN_API release ();
protected :
	::Steinberg::int32 __funknownRefCount;
}; // Vst3MidiAdapter

//=============================================================================
class VST3ParameterChanges : public Steinberg::Vst::IParameterChanges
{
//=============================================================================
public:
    typedef Steinberg::int32 int32;
    typedef Steinberg::Vst::ParamID ParamID;
    typedef Steinberg::Vst::IParamValueQueue IParamValueQueue;
    VST3ParameterChanges() : __funknownRefCount(0) {}
    virtual ~VST3ParameterChanges();
protected:
    VST3ParameterChanges(const VST3ParameterChanges&) : __funknownRefCount(0){}
    VST3ParameterChanges & operator=(const VST3ParameterChanges&) {return *this;}
private:
    typedef std::vector<VST3ParamValueQueue*> Queues;
    Queues _queues;
public:
    void clear();
    virtual int32 PLUGIN_API getParameterCount ();
    virtual IParamValueQueue * PLUGIN_API getParameterData (int32 index);
    virtual IParamValueQueue * PLUGIN_API addParameterData (const ParamID &id, int32 &index);
    ///////////////////////////////////////////////////////////////////////////
    virtual ::Steinberg::tresult PLUGIN_API queryInterface (const ::Steinberg::TUID iid, void** obj);
	virtual ::Steinberg::uint32 PLUGIN_API addRef ();
	virtual ::Steinberg::uint32 PLUGIN_API release ();
protected :
	::Steinberg::int32 __funknownRefCount;
}; // Vst3MidiAdapter

}}

#endif
