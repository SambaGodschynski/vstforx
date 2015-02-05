/*
 * PluginSession.cpp
 *
 *  Created on: Tue Dec  3 13:30:20 2013
 *      Author: Johannes Unger
 */

#include <gui/components/interprocess/WindowSession.hpp>
#include "PluginSession.hpp"
#include <sambag/com/exceptions/IllegalArgumentException.hpp>
#include <algorithm>
#include "SessionManager.hpp"
#include "BridgeSession.hpp"
#include <sambag/com/PlacementAlloc.hpp>
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
    neu->startProcessThread();
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
    MemoryGuard::Ptr g = getMemoryGuard();
    Op::ArgPtr args = g->getArg<Op::Arg>();
    args->val = val;
    waitForProcess( SessionHost::OpcM::getOPC<Op>(), g );
}
//-----------------------------------------------------------------------------
sdsp::HostTimeInfo PluginSessionHost::getHostTimeInfo(int filter) {
    typedef SessionHost::Operations::GetTimeInfo Op;
    MemoryGuard::Ptr g = getMemoryGuard();
    Op::ArgPtr args = g->getArg<Op::Arg>();
    Op::RetPtr rets = g->getRet<Op::Ret>();
    args->filter = (Integer)filter;
    waitForResult(SessionHost::OpcM::getOPC<Op>(), g);
    return rets->info;
}
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(PluginSessionHost, GetPluginInfo) {
    const ::processing::PluginInfo info = delegate->getPluginInfo();
    shm_cpypath(ret->location, info.location);
    shm_cpystr(ret->name, info.name);
    shm_cpystr(ret->vendor, info.vendor);
    ret->isSynth = info.isSynth;
    ret->uid = 0; //TODO: info.uid; string goes here
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
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(PluginSessionHost, GetStateData) {
    enum { OPC = OpcM::GetOPC<Operations::GetStateData>::Value };
    void *data; size_t size;
    boost::tie(size, data) = delegate->getPluginImpl()->getStateData();
    transferData(OPC, data, size, getTransferSenderGuard());
}
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(PluginSessionHost, SetStateData) {
    enum { OPC = OpcM::GetOPC<Operations::SetStateData>::Value };
    size_t byteSize = getTransferedDataSize(OPC);
    if (byteSize==0) {
        return;
    }
    void * data;
    TransferReceiverGuardPtr guard;
    boost::tie(data, guard) = getTransferedData(OPC);
    delegate->getPluginImpl()->setStateData(byteSize, data);
}
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(PluginSessionHost, CanHandleMidi) {
    ret->value = delegate->getPluginImpl()->canHandleMidiEvent();
}
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(PluginSessionHost, ProcessMidiEvents) {
    enum { OPC = OpcM::GetOPC<Operations::ProcessMidiEvents>::Value };
    size_t byteSize = getTransferedDataSize(OPC);
    if (byteSize==0) {
        return;
    }
    using sambag::dsp::IMidiEvents;
    void *data;
    TransferReceiverGuardPtr guard;
    boost::tie(data, guard) = getTransferedData(OPC);
    tmpMidiEvents = MidiEventsPtr(sambag::dsp::createMidiEvents((IMidiEvents::DataPtr)data, byteSize));
    delegate->getPluginImpl()->processMidiEvents(tmpMidiEvents.get());
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
    res->startProcessThread();
    return res;
}
//-----------------------------------------------------------------------------
void PluginSessionClient::updatePluginInfo (::processing::PluginInfo &inf) {
    typedef PluginSessionHost::Operations::GetPluginInfo Op;
    MemoryGuard::Ptr g = getMemoryGuard();
    Op::RetPtr rets = g->getRet<Op::Ret>();
    waitForResult( SessionHost::OpcM::getOPC<Op>(), g);
    inf.name = rets->name;
    inf.vendor = rets->vendor;
    inf.isSynth = rets->isSynth;
    inf.uid = rets->uid;
    inf.pluginType = rets->type;
}
//-----------------------------------------------------------------------------
void PluginSessionClient::turnOff() {
    typedef SessionHost::Operations::TurnOff Op;
    MemoryGuard::Ptr g = getMemoryGuard();
    waitForProcess(SessionHost::OpcM::getOPC<Op>(), g);
}
//-----------------------------------------------------------------------------
void PluginSessionClient::turnOn() {
    typedef SessionHost::Operations::TurnOn Op;
    MemoryGuard::Ptr g = getMemoryGuard();
    waitForProcess(SessionHost::OpcM::getOPC<Op>(), g);
}
//-----------------------------------------------------------------------------
void PluginSessionClient::openPlugin() {
    typedef SessionHost::Operations::Open Op;
    MemoryGuard::Ptr g = getMemoryGuard();
    waitForProcess(SessionHost::OpcM::getOPC<Op>(), g);
}
//-----------------------------------------------------------------------------
void PluginSessionClient::closePlugin() {
    typedef SessionHost::Operations::Close Op;
    MemoryGuard::Ptr g = getMemoryGuard();
    waitForProcess(SessionHost::OpcM::getOPC<Op>(), g);
}
//-----------------------------------------------------------------------------
size_t PluginSessionClient::getNumInputChannels() {
    typedef SessionHost::Operations::GetNumInputChannels Op;
    MemoryGuard::Ptr g = getMemoryGuard();
    Op::RetPtr rets = g->getRet<Op::Ret>();
    waitForResult(SessionHost::OpcM::getOPC<Op>(), g);
    tmpNumInputs = rets->num;
    return rets->num;
}
//-----------------------------------------------------------------------------
size_t PluginSessionClient::getNumOutputChannels() {
    typedef SessionHost::Operations::GetNumOutputChannels Op;
    MemoryGuard::Ptr g = getMemoryGuard();
    Op::RetPtr rets = g->getRet<Op::Ret>();
    waitForResult(SessionHost::OpcM::getOPC<Op>(), g);
    tmpNumOutputs = rets->num;
    return rets->num;
}
//-----------------------------------------------------------------------------
int PluginSessionClient::getNumParameter()
{
    typedef SessionHost::Operations::GetNumParameter Op;
    MemoryGuard::Ptr g = getMemoryGuard();
    Op::RetPtr rets = g->getRet<Op::Ret>();
    waitForResult(SessionHost::OpcM::getOPC<Op>(), g);
    return rets->num;
}
//-----------------------------------------------------------------------------
void PluginSessionClient::getParameterValues(
    ::processing::parameter::Parameter::Ptr p, size_t index)
{
    typedef SessionHost::Operations::GetParameterValues Op;
    MemoryGuard::Ptr g = getMemoryGuard();
    Op::ArgPtr args = g->getArg<Op::Arg>();
    Op::RetPtr rets = g->getRet<Op::Ret>();
    args->index = index;
    waitForResult(SessionHost::OpcM::getOPC<Op>(), g);
    p->setName(rets->name);
    p->setDisplay(rets->display);
    p->setLabel(rets->label);
    p->setValue(rets->value);
}
//-----------------------------------------------------------------------------
void PluginSessionClient::setParameterValues(
    ::processing::parameter::Parameter::Ptr p, size_t index)
{
    typedef SessionHost::Operations::SetParameterValue Op;
    MemoryGuard::Ptr g = getMemoryGuard();
    Op::ArgPtr args = g->getArg<Op::Arg>();
    Op::RetPtr rets = g->getRet<Op::Ret>();
    args->index = index;
    args->value = p->getValue();
    waitForResult(SessionHost::OpcM::getOPC<Op>(), g);
    p->setDisplay(rets->display);
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
    
    Float **ins, **outs;
    
    {
        MemoryGuard::Ptr mem = getMemoryGuard();
        Op::ArgPtr arg = static_cast<Op::ArgPtr>( mem->argmem );
        Op::RetPtr ret = static_cast<Op::RetPtr>( mem->retmem );
    
        PointerIterator arg_pIt(arg + 1, getRequestArgmemSize() - sizeof(Op::Arg));
        PointerIterator ret_pIt(ret + 1, getRequestRetmemSize() - sizeof(Op::Ret));
    
        ins = _assignMemory<Float>(arg_pIt, tmpNumInputs, numSamples);
        outs = _assignMemory<Float>(ret_pIt, tmpNumOutputs, numSamples);
   
        // copy into shared session memory
        for (int i=0; i<tmpNumInputs; ++i) {
            memcpy(ins[i], srcIns[i], numSamples*sizeof(Float));
        }
        // call process
        arg->numSamples = numSamples;
        waitForProcess (SessionHost::OpcM::getOPC<Op>(), mem);
    
        // copy result into out memry
        for (int i=0; i<tmpNumOutputs; ++i) {
            memcpy(srcOuts[i], outs[i], numSamples*sizeof(Float));
        }
    }
    
    delete [] ins;
    delete [] outs;
}
//-----------------------------------------------------------------------------
bool PluginSessionClient::hasEditor() {
    typedef SessionHost::Operations::HasEditor Op;
    MemoryGuard::Ptr g = getMemoryGuard();
    Op::RetPtr rets = g->getRet<Op::Ret>();
    waitForResult(SessionHost::OpcM::getOPC<Op>(), g);
    return rets->value;
}
//-----------------------------------------------------------------------------
std::string PluginSessionClient::getEditorSessionId() {
    typedef SessionHost::Operations::GetEditorSessionId Op;
    MemoryGuard::Ptr g = getMemoryGuard();
    Op::RetPtr rets = g->getRet<Op::Ret>();
    waitForResult(SessionHost::OpcM::getOPC<Op>(), g);
    return rets->id;
}

//-----------------------------------------------------------------------------
void PluginSessionClient::openEditor() {
    typedef SessionHost::Operations::OpenEditor Op;
    MemoryGuard::Ptr g = getMemoryGuard();
    waitForProcess(SessionHost::OpcM::getOPC<Op>(), g);
}
//-----------------------------------------------------------------------------
void PluginSessionClient::closeEditor() {
    typedef SessionHost::Operations::CloseEditor Op;
    MemoryGuard::Ptr g = getMemoryGuard();
    waitForProcess(SessionHost::OpcM::getOPC<Op>(), g);
}
//-----------------------------------------------------------------------------
void PluginSessionClient::setHostInfo(IHostInfo::Ptr hI) {
    hostInfo = hI;
}
//-----------------------------------------------------------------------------
std::pair<size_t, void*> PluginSessionClient::getStateData() {
    SAMBAG_LOG_TRACE<<"demand bridged plugin state data";
    typedef SessionHost::Operations::GetStateData Op;
    enum {OPC = SessionHost::OpcM::GetOPC<Op>::Value};
    MemoryGuard::Ptr g = getMemoryGuard();
    waitForProcess(OPC, g);
    void *data;
    TransferReceiverGuardPtr guard;
    boost::tie(data, guard) = getTransferedData(OPC);
    std::pair<size_t, void*> res = std::make_pair(
        getTransferedDataSize(OPC),
        data
    );
    SAMBAG_LOG_TRACE<<"got "<<res.first<<" bytes.";
    return res;
}
//-----------------------------------------------------------------------------
void PluginSessionClient::setStateData(size_t size, void* data) {
    typedef SessionHost::Operations::SetStateData Op;
    enum {OPC = SessionHost::OpcM::GetOPC<Op>::Value};
    transferData(OPC, data, size, getTransferSenderGuard());
    waitForProcess(OPC, getMemoryGuard());
    SAMBAG_LOG_TRACE<<"set bridged plugins state ("<<size<<" bytes)";
}
//-----------------------------------------------------------------------------
bool PluginSessionClient::canHandleMidiEvent() {
    typedef SessionHost::Operations::CanHandleMidi Op;
    MemoryGuard::Ptr g = getMemoryGuard();
    Op::RetPtr rets = g->getRet<Op::Ret>();
    waitForResult(SessionHost::OpcM::GetOPC<Op>::Value, g);
    return rets->value;
}
//-----------------------------------------------------------------------------
void PluginSessionClient::processMidiEvents(sambag::dsp::IMidiEvents *ev) {
    if (!ev) {
        return;
    }
    using namespace sambag::dsp;
    typedef SessionHost::Operations::ProcessMidiEvents Op;
    enum {OPC = SessionHost::OpcM::GetOPC<Op>::Value};
    // get raw event data
    std::vector<IMidiEvents::Data> tmp;
    createFlatRawData(*ev, tmp);
    if (tmp.empty()) {
        return;
    }
    transferData(OPC, &tmp[0], tmp.size(), getTransferSenderGuard());
    waitForProcess(OPC, getMemoryGuard());
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
