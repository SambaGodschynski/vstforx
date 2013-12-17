/*
 * PluginSession.cpp
 *
 *  Created on: Tue Dec  3 13:30:20 2013
 *      Author: Johannes Unger
 */

#include "PluginSession.hpp"
#include <sambag/com/exceptions/IllegalArgumentException.hpp>
#include <algorithm>

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
    :  Session( normalizeStringForShmId( dg->getLocation() ),
                ChannelSize(
                    _minmem<OpcM>(dg->getBlockSize(), dg->getNumInputChannels()),
                    _minmem<OpcM>(dg->getBlockSize(), dg->getNumOutputChannels())
                ), ChannelSize(
                    PluginSessionHost::OpcM::MaxArgmemSize,
                    PluginSessionHost::OpcM::MaxRetmemSize
                )
        )
{
    float sampleRate = dg->getSampleRate();
    size_t bs = dg->getBlockSize();
    if (sampleRate<=0 || bs<=0) {
        using sambag::com::exceptions::IllegalArgumentException;
        std::stringstream ss;
        ss<<"BridgeSession illegal audio setup: sr("<<sampleRate<<") ";
        ss<<"bs("<<bs<<")";
        SAMBAG_THROW(IllegalArgumentException,
        ss.str());
    }
    setMaxSleeping((Integer)(1000.f/(sampleRate/(float)bs))); // TODO: only a suggestion
}
//-----------------------------------------------------------------------------
void PluginSessionHost::processImpl(Opc opc, void *argmem, void *retmem) {
    try {
        Operations::OpcManager::process(opc, this, argmem, retmem);
    } catch(const std::exception &ex) {
        SAMBAG_LOG_ERR<<ex.what()<<" opc("<<opc<<")";
        throw;
    }
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
void PluginSessionHost::auto_opc_callback(Operations::Close::ArgPtr,
        Operations::Close::RetPtr)
{
}
//=============================================================================
//  Class PluginSessionClient
//=============================================================================
//-----------------------------------------------------------------------------
PluginSessionClient::PluginSessionClient(const std::string &id) : Session(id)
{
}
//-----------------------------------------------------------------------------
void PluginSessionClient::processImpl(Opc opc, void *argmem, void *retmem) {
    try {
        Operations::OpcManager::process(opc, this, argmem, retmem);
    } catch(const std::exception &ex) {
        SAMBAG_LOG_ERR<<ex.what()<<" opc("<<opc<<")";
        throw;
    }
}
//-------------------------------------------------------------------------
PluginSessionClient::Ptr PluginSessionClient::create(const std::string &id) {
    Ptr res = Ptr( new PluginSessionClient(id) );
    return res;
}
}}} // namespace(s)
