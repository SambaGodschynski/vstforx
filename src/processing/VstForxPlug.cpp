/*
 * VstForxPlug.cpp
 *
 *  Created on: Mon Oct  8 12:58:07 2012
 *      Author: Johannes Unger
 */

#include "VstForxPlug.hpp"
#include <stdlib.h>
#include <sstream>
#include <processing/Frames.h>
#include <processing/parameter/parameter.h>
#include <com/one4All.h>
#include <boost/bimap.hpp>
#include <processing/SerializationRegister.hpp>
#include <OS_Specific/OS_com.h>
#include <gui/components/VstForxEditor.hpp>
#include <sambag/com/exceptions/IllegalStateException.hpp>
#include <gui/components/FrxSerializationRegister.hpp>
#include <processing/FrxAsyncDSPTimer.hpp>
#include <sambag/com/Common.hpp>
#include <com/Legacy.hpp>
#include <processing/ProcessorAdapter.hpp>
namespace frx { namespace processing {
namespace {
    /**
     * @brief will be thrown if archve has old format
     */
    struct LegacyArchive {
        int version;
        LegacyArchive(int version) : version(version){}
    };
	struct HostInfoAdapter : public IHostInfo {
		template< typename Archive >
		void serialize ( Archive &ar, const unsigned int version ) {
			ar & boost::serialization::base_object<IHostInfo> ( *this );
            archiveVersion = version;
		} 
		IHostInfo *hostInfo;
		HostInfoAdapter(IHostInfo *hostInfo=NULL) :
            hostInfo(hostInfo), archiveVersion(0) {}
		virtual float getSampleRate() const {
			return hostInfo->getSampleRate();
		}
		virtual int getBlockSize() const {
			return hostInfo->getBlockSize();
		}
		virtual bool ioChanged() {
			return hostInfo->ioChanged();
		}
		virtual TimeInfo * getHostTimeInfo (int filter) {
			return hostInfo->getHostTimeInfo(filter);
		}
		virtual HostIOChangedConnection 
			addHostChangedListener(const HostIOChangedFunction &f) 
		{
			return hostInfo->addHostChangedListener(f);
		}
		virtual HostIOChangedConnection 
		addTrackedHostChangedListener(const HostIOChangedFunction &f, AnyWPtr wptr)
		{
			return hostInfo->addTrackedHostChangedListener(f, wptr);
		}
		virtual void * getEffectPtr() {
			return hostInfo->getEffectPtr();
		}
		virtual void * getMasterCallback() {
			return hostInfo->getMasterCallback();
		}
        virtual MasterType getMasterType() const {
			return hostInfo->getMasterType();
		}
        virtual scripts::PluginScriptCtrlPtr getScriptController() {
            return hostInfo->getScriptController();
        }
        int archiveVersion;
	};
	//-------------------------------------------------------------------------
	typedef boost::bimap<fgc::FrxCircuidViewPtr, VstForxPlug*>
		PlugMap;
	PlugMap plugMap;
	//-------------------------------------------------------------------------
	VstForxPlug * getPlugin(fgc::FrxCircuidViewPtr view) {
		PlugMap::left_map::const_iterator it = plugMap.left.find(view);
		if (it==plugMap.left.end())
			return NULL;
		return it->second;
	}
	int _instances = 0;
	FrxAsyncDSPTimer::WorkerThreadHolder _timerThreadHolder;

} // namespace
    typedef HostInfoAdapter __HostInfoAdapter__;
}}

BOOST_CLASS_VERSION(frx::processing::__HostInfoAdapter__, FRX_ARCHIVE_VERSION)

namespace frx { namespace processing {
//=============================================================================
// class VstForxPlug 
//=============================================================================
//-----------------------------------------------------------------------------
VstForxPlug::VstForxPlug() : 
effectPtr(NULL), 
masterCallback(NULL),
chunkData(NULL),
blockSize(0),
sampleRate(0.f)
{
}
//-----------------------------------------------------------------------------
void VstForxPlug::onGraphDelayChanged(void *src, const ::processing::GraphDelayChanged &ev)
{
	if (host) {
		host->delayChanged(ev.delay);
	}
}
//-----------------------------------------------------------------------------
void VstForxPlug::registerView(fgc::FrxCircuidViewPtr view) {
	PlugMap::right_map::iterator it = plugMap.right.find(this);
	if (it!=plugMap.right.end()) {
		plugMap.right.erase(it);
	}
	plugMap.insert(PlugMap::value_type(view, this));
}
//-----------------------------------------------------------------------------
void VstForxPlug::unRegisterView(fgc::FrxCircuidViewPtr view) {
    PlugMap::left_map::iterator it = plugMap.left.find(view);
	if (it == plugMap.left.end()) {
        return;
    }
	plugMap.left.erase(it);
}
//-----------------------------------------------------------------------------
void VstForxPlug::unRegisterInstance() {
	PlugMap::right_map::iterator it = plugMap.right.find(this);
	if (it == plugMap.right.end())
		return;
	plugMap.right.erase(it);
}
//-----------------------------------------------------------------------------
void VstForxPlug::installGraphListener() {
	typedef com::events::EventSender< ::processing::GraphDelayChanged> Sender;
	graph->Sender::addTrackedEventListener(
		boost::bind(&VstForxPlug::onGraphDelayChanged, this, _1, _2),
		hostInfoAdapter
	);
}
//-----------------------------------------------------------------------------
void VstForxPlug::open() {
	if (isOpen()) {
		return;
	}
	if (_instances++ == 0) {
		_timerThreadHolder = 
			FrxAsyncDSPTimer::startWorkerThread();
	}
	_open = true;
	if (graph) {
		updateGraphBaseConfiguration();
		return;
	}
	hostInfoAdapter = IHostInfo::Ptr(new HostInfoAdapter(this));
	graph = ::processing::Graph::create(hostInfoAdapter);
	installGraphListener();
	ctrl = ModelController::create();
	ctrl->setGraph(graph);
	map = frx::gui::ViewModelMap::create();
	updateGraphBaseConfiguration();
	initHostParameter();
    processingThread = sambag::com::getThreadId();
}	
//-----------------------------------------------------------------------------
void VstForxPlug::initHostParameter() {
	// init hostParameter	
	for ( size_t i=0; i<graph->getNumHostParameter(); ++i ){
		graph->getHostParameter(i)->addValueChangedListener ( 
			boost::bind(&VstForxPlug::hostParameterChanged, this, _1, _2, i)
		);
	}
}
//-----------------------------------------------------------------------------
void VstForxPlug::close() {
	if (!isOpen()) {
		return;
	}
	_open = false;
    
	if (chunkData) {
		delete chunkData;
		chunkData = NULL;
	}

	if (--_instances == 0) {
		// stop timer worker thread
		FrxAsyncDSPTimer::closeAllTimer();
		_timerThreadHolder.reset();
	}
}
//-----------------------------------------------------------------------------
VstForxPlug::~VstForxPlug() {
    unRegisterInstance();
}
//-----------------------------------------------------------------------------
void VstForxPlug::process(float **in, float **out, int numSamples) {
    lastTimeInfo = *(getHost()->getHostTimeInfoImpl(0xFFFF));
	TRY_TO_LOCK_TIMED2 (processingLoadLock, 120);
	if ( !graph ) 
		return;
	::processing::Frames fr ( in, numSamples ); 
	if ( !graph->getEndNode()->isActive() ){
		fr.setZero ( numSamples );
		fr.getBlock ( out, numSamples );
		return;
	}
	{
		TRY_TO_LOCK_TIMED2 ( graph->getProcessingLock(), 10 ); // pushandcopy needs the lock #issue272
		graph->pushAndCopy ( &fr, numSamples );
		graph->processGraph( out, numSamples  );
	}
}
//-----------------------------------------------------------------------------
void VstForxPlug::processEvents(sambag::dsp::IMidiEvents *ev) {
	if ( !graph ) 
		return;
	graph->processEvents(ev);
}
//-----------------------------------------------------------------------------
void VstForxPlug::setBlockSize(int blockSize) {
	this->blockSize = blockSize; 
	updateGraphBaseConfiguration();
}
//-----------------------------------------------------------------------------
void VstForxPlug::setSampleRate(float sampleRate)  {
	this->sampleRate = sampleRate; 
	updateGraphBaseConfiguration();
}
//-----------------------------------------------------------------------------
void VstForxPlug::setParameterValue(int index, float value) {
	if ( !graph ) 
		return;
	if (!onHostParameterUpdate) {
		*(graph->getHostParameter(index)) = value;
	}
}
//-----------------------------------------------------------------------------
void VstForxPlug::getParameterValue(int index, float &outValue) {
	if ( !graph ) 
		outValue = 0.0;
	outValue = *(graph->getHostParameter(index));
}
//-----------------------------------------------------------------------------
void VstForxPlug::getParameterName (int index, std::string &outStr) const
{
	if ( !graph ) 
		return;
	outStr = graph->getHostParameter(index)->getName();
}
//-----------------------------------------------------------------------------
void VstForxPlug::hostParameterChanged(void *src, float value, int index) {
	using ::processing::parameter::Parameter;
	onHostParameterUpdate = true;
	getHost()->parameterChanged(index);
	onHostParameterUpdate = false;
}
//-----------------------------------------------------------------------------
float VstForxPlug::getSampleRate() const  {
	return sampleRate;
}
//-----------------------------------------------------------------------------
int VstForxPlug::getBlockSize() const  {
	return blockSize;
}
//-----------------------------------------------------------------------------
bool VstForxPlug::ioChanged() {
	sce::EventSender<HostIOChanged>::notifyListeners(this, HostIOChanged());
	return true;
}
//-----------------------------------------------------------------------------
TimeInfo * VstForxPlug::getHostTimeInfo (int filter) {
    if (sambag::com::getThreadId()==processingThread) {
        return getHost()->getHostTimeInfoImpl(filter);
    }
    // this is a bit fuzzy becuase the lastTimeInfo could be at least
    // 1/(sampleRate/maxBlockSize)ms (e.g. 90 ms for bs=4096) old.
    // But when getHostTimeInfo() is called from another thread than the
    // processing thread (eg. called from a timer) we assume that
    // accuracy isn't so important at all.
    // If this thought appears as wrong, a solution could be to use a
    // stopwatch (dspTools::Timer) to calculate the difference
    // between getting lastTimeInfo and getHostTimeInfo() (for every
    // TimeInfo value!).
    return &lastTimeInfo;
}
//-----------------------------------------------------------------------------
VstForxPlug::HostIOChangedConnection 
VstForxPlug::addHostChangedListener(const HostIOChangedFunction &f) {
	return sce::EventSender<HostIOChanged>::addEventListener(f);
}
//-----------------------------------------------------------------------------
VstForxPlug::HostIOChangedConnection 
VstForxPlug::addTrackedHostChangedListener(const HostIOChangedFunction &f, AnyWPtr wptr)
{
	return sce::EventSender<HostIOChanged>::addTrackedEventListener(f, wptr);
}
//-----------------------------------------------------------------------------
void VstForxPlug::setEffectPtr(void *effPtr) {
	this->effectPtr = effPtr;
}
//-----------------------------------------------------------------------------
void VstForxPlug::setMasterCallback(void *mCallbk) {
	this->masterCallback = mCallbk;
}
//-----------------------------------------------------------------------------
void * VstForxPlug::getEffectPtr() {
	return effectPtr;
}
//-----------------------------------------------------------------------------
void * VstForxPlug::getMasterCallback() {
	return masterCallback;
}
//-----------------------------------------------------------------------------
void VstForxPlug::updateGraphBaseConfiguration() {
	if (!graph)
		return;
	if (sampleRate==0. || blockSize == 0)
		return;
    SAMBAG_LOG_INFO<<"update configuration: ...";
    SAMBAG_LOG_INFO<<"  + set samplerate: "<<sampleRate;
    SAMBAG_LOG_INFO<<"  + set blocksize: "<<blockSize;
	graph->getJanitor()->hostBaseConfigChanged();
    SAMBAG_LOG_INFO<<"update configuration: SUCCEED";
}
//-----------------------------------------------------------------------------
bool VstForxPlug::requestEditorResize(int width, int height) {
	return getHost()->requestEditorResize(width, height);
}
//-----------------------------------------------------------------------------
int VstForxPlug::getChunk(void **data) {
	if (!_open) {
		open();
	}
	try {
        SAMBAG_LOG_INFO<<"serialize vstforx: ...";
		std::stringstream ss;
		save(ss);
		const std::string &datastr = ss.str();
		size_t datasize = datastr.size();
		if (datasize==0) {
			// some hosts have problems with
			// 0 as result so we do a dummy save op.:
			return Super::getChunk(data);
		}
		if (chunkData) {
			delete chunkData;
			chunkData = NULL;
		}
		chunkData = new char[datasize];
		memcpy(chunkData, datastr.c_str(), datasize);
		*data = (void*)chunkData;
		//std::cout<<datasize<<" bytes saved."<<std::endl;
        SAMBAG_LOG_INFO<<"serialize vstforx: SUCCEED, ("<< datasize <<" bytes)";
		return datasize;
	} catch(const std::exception &ex) {
        SAMBAG_LOG_ERR<<"serialize vstforx: FAILED, "<<ex.what();
		std::stringstream ss;
		ss<<"serialization vstforx: FAILED, "<<ex.what();
		::com::osMessageBox("Error", ss.str(), ::com::MSG_ALERT);
		return 0;
	} catch(...) {
        SAMBAG_LOG_ERR<<"serialize vstforx: FAILED, unknown error";
		std::stringstream ss;
		ss<<"serialization failed: unkown reason.";
		::com::osMessageBox("Error", ss.str(), ::com::MSG_ALERT);
		return 0;
	}
}
//-----------------------------------------------------------------------------
int VstForxPlug::setChunk(void *data, int byteSize, int version) {
	if (!_open) {
		open();
	}
	if (byteSize==0) {
		return 0;
	}
	try {
        SAMBAG_LOG_INFO<<"deserialize vstforx: ..., ("<<byteSize<<" bytes)";
		std::stringstream ss;
		std::string dataStr((char*)data, byteSize);
		ss<<dataStr;
		load(ss, version);
		//std::cout<<byteSize<<" bytes loaded."<<std::endl;
        SAMBAG_LOG_INFO<<"deserialize vstforx: SUCCEED";
		return byteSize;
	} catch(const LegacyArchive &ex) {
        if (version==ex.version) {
            ::com::osMessageBox("Error", "unhandled version exception", ::com::MSG_ALERT);
            return 0;
        }
        setChunk(data, byteSize, ex.version);
    } catch(const std::exception &ex) {
        SAMBAG_LOG_ERR<<"deserialize vstforx: FAILED, "<<ex.what();
		std::stringstream ss;
		ss<<"serialization failed: "<<ex.what();
		::com::osMessageBox("Error", ss.str(), ::com::MSG_ALERT);
		return 0;
	} catch(...) {
        SAMBAG_LOG_ERR<<"deserialize vstforx: FAILED, unknown error";
		std::stringstream ss;
		ss<<"serialization failed: unkown reason.";
		::com::osMessageBox("Error", ss.str(), ::com::MSG_ALERT);
		return 0;
	}
    return 0;
}
//-----------------------------------------------------------------------------
void VstForxPlug::saveEditor(::com::oArchive &ar) {
	using frx::gui::components::VstForxEditor;
	VstForxEditor * editor = static_cast<VstForxEditor*>(
		host->getEditor()
	);
	if (!editor) {
		SAMBAG_THROW(
			sambag::com::exceptions::IllegalStateException,
			"editor == NULL"
		);
	}
	std::string serializedViewStream;
	std::stringstream tmpss;
	frx::gui::ViewModelMap::Ptr tmpMap;
	frx::gui::ViewModelMap::Ptr origMap;
	if (editor->isOpen()) {
		origMap = map; // keep orig. map untouched
		tmpMap = map->clone();
		map = tmpMap;
		//void serializeViewTemp(::com::oArchive &ar, FrxCircuidViewPtr view);
		::com::oArchive tmp(tmpss);
		frx::gui::components::register_types(tmp);
		editor->serializeViewTemp(tmp, editor->getCircuidView());
		serializedViewStream = tmpss.str();
	} else {
		serializedViewStream = editor->hiChamber.first;
	}
	frx::gui::components::register_types(ar);
	ar.register_type<frx::gui::ViewModelMap>();
	ar & serializedViewStream; //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<1.
	ar & map;				   //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<2.
	
	if (origMap) {
		map = origMap;
	}
}
//-----------------------------------------------------------------------------
void VstForxPlug::loadEditor(::com::iArchive &ar, int version) {
	using frx::gui::components::VstForxEditor;
	VstForxEditor * editor = static_cast<VstForxEditor*>(
		host->getEditor()
	);
	if (!editor) {
		SAMBAG_THROW(
			sambag::com::exceptions::IllegalStateException,
			"editor == NULL"
		);
	}
    
    frx::gui::components::register_types(ar, version);
	
	ar.register_type<frx::gui::ViewModelMap>();
	std::string serializedViewStream;
	ar & serializedViewStream; //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<1.
	ar & map;				   //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<2.
	if (editor->isOpen()) {
		std::stringstream tmpss;
		tmpss<<serializedViewStream;
		::com::iArchive tmp(tmpss);
		frx::gui::components::register_types(tmp, version);
		frx::gui::components::FrxCircuidViewPtr view =
			editor->deserializeViewTemp(tmp);
		editor->setCircuidView(view);
		return;
	}
	editor->hiChamber.first  = serializedViewStream;
    editor->hiChamber.second = version;
}
//-----------------------------------------------------------------------------
void VstForxPlug::save(std::ostream &os) {
	::com::oArchive ar(os);
	ar.register_type<HostInfoAdapter>();
	register_types(ar);
	ar & hostInfoAdapter;
	ar & graph;
	saveEditor(ar);
    // script ctrl user data
    // the boost::serialization multimap impl. gives a fuck about
    // map value order, so we have to do it manually
    // (for some reason Map::value_type produces compiler errors)
    typedef std::pair<std::string, std::string> Bodge;
    std::vector<Bodge> tmp;
    if (scriptCtrl) {
        const scripts::PluginScriptCtrl::PersistUserData &data =
            scriptCtrl->getPersistUserData();
        tmp.reserve(data.size());
        BOOST_FOREACH(const Bodge &x, data) {
            tmp.push_back(x);
        }
    }
    ar<<tmp;
}
//-----------------------------------------------------------------------------
void VstForxPlug::load(std::istream &is, int version) {
	TRY_TO_LOCK_TIMED2 ( processingLoadLock, 10 );
	::processing::Graph::Ptr alt = graph; // hold old until loosing scope
	IHostInfo::Ptr altHostInfoAdapter = hostInfoAdapter;

	::com::iArchive ar(is);
    ar.register_type<HostInfoAdapter>();
   
    register_types(ar, version);
	
    ar & hostInfoAdapter;
    if (!hostInfoAdapter) {
        SAMBAG_THROW(
			sambag::com::exceptions::IllegalStateException,
			"VstForxPlug: serialization failed."
		);
    }
	HostInfoAdapter *hiAdapter = dynamic_cast<HostInfoAdapter*>
		(hostInfoAdapter.get());
    if (hiAdapter->archiveVersion!=version) {
        throw LegacyArchive(hiAdapter->archiveVersion);
    }
    hiAdapter->hostInfo = this;
	ar & graph;
	installGraphListener();
	ctrl->setGraph(graph);
	loadEditor(ar, version);
	// reinit graph
	::processing::Graph::Janitor::Ptr janitor = graph->getJanitor();
	if ( sampleRate != 0.0 && blockSize != 0 ) {
		janitor->hostBaseConfigChanged();
	}
	initHostParameter();
    // script ctrl user data
    if (version>0) {
        getScriptController()->getPersistUserData().clear();
        // script ctrl user data
        // the boost::serialization multimap impl. gives a fuck about
        // map value order, so we have to do it manually
        // (for some reason Map::value_type produces compiler errors)
        typedef std::pair<std::string, std::string> Bodge;
        std::vector<Bodge> tmp;
        scripts::PluginScriptCtrl::PersistUserData &data =
            scriptCtrl->getPersistUserData();
        ar >> tmp;
        BOOST_FOREACH(const Bodge &x, tmp) {
            data.insert(x);
        }
    }
}
//-----------------------------------------------------------------------------
void VstForxPlug::onScriptExeFailed(const frx::scripts::ScriptExeFailedEvent &ev)
{
}
//-----------------------------------------------------------------------------
void * VstForxPlug::getEditor() {
	return host->getEditor();
}
//-----------------------------------------------------------------------------
int VstForxPlug::getLatency() const {
	if (graph) {
		return graph->getGraphDelay();
	}
	return 0;
}
//-----------------------------------------------------------------------------
VstForxPlug::ScriptCtrlPtr VstForxPlug::getScriptController() {
    if (!scriptCtrl) {
        scriptCtrl = ScriptCtrlPtr(new frx::scripts::PluginScriptCtrl(true));
        scriptCtrl->setPlugin(this);
        scriptCtrl->sce::EventSender<frx::scripts::ScriptExeFailedEvent>::addEventListener(
            boost::bind(&VstForxPlug::onScriptExeFailed, this, _2)
        );
    }
    return scriptCtrl;
}
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
IModelController::Ptr
getModelController(frx::gui::components::FrxCircuidViewPtr view)
{
	VstForxPlug *plug = getPlugin(view);
	if (!plug)
		return IModelController::Ptr();
	return plug->getModelController();
}
VstForxPlug::ScriptCtrlPtr
getScriptControl(frx::gui::components::FrxCircuidViewPtr view) {
    VstForxPlug *plug = getPlugin(view);
	if (!plug)
		return VstForxPlug::ScriptCtrlPtr();
	return plug->getScriptController();
}
}} // namespace(s)

namespace frx { namespace gui {
//-----------------------------------------------------------------------------
IViewModelMap::Ptr 
getViewModelMap(components::FrxCircuidViewPtr view)
{
	using namespace frx::processing;
	VstForxPlug *plug = getPlugin(view);
	if (!plug)
		return frx::gui::IViewModelMap::Ptr();
	return plug->getViewModelMap();
}
}} // namespace(s)
