/*
 * PluginSession.cpp
 *
 *  Created on: Tue Dec  3 13:30:20 2013
 *      Author: Johannes Unger
 */

#include "PluginSession.hpp"
#include <sambag/com/exceptions/IllegalArgumentException.hpp>
#include <algorithm>
#include "SessionManager.hpp"
#include "BridgeSession.hpp"
#include <sambag/com/PlacementAlloc.hpp>
#include <gui/components/interprocess/WindowSession.hpp>
#include <sambag/disco/components/Timer.hpp>

namespace frx { namespace processing { namespace interprocess {
namespace {
    inline Integer _minmem(Integer blockSize, Integer numChannels) {
        return std::max(
            (Integer)((blockSize*numChannels*sizeof(PluginSessionHost::Float)) + 64),
            (Integer)256
        );
    }
} // namespace
//=============================================================================
//  Class PluginSessionHost
//=============================================================================
//-----------------------------------------------------------------------------
PluginSessionHost::PluginSessionHost(BridgePluginDelegate::Ptr dg)
    :  Session( "ps-"+SessionManager::createUniqueName(),
                ChannelSize(
                    _minmem(dg->getBlockSize(), dg->getNumInputChannels())  + OpcM::MaxArgmemSize,
                    _minmem(dg->getBlockSize(), dg->getNumOutputChannels()) + OpcM::MaxRetmemSize
                ), ChannelSize(
                    PluginSessionHost::OpcM::MaxArgmemSize,
                    PluginSessionHost::OpcM::MaxRetmemSize
                )
        ),
        delegate(dg)
{
    float sampleRate = dg->getSampleRate();
    size_t bs = dg->getBlockSize();
    SAMBAG_LOG_TRACE<<"PluginSession audio config("<<sampleRate<<", "<<bs<<")";
    if (sampleRate<=0 || bs<=0) {
        using sambag::com::exceptions::IllegalArgumentException;
        std::stringstream ss;
        ss<<"BridgeSession illegal audio setup: sr("<<sampleRate<<") ";
        ss<<"bs("<<bs<<")";
        SAMBAG_THROW(IllegalArgumentException,
        ss.str());
    }
    setPriority(High);

    dg->sce::EventSender<sce::PropertyChanged>::addEventListener(
        boost::bind(&PluginSessionHost::onPluginPropertyChanged, this, _1, _2)
    );

}
//-----------------------------------------------------------------------------
PluginSessionHost::Ptr PluginSessionHost::create(BridgePluginDelegate::Ptr dg,
    BridgeSession *host)
{
    if (!dg || !host) {
        using sambag::com::exceptions::IllegalArgumentException;
        SAMBAG_THROW(IllegalArgumentException, "NULL Ptr");
    }
    Ptr neu = Ptr( new PluginSessionHost(dg) );
    neu->host = host;
    return neu;
}
//-----------------------------------------------------------------------------
void PluginSessionHost::onPluginPropertyChanged(void*,
    const sce::PropertyChanged &ev)
{
    namespace newEvents=sambag::com::events;
    namespace oldEvents=::com::events;
    if (ev.getPropertyName() == "process delay") {
        // TODO
        return;
    }
    if (ev.getPropertyName() == "editor size") {
        frx::processing::APluginImpl::EditorSize _new;
        ev.getNewValue(_new);
        onPluginEditorResized(_new);
        return;
    }
}
//-----------------------------------------------------------------------------
void PluginSessionHost::onPluginEditorResized(const APluginImpl::EditorSize &val)
{
    SAMBAG_LOG_TRACE<<"PluginSessionHost";
    typedef SessionHost::Operations::OnEditorResized Op;
    Op::ArgPtr arg = static_cast<Op::ArgPtr>( getArgmem() );
    arg->val = val;
    waitForResult( SessionHost::OpcM::getOPC<Op>() );
}
//-----------------------------------------------------------------------------
sdsp::HostTimeInfo PluginSessionHost::getHostTimeInfo(int filter) {
    typedef SessionHost::Operations::GetTimeInfo Op;
    Op::ArgPtr arg = static_cast<Op::ArgPtr>( getArgmem() );
    arg->filter = (Integer)filter;
    Op::RetPtr ret = waitForResult<Op::RetPtr>(SessionHost::OpcM::getOPC<Op>());
    return ret->info;
}
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(PluginSessionHost, GetPluginInfo) {
    const ::processing::PluginInfo info = delegate->getPluginInfo();
    shm_cpypath(ret->location, info.location);
    shm_cpystr(ret->name, info.name);
    shm_cpystr(ret->vendor, info.vendor);
    ret->isSynth = info.isSynth;
    ret->uid = info.uid;
    ret->type = info.pluginType;
}
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(PluginSessionHost, Open) {
    delegate->getPluginImpl()->openPlugin();
}
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(PluginSessionHost, Close) {
    delegate->getPluginImpl()->closePlugin();
}
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(PluginSessionHost, TurnOn) {
    delegate->getPluginImpl()->turnOn();
}
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(PluginSessionHost, TurnOff) {
    delegate->getPluginImpl()->turnOff();
}
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(PluginSessionHost, GetNumInputChannels) {
    ret->num = delegate->getNumInputChannels();
}
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(PluginSessionHost, GetNumOutputChannels) {
    ret->num = delegate->getNumOutputChannels();
}
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(PluginSessionHost, GetParameterValues) {
    ::processing::parameter::Parameter::Ptr p =
        delegate->getParameters().at(arg->index);
    shm_cpystr(ret->name, p->getName());
    shm_cpystr(ret->label, p->getLabel());
    shm_cpystr(ret->display, p->getDisplay());
    ret->value = p->getValue();
}
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(PluginSessionHost, GetNumParameter) {
    ret->num = delegate->getParameters().size();
}
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(PluginSessionHost, SetParameterValue) {
    ::processing::parameter::Parameter::Ptr p =
        delegate->getParameters().at(arg->index);
    p->setValue(arg->value);
    shm_cpystr(ret->display, p->getDisplay());
}
//-----------------------------------------------------------------------------
namespace {
    template <typename Float>
    inline Float ** _assignMemory(sambag::com::interprocess::PointerIterator &pIt,
        int numChannels,
        int numSamples)
    {
        using namespace ::sambag::com::interprocess;
        typedef PlacementAlloc< Float* > Allocator;
        Allocator alloc(pIt);
        Float **res = new Float*[numChannels];
        for (int i=0; i<numChannels; ++i) {
             res[i] = typename Allocator::template rebind<Float>::other(alloc).allocate(numSamples);
        }
        return res;
    }
}
FRX_OP_CALLBACK_METHOD_IMPL(PluginSessionHost, Process) {
    // Shared Session Memory:
    // struct OP::Arg{}; <-argument memory
    // void * raw; <-reserved memory
    //   ...
    //   ...
    using namespace ::sambag::com::interprocess;
    PointerIterator arg_pIt(arg + 1, getProcessArgmemSize() - sizeof(Operations::Process::Arg));
    PointerIterator ret_pIt(ret + 1, getProcessRetmemSize() - sizeof(Operations::Process::Ret));
    
    Float  **ins = _assignMemory<Float>(arg_pIt,
        delegate->getNumInputChannels(), arg->numSamples);
    Float  **outs = _assignMemory<Float>(ret_pIt,
        delegate->getNumOutputChannels(), arg->numSamples);
   
   delegate->getPluginImpl()->processPlugin(ins, outs, arg->numSamples);
   
   delete [] ins;
   delete [] outs;
}
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(PluginSessionHost, GetEditorSessionId) {
    if (!delegate->getPluginImpl()->hasEditor()) {
        shm_cpystr(ret->id, "");
        return;
    }
    shm_cpystr(ret->id, delegate->getWindowSession()->getId());
}
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(PluginSessionHost, HasEditor) {
    ret->value = delegate->getPluginImpl()->hasEditor();
}
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(PluginSessionHost, OpenEditor) {
    delegate->openEditor();
}
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(PluginSessionHost, CloseEditor) {
    delegate->closeEditor();
}
//=============================================================================
//  Class PluginSessionClient
//=============================================================================
//-----------------------------------------------------------------------------
PluginSessionClient::PluginSessionClient(const std::string &id) : Session(id),
    tmpNumInputs(-1),
    tmpNumOutputs(-1)
{
}
//-----------------------------------------------------------------------------
PluginSessionClient::Ptr PluginSessionClient::create(const std::string &id) {
    Ptr res = Ptr( new PluginSessionClient(id) );
    res->self = res;
    return res;
}
//-----------------------------------------------------------------------------
void PluginSessionClient::updatePluginInfo (::processing::PluginInfo &inf) {
    typedef PluginSessionHost::Operations::GetPluginInfo Op;
    Op::RetPtr res = waitForResult<Op::RetPtr>( SessionHost::OpcM::getOPC<Op>());
    inf.location = res->location;
    inf.name = res->name;
    inf.vendor = res->vendor;
    inf.isSynth = res->isSynth;
    inf.uid = res->uid;
    inf.pluginType = res->type;
}
//-----------------------------------------------------------------------------
void PluginSessionClient::turnOff() {
    typedef SessionHost::Operations::TurnOff Op;
    waitForResult<Op::RetPtr>( SessionHost::OpcM::getOPC<Op>());
}
//-----------------------------------------------------------------------------
void PluginSessionClient::turnOn() {
    typedef SessionHost::Operations::TurnOn Op;
    waitForResult<Op::RetPtr>( SessionHost::OpcM::getOPC<Op>());
}
//-----------------------------------------------------------------------------
void PluginSessionClient::openPlugin() {
    typedef SessionHost::Operations::Open Op;
    waitForResult<Op::RetPtr>( SessionHost::OpcM::getOPC<Op>());
}
//-----------------------------------------------------------------------------
void PluginSessionClient::closePlugin() {
    typedef SessionHost::Operations::Close Op;
    waitForResult<Op::RetPtr>( SessionHost::OpcM::getOPC<Op>());
}
//-----------------------------------------------------------------------------
size_t PluginSessionClient::getNumInputChannels() {
    typedef SessionHost::Operations::GetNumInputChannels Op;
    Op::RetPtr res = waitForResult<Op::RetPtr>( SessionHost::OpcM::getOPC<Op>());
    tmpNumInputs = res->num;
    return res->num;
}
//-----------------------------------------------------------------------------
size_t PluginSessionClient::getNumOutputChannels() {
    typedef SessionHost::Operations::GetNumOutputChannels Op;
    Op::RetPtr res = waitForResult<Op::RetPtr>( SessionHost::OpcM::getOPC<Op>());
    tmpNumOutputs = res->num;
    return res->num;
}
//-----------------------------------------------------------------------------
int PluginSessionClient::getNumParameter()
{
    typedef SessionHost::Operations::GetNumParameter Op;
    Op::RetPtr res = waitForResult<Op::RetPtr>( SessionHost::OpcM::getOPC<Op>());
    return res->num;
}
//-----------------------------------------------------------------------------
void PluginSessionClient::getParameterValues(
    ::processing::parameter::Parameter::Ptr p, size_t index)
{
    typedef SessionHost::Operations::GetParameterValues Op;
    Op::ArgPtr args = static_cast<Op::ArgPtr>( getArgmem() );
    args->index = index;
    Op::RetPtr res = waitForResult<Op::RetPtr>( SessionHost::OpcM::getOPC<Op>());
    p->setName(res->name);
    p->setDisplay(res->display);
    p->setLabel(res->label);
    p->setValue(res->value);
}
//-----------------------------------------------------------------------------
void PluginSessionClient::setParameterValues(
    ::processing::parameter::Parameter::Ptr p, size_t index)
{
    typedef SessionHost::Operations::SetParameterValue Op;
    Op::ArgPtr args = static_cast<Op::ArgPtr>( getArgmem() );
    args->index = index;
    args->value = p->getValue();
    Op::RetPtr res = waitForResult<Op::RetPtr>( SessionHost::OpcM::getOPC<Op>());
    p->setDisplay(res->display);
}
//-----------------------------------------------------------------------------
void PluginSessionClient::process(SessionHost::Float **srcIns,
    SessionHost::Float **srcOuts, int numSamples)
{
    // Shared Session Memory:
    // struct OP::Arg{}; <-argument memory
    // void * raw; <-reserved memory
    //   ...
    //   ...
    if (tmpNumInputs<0 || tmpNumOutputs<0) {
        using sambag::com::exceptions::IllegalArgumentException;
        SAMBAG_THROW(IllegalArgumentException, "PluginSessionClient numIO not set.");
    }
    typedef SessionHost::Operations::Process Op;
    typedef SessionHost::Float Float;
    using namespace ::sambag::com::interprocess;
    Op::ArgPtr arg = static_cast<Op::ArgPtr>( getArgmem() );
    Op::RetPtr ret = static_cast<Op::RetPtr>( getRetmem() );
    
    PointerIterator arg_pIt(arg + 1, getRequestArgmemSize() - sizeof(Op::Arg));
    PointerIterator ret_pIt(ret + 1, getRequestRetmemSize() - sizeof(Op::Ret));
    
    Float **ins = _assignMemory<Float>(arg_pIt, tmpNumInputs, numSamples);
    Float **outs = _assignMemory<Float>(ret_pIt, tmpNumOutputs, numSamples);
   
    // copy into shared session memory
    for (int i=0; i<tmpNumInputs; ++i) {
        memcpy(ins[i], srcIns[i], numSamples*sizeof(Float));
    }
    // call process
    arg->numSamples = numSamples;
    waitForResult (SessionHost::OpcM::getOPC<Op>());
    
    // copy result into out memry
    for (int i=0; i<tmpNumOutputs; ++i) {
        memcpy(srcOuts[i], outs[i], numSamples*sizeof(Float));
    }
    
    delete [] ins;
    delete [] outs;
}
//-----------------------------------------------------------------------------
bool PluginSessionClient::hasEditor() {
    typedef SessionHost::Operations::HasEditor Op;
    Op::RetPtr res = waitForResult<Op::RetPtr>(SessionHost::OpcM::getOPC<Op>());
    return res->value;
}
//-----------------------------------------------------------------------------
std::string PluginSessionClient::getEditorSessionId() {
    typedef SessionHost::Operations::GetEditorSessionId Op;
    Op::RetPtr res = waitForResult<Op::RetPtr>(SessionHost::OpcM::getOPC<Op>());
    return res->id;
}

//-----------------------------------------------------------------------------
void PluginSessionClient::openEditor() {
    typedef SessionHost::Operations::OpenEditor Op;
    waitForResult(SessionHost::OpcM::getOPC<Op>());
}
//-----------------------------------------------------------------------------
void PluginSessionClient::closeEditor() {
    typedef SessionHost::Operations::CloseEditor Op;
    waitForResult(SessionHost::OpcM::getOPC<Op>());
}
//-----------------------------------------------------------------------------
void PluginSessionClient::setHostInfo(IHostInfo::Ptr hI) {
    hostInfo = hI;
}
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(PluginSessionClient, OnEditorResized) {
    
    sdc::Timer::Ptr tm = sdc::Timer::create(50);
    tm->sce::EventSender<sdc::TimerEvent>::addTrackedEventListener(
        boost::bind( &PluginSessionClient::doSendEvent<sce::PropertyChanged>,
            this,
            sce::PropertyChanged("editor size", APluginImpl::EditorSize(0,0), arg->val)
        ),
            self
    );
    tm->start();
    
}
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(PluginSessionClient, GetTimeInfo) {
    if (!hostInfo) {
        SAMBAG_LOG_WARN<<"PluginSessionClient: HostInfo=NULL.";
        return;
    }
    ret->info = *(hostInfo->getHostTimeInfo(arg->filter));
}
}}} // namespace(s)
