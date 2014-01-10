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

namespace frx { namespace processing { namespace interprocess {
namespace {
    template<class OpcM>
    inline Integer _minmem(Integer blockSize, Integer numChannels) {
        return std::max( blockSize*numChannels + OpcM::MaxArgmemSize
        , 256 );
    }
} // namespace
//=============================================================================
//  Class PluginSessionHost
//=============================================================================
//-----------------------------------------------------------------------------
PluginSessionHost::PluginSessionHost(BridgePluginDelegate::Ptr dg)
    :  Session( SessionManager::createUniqueName(),
                ChannelSize(
                    _minmem<OpcM>(dg->getBlockSize(), dg->getNumInputChannels()),
                    _minmem<OpcM>(dg->getBlockSize(), dg->getNumOutputChannels())
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
    setMaxSleeping(11);
    SAMBAG_LOG_TRACE<<"sleeping:"<<(Integer)(1000.f/(sampleRate/(float)bs)); // TODO: only a suggestion
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
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(PluginSessionHost, GetPluginInfo) {
    const ::processing::PluginInfo info = delegate->getPluginInfo();
    shm_cpypath(ret->location, info.location);
    shm_cpypath(ret->name, info.name);
    shm_cpypath(ret->vendor, info.vendor);
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
//=============================================================================
//  Class PluginSessionClient
//=============================================================================
//-----------------------------------------------------------------------------
PluginSessionClient::PluginSessionClient(const std::string &id) : Session(id)
{
}
//-----------------------------------------------------------------------------
PluginSessionClient::Ptr PluginSessionClient::create(const std::string &id) {
    Ptr res = Ptr( new PluginSessionClient(id) );
    return res;
}
//-----------------------------------------------------------------------------
void PluginSessionClient::updatePluginInfo (::processing::PluginInfo &inf) {
    typedef PluginSessionHost::Operations::GetPluginInfo Op;
    Op::RetPtr res = waitForResult<Op::RetPtr>( SessionHost::OpcM::getOPC<Op>(),
        FRX_BRIDGE_CREATE_PL_SESSION_TIMEOUT
    );
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
    waitForResult<Op::RetPtr>( SessionHost::OpcM::getOPC<Op>(),
        FRX_BRIDGE_CREATE_PL_SESSION_TIMEOUT
    );
}
//-----------------------------------------------------------------------------
void PluginSessionClient::turnOn() {
    typedef SessionHost::Operations::TurnOn Op;
    waitForResult<Op::RetPtr>( SessionHost::OpcM::getOPC<Op>(),
        FRX_BRIDGE_CREATE_PL_SESSION_TIMEOUT
    );
}
//-----------------------------------------------------------------------------
void PluginSessionClient::openPlugin() {
    typedef SessionHost::Operations::Open Op;
    waitForResult<Op::RetPtr>( SessionHost::OpcM::getOPC<Op>(),
        FRX_BRIDGE_CREATE_PL_SESSION_TIMEOUT
    );
}
//-----------------------------------------------------------------------------
void PluginSessionClient::closePlugin() {
    typedef SessionHost::Operations::Close Op;
    waitForResult<Op::RetPtr>( SessionHost::OpcM::getOPC<Op>(),
        FRX_BRIDGE_CREATE_PL_SESSION_TIMEOUT
    );
}
//-----------------------------------------------------------------------------
size_t PluginSessionClient::getNumInputChannels() {
    typedef SessionHost::Operations::GetNumInputChannels Op;
    Op::RetPtr res = waitForResult<Op::RetPtr>( SessionHost::OpcM::getOPC<Op>(),
        FRX_BRIDGE_CREATE_PL_SESSION_TIMEOUT
    );
    return res->num;
}
//-----------------------------------------------------------------------------
size_t PluginSessionClient::getNumOutputChannels() {
    typedef SessionHost::Operations::GetNumOutputChannels Op;
    Op::RetPtr res = waitForResult<Op::RetPtr>( SessionHost::OpcM::getOPC<Op>(),
        FRX_BRIDGE_CREATE_PL_SESSION_TIMEOUT
    );
    return res->num;
}
//-----------------------------------------------------------------------------
int PluginSessionClient::getNumParameter()
{
    typedef SessionHost::Operations::GetNumParameter Op;
    Op::RetPtr res = waitForResult<Op::RetPtr>( SessionHost::OpcM::getOPC<Op>(),
        FRX_BRIDGE_CREATE_PL_SESSION_TIMEOUT
    );
    return res->num;
}
//-----------------------------------------------------------------------------
void PluginSessionClient::getParameterValues(
    ::processing::parameter::Parameter::Ptr p, size_t index)
{
    typedef SessionHost::Operations::GetParameterValues Op;
    Op::ArgPtr args = static_cast<Op::ArgPtr>( getArgmem() );
    args->index = index;
    Op::RetPtr res = waitForResult<Op::RetPtr>( SessionHost::OpcM::getOPC<Op>(),
        FRX_BRIDGE_CREATE_PL_SESSION_TIMEOUT
    );
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
    Op::RetPtr res = waitForResult<Op::RetPtr>( SessionHost::OpcM::getOPC<Op>(),
        FRX_BRIDGE_CREATE_PL_SESSION_TIMEOUT
    );
    p->setDisplay(res->display);
}
}}} // namespace(s)
