/*
 * Vst3MidiEvents.cpp
 *
 *  Created on: Mon Feb 16 21:10:23 2015
 *      Author: Johannes Unger
 */

#include "VST3ParameterChanges.hpp"
#include <boost/foreach.hpp>

namespace frx { namespace processing {
//=============================================================================
//  Class VST3ParamValueQueue
//=============================================================================
IMPLEMENT_FUNKNOWN_METHODS(VST3ParamValueQueue, Steinberg::Vst::IParamValueQueue, Steinberg::Vst::IParamValueQueue::iid)
VST3ParamValueQueue::ParamID PLUGIN_API VST3ParamValueQueue::getParameterId ()
{
    return _paramId;
}
VST3ParamValueQueue::int32 PLUGIN_API VST3ParamValueQueue::getPointCount ()
{
    return _points.size();
}
void VST3ParamValueQueue::clear()
{
    return _points.clear();
}
::Steinberg::tresult PLUGIN_API VST3ParamValueQueue::getPoint (int32 index, int32 &sampleOffset, ParamValue &value)
{
    if (index>=_points.size()) {
        return Steinberg::kResultFalse;
    }
    boost::tie(sampleOffset, value) = _points[index];
    return Steinberg::kResultOk;
}
::Steinberg::tresult PLUGIN_API VST3ParamValueQueue::addPoint (int32 sampleOffset, ParamValue value, int32 &index)
{
    for (int32 i = 0; i<_points.size(); ++i) {
        index = i;
        Point &p = _points[i];
        if (boost::get<0>(p) == sampleOffset) {
            boost::get<1>(p) = value;
        }
        return Steinberg::kResultOk;
    }
    index = _points.size();
    _points.push_back( Point(sampleOffset, value) );
    return Steinberg::kResultOk;
}
//=============================================================================
//  Class VST3ParameterChanges
//=============================================================================
IMPLEMENT_FUNKNOWN_METHODS(VST3ParameterChanges, Steinberg::Vst::IParameterChanges, Steinberg::Vst::IParameterChanges::iid)
VST3ParameterChanges::~VST3ParameterChanges()
{
    BOOST_FOREACH(Queues::value_type &x, _queues)
    {
        x->release();
        x = NULL;
    }
}
void VST3ParameterChanges::clear()
{
    BOOST_FOREACH(Queues::value_type &x, _queues)
    {
        x->clear();
    }
}
VST3ParameterChanges::int32 VST3ParameterChanges::getParameterCount ()
{
    return _queues.size();
}
VST3ParameterChanges::IParamValueQueue * PLUGIN_API VST3ParameterChanges::getParameterData (int32 index)
{
    if (index>=_queues.size()) {
        return NULL;
    }
    return _queues[index];
}
VST3ParameterChanges::IParamValueQueue * PLUGIN_API VST3ParameterChanges::addParameterData (const ParamID &id, int32 &index)
{
   for (int32 i = 0; i<_queues.size(); ++i) {
        index = i;
        VST3ParameterChanges::IParamValueQueue *q = _queues[i];
        if (q==NULL) {
            continue;
        }
        if (q->getParameterId()==id) {
            index = i;
            return q;
        }
    }
    index = _queues.size();
    _queues.push_back(new VST3ParamValueQueue(id));
    return _queues[index];
}
}} // namespace(s)
