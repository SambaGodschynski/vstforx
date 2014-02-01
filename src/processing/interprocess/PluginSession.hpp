/*
 * PluginSession.hpp
 *
 *  Created on: Tue Dec  3 13:30:20 2013
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_PLUGINSESSION_H
#define SAMBAG_PLUGINSESSION_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include "Session.hpp"
#include "ShmCom.hpp"
#include "BridgePluginDelegate.hpp"
#include <processing/pluginTypes/PluginImpl.hpp>
#include <processing/parameter/Parameter.h>
#include <sambag/dsp/HostTimeInfo.hpp>

namespace frx { namespace processing { namespace interprocess {
class BridgeSession;
class PluginSessionClient;
namespace sci = sambag::com::interprocess;
namespace sdsp = sambag::dsp;
//=============================================================================
/** 
  * @class PluginSession.
  */
class PluginSessionHost : public Session {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<PluginSessionHost> Ptr;
    //-------------------------------------------------------------------------
    typedef PluginSessionClient SessionHost; // host for session calls
    //-------------------------------------------------------------------------
    typedef ::processing::Frames::T Float;
    //-------------------------------------------------------------------------
    FRX_OP_BEGIN_OPERATIONS
        FRX_OP_OPERATION(Open, FRX_OP_ARG(), FRX_OP_RET());
        FRX_OP_OPERATION(Close, FRX_OP_ARG(), FRX_OP_RET());
        FRX_OP_OPERATION(TurnOn, FRX_OP_ARG(), FRX_OP_RET());
        FRX_OP_OPERATION(TurnOff, FRX_OP_ARG(), FRX_OP_RET());
        FRX_OP_OPERATION(GetPluginInfo,
            FRX_OP_ARG(),
            FRX_OP_RET_6(
                char location[FRX_SHMSESS_MAX_PATH_LENGTH],
                char name[FRX_SHMSESS_MAX_STR_LENGTH],
                char vendor[FRX_SHMSESS_MAX_STR_LENGTH],
                bool isSynth,
                int uid,
                ::processing::PluginInfo::PluginType type
            ));
        FRX_OP_OPERATION(GetNumInputChannels, FRX_OP_ARG(), FRX_OP_RET_1(int num));
        FRX_OP_OPERATION(GetNumOutputChannels, FRX_OP_ARG(), FRX_OP_RET_1(int num));
        FRX_OP_OPERATION(GetNumParameter, FRX_OP_ARG(), FRX_OP_RET_1(int num));
        FRX_OP_OPERATION(GetParameterValues,
            FRX_OP_ARG_1(int index),
            FRX_OP_RET_4(char name[FRX_SHMSESS_MAX_STR_LENGTH],
                         char display[FRX_SHMSESS_MAX_STR_LENGTH],
                         char label[FRX_SHMSESS_MAX_STR_LENGTH],
                         float value)
        );
        FRX_OP_OPERATION(SetParameterValue,
            FRX_OP_ARG_2(int index, float value),
            FRX_OP_RET_1(char display[FRX_SHMSESS_MAX_STR_LENGTH])
        );
        FRX_OP_OPERATION(Process,
            FRX_OP_ARG_1(int numSamples), // sample memory is beyond of this struct
            FRX_OP_RET()
        );
        FRX_OP_OPERATION(GetEditorSessionId, FRX_OP_ARG(),
            FRX_OP_RET_1(char id[FRX_SHMSESS_MAX_STR_LENGTH])
        );
        FRX_OP_OPERATION(HasEditor, FRX_OP_ARG(), FRX_OP_RET_1(bool value));
        FRX_OP_OPERATION(OpenEditor, FRX_OP_ARG(), FRX_OP_RET());
        FRX_OP_OPERATION(CloseEditor, FRX_OP_ARG(), FRX_OP_RET());
        FRX_OP_OPERATION(GetStateData, FRX_OP_ARG(), FRX_OP_RET()); // uses Session::trasferData()
        FRX_OP_OPERATION(SetStateData, FRX_OP_ARG(), FRX_OP_RET()); // uses Session::trasferData()
        //---------------------------------------------------------------------
        typedef LOKI_TYPELIST_17(Open,
            Close,
            GetPluginInfo,
            TurnOn,
            TurnOff,
            GetNumInputChannels,
            GetNumOutputChannels,
            GetNumParameter,
            GetParameterValues,
            SetParameterValue,                      // 10
            Process,
            GetEditorSessionId,
            HasEditor,
            OpenEditor,
            CloseEditor,
            GetStateData,
            SetStateData
        ) OPs;
    FRX_OP_END_OPERATIONS_AND_IMPL_PROCESS(OPs)
    //-------------------------------------------------------------------------
    PluginSessionHost(BridgePluginDelegate::Ptr delegate);
private:
    //-------------------------------------------------------------------------
    BridgeSession *host;
    //-------------------------------------------------------------------------
    BridgePluginDelegate::Ptr delegate;
public:
    //-------------------------------------------------------------------------
    void onPluginPropertyChanged(void*, const sce::PropertyChanged &ev);
    //-------------------------------------------------------------------------
    void onPluginEditorResized(const APluginImpl::EditorSize &val);
    //-------------------------------------------------------------------------
    sdsp::HostTimeInfo getHostTimeInfo(int filter);
    //-------------------------------------------------------------------------
    BridgeSession * getBridgeSession() const {
        return host;
    }
    //-------------------------------------------------------------------------
    /**
     * creates new session
     */
    static Ptr create(BridgePluginDelegate::Ptr delegate, BridgeSession *host);
    //-------------------------------------------------------------------------
    ///////////////////////////////////////////////////////////////////////////
    //-------------------------------------------------------------------------
    FRX_OP_CALLBACK_METHOD(Open);
    FRX_OP_CALLBACK_METHOD(Close);
    FRX_OP_CALLBACK_METHOD(TurnOn);
    FRX_OP_CALLBACK_METHOD(TurnOff);
    FRX_OP_CALLBACK_METHOD(GetPluginInfo);
    FRX_OP_CALLBACK_METHOD(GetNumInputChannels);
    FRX_OP_CALLBACK_METHOD(GetNumOutputChannels);
    FRX_OP_CALLBACK_METHOD(GetNumParameter);
    FRX_OP_CALLBACK_METHOD(GetParameterValues);
    FRX_OP_CALLBACK_METHOD(SetParameterValue);
    FRX_OP_CALLBACK_METHOD(Process);
    FRX_OP_CALLBACK_METHOD(GetEditorSessionId);
    FRX_OP_CALLBACK_METHOD(HasEditor);
    FRX_OP_CALLBACK_METHOD(OpenEditor);
    FRX_OP_CALLBACK_METHOD(CloseEditor);
    FRX_OP_CALLBACK_METHOD(GetStateData);
    FRX_OP_CALLBACK_METHOD(SetStateData);

}; // PluginSession
//=============================================================================
/** 
  * @class PluginSessionClient.
  */
class PluginSessionClient : public Session,
    public sce::EventSender<sce::PropertyChanged>
{
//=============================================================================
public:
    //-------------------------------------------------------------------------
	typedef boost::shared_ptr<PluginSessionClient> Ptr;
    //-------------------------------------------------------------------------
	typedef boost::weak_ptr<PluginSessionClient> WPtr;
    //-------------------------------------------------------------------------
    typedef PluginSessionHost SessionHost; // host for session calls
    //-------------------------------------------------------------------------
    FRX_OP_BEGIN_OPERATIONS
        FRX_OP_OPERATION(OnEditorResized,
            FRX_OP_ARG_1(APluginImpl::EditorSize val),
            FRX_OP_RET()
        );
        FRX_OP_OPERATION(GetTimeInfo,
            FRX_OP_ARG_1(Integer filter),
            FRX_OP_RET_1(sambag::dsp::HostTimeInfo info)
        );
        typedef LOKI_TYPELIST_2(OnEditorResized,
            GetTimeInfo
        ) OPs;
    FRX_OP_END_OPERATIONS_AND_IMPL_PROCESS(OPs)
    //-------------------------------------------------------------------------
    FRX_OP_CALLBACK_METHOD(OnEditorResized);
    FRX_OP_CALLBACK_METHOD(GetTimeInfo);
private:
    //-------------------------------------------------------------------------
    // will be updated with every getNumXXXChannels call
    // needed for shared memory alloc in process
    mutable int tmpNumInputs, tmpNumOutputs;
    //-------------------------------------------------------------------------
    IHostInfo::Ptr hostInfo;
protected:
    //-------------------------------------------------------------------------
    PluginSessionClient(const std::string &id);
    //-------------------------------------------------------------------------
    /**
     * @brief since Session channel communication can be blocked when
     * both channels are in use we notify events delayed to prevent running
     * into mutual blocking. 
     */
    template <class Event>
    void doSendEvent(Event &ev) {
        sce::EventSender<Event>::notifyListeners(this, ev);
    }
    //-------------------------------------------------------------------------
    WPtr self;
public:
    //-------------------------------------------------------------------------
    void setHostInfo(IHostInfo::Ptr hI);
    //-------------------------------------------------------------------------
    IHostInfo::Ptr getHostInfo() const {
        return hostInfo;
    }
    //-------------------------------------------------------------------------
    /**
     * @brief creates new session
     */
    static Ptr create(const std::string &id);
    //-------------------------------------------------------------------------
    /**
     * @brief transmit plugin parameter values on index i into p
     */
    void getParameterValues(::processing::parameter::Parameter::Ptr p, size_t index);
    //-------------------------------------------------------------------------
    /**
     * @brief transmit parameter values from p into plugin parameter on index i
     */
    void setParameterValues(::processing::parameter::Parameter::Ptr p, size_t index);
    //-------------------------------------------------------------------------
    int getNumParameter();
    //-------------------------------------------------------------------------
    ///////////////////////////////////////////////////////////////////////////
    // APluginImpl
    //-------------------------------------------------------------------------
    void updatePluginInfo (::processing::PluginInfo &inf);
    void turnOff();
    void turnOn();
    void openPlugin();
    void closePlugin();
    size_t getNumInputChannels();
    size_t getNumOutputChannels();
    void process(SessionHost::Float **ins, SessionHost::Float **outs, int numSamples);
    /**
     * @return id of WindowSessionHost if exists 
     */
    std::string getEditorSessionId();
    bool hasEditor();
    void openEditor();
    void closeEditor();
    std::pair<size_t, void*> getStateData();
    void setStateData(size_t size, void* data);
}; // PluginSession
}}} // namespace(s)

#endif /* SAMBAG_PLUGINSESSION_H */
