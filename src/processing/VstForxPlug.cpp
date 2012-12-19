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

namespace frx { namespace processing {
namespace {
	struct HostInfoAdapter : public IHostInfo {
		template< typename Archive >
		void serialize ( Archive &ar, const unsigned int version ) {
			ar & boost::serialization::base_object<IHostInfo> ( *this );
		} 
		IHostInfo *hostInfo;
		HostInfoAdapter(IHostInfo *hostInfo=NULL) : hostInfo(hostInfo) {}
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
} // namespace
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
	if (it == plugMap.left.end())
		return;
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
void VstForxPlug::open() {
	if (graph) {
		updateGraphBaseConfiguration();
		return;
	}
	hostInfoAdapter = IHostInfo::Ptr(new HostInfoAdapter(this));
	graph = ::processing::Graph::create(hostInfoAdapter);
	ctrl = ModelController::create();
	ctrl->setGraph(graph);
	map = frx::gui::ViewModelMap::create();
	updateGraphBaseConfiguration();
	initHostParameter();
}	
//-----------------------------------------------------------------------------
void VstForxPlug::initHostParameter() {
	// init hostParameter	
	for ( int i=0; i<graph->getNumHostParameter(); ++i ){
		graph->getHostParameter(i)->addValueChangedListener ( 
			boost::bind(&VstForxPlug::hostParameterChanged, this, _1, _2, i)
		);
	}
}
//-----------------------------------------------------------------------------
void VstForxPlug::close() {
}
//-----------------------------------------------------------------------------
VstForxPlug::~VstForxPlug() {
	unRegisterInstance();
	if (chunkData) {
		delete chunkData;
		chunkData = NULL;
	}
}
//-----------------------------------------------------------------------------
void VstForxPlug::process(float **in, float **out, int numSamples) {
	if ( !graph ) 
		return;
	::processing::Frames fr ( in, numSamples ); 
	TRY_TO_LOCK_TIMED2 ( graph->getProcessingLock(), 30 );
	if ( !graph->getEndNode()->isActive() ){
		fr.setZero ( numSamples );
		fr.getBlock ( out, numSamples );
		return;
	} 
	graph->pushAndCopy ( &fr, numSamples );
	graph->processGraph( out, numSamples  );
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
void VstForxPlug::hostParameterChanged(void *src, float value, int index) {
	using ::processing::parameter::Parameter;
	Parameter *p = (Parameter*) src;
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
	EventSender<HostIOChanged>::notifyListeners(this, HostIOChanged());
	return true;
}
//-----------------------------------------------------------------------------
TimeInfo * VstForxPlug::getHostTimeInfo (int filter) {
	return getHost()->getHostTimeInfo(filter);
}
//-----------------------------------------------------------------------------
VstForxPlug::HostIOChangedConnection 
VstForxPlug::addHostChangedListener(const HostIOChangedFunction &f) {
	return EventSender<HostIOChanged>::addEventListener(f);
}
//-----------------------------------------------------------------------------
VstForxPlug::HostIOChangedConnection 
VstForxPlug::addTrackedHostChangedListener(const HostIOChangedFunction &f, AnyWPtr wptr)
{
	return EventSender<HostIOChanged>::addTrackedEventListener(f, wptr);
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
	graph->getJanitor()->hostBaseConfigChanged();
}
//-----------------------------------------------------------------------------
void VstForxPlug::requestEditorResize(int width, int height) {
	getHost()->requestEditorResize(width, height);
}
//-----------------------------------------------------------------------------
int VstForxPlug::getChunk(void **data) {
	try {
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
		std::cout<<datasize<<" bytes saved."<<std::endl;
		return datasize;
	} catch(const std::exception &ex) {
		std::stringstream ss;
		ss<<"serialization failed: "<<ex.what();
		::com::osMessageBox("Error", ss.str(), ::com::MSG_ALERT);
		return 0;
	} catch(...) {
		std::stringstream ss;
		ss<<"serialization failed: unkown reason.";
		::com::osMessageBox("Error", ss.str(), ::com::MSG_ALERT);
		return 0;
	}
}
//-----------------------------------------------------------------------------
int VstForxPlug::setChunk(void *data, int byteSize) {
	if (byteSize==0) {
		return 0;
	}
	try {
		std::stringstream ss;
		std::string dataStr((char*)data, byteSize);
		ss<<dataStr;
		load(ss);
		std::cout<<byteSize<<" bytes loaded."<<std::endl;
		return byteSize;
	} catch(const std::exception &ex) {
		std::stringstream ss;
		ss<<"serialization failed: "<<ex.what();
		::com::osMessageBox("Error", ss.str(), ::com::MSG_ALERT);
		return 0;
	} catch(...) {
		std::stringstream ss;
		ss<<"serialization failed: unkown reason.";
		::com::osMessageBox("Error", ss.str(), ::com::MSG_ALERT);
		return 0;
	}
}
//-----------------------------------------------------------------------------
void VstForxPlug::saveEditor(::com::oArchive &ar) {
	using frx::gui::components::VstForxEditor;
	VstForxEditor * editor = dynamic_cast<VstForxEditor*>(
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
	if (editor->isOpen()) {
		//void serializeViewTemp(::com::oArchive &ar, FrxCircuidViewPtr view);
		::com::oArchive tmp(tmpss);
		frx::gui::components::register_types(tmp);
		editor->serializeViewTemp(tmp, editor->getCircuidView()); // as of now map is locked;
		serializedViewStream = tmpss.str();
	} else {
		serializedViewStream = editor->hiChamber.str();
	}
	frx::gui::components::register_types(ar);
	ar.register_type<frx::gui::ViewModelMap>();
	ar & serializedViewStream; //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<1.
	ar & map;				   //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<2.
	
	if (tmpss.str().length() > 0) {
		::com::iArchive tmp(tmpss);
		frx::gui::components::register_types(tmp);
		getViewModelMap()->unlock(tmp);
	}
}
//-----------------------------------------------------------------------------
void VstForxPlug::loadEditor(::com::iArchive &ar) {
	using frx::gui::components::VstForxEditor;
	VstForxEditor * editor = dynamic_cast<VstForxEditor*>(
		host->getEditor()
	);
	if (!editor) {
		SAMBAG_THROW(
			sambag::com::exceptions::IllegalStateException,
			"editor == NULL"
		);
	}
	frx::gui::components::register_types(ar);
	ar.register_type<frx::gui::ViewModelMap>();
	std::string serializedViewStream;
	ar & serializedViewStream; //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<1.
	ar & map;				   //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<2.
	if (editor->isOpen()) {
		std::stringstream tmpss;
		tmpss<<serializedViewStream;
		::com::iArchive tmp(tmpss);
		frx::gui::components::register_types(tmp);
		frx::gui::components::FrxCircuidViewPtr view =
			editor->deserializeViewTemp(tmp);
		editor->setCircuidView(view);
		return;
	}
	editor->hiChamber.str("");
	editor->hiChamber.clear();
	editor->hiChamber << serializedViewStream;
}
//-----------------------------------------------------------------------------
void VstForxPlug::save(std::ostream &os) {
	::com::oArchive ar(os);
	ar.register_type<HostInfoAdapter>();
	register_types(ar);
	ar & hostInfoAdapter;
	ar & graph;
	saveEditor(ar);
}
//-----------------------------------------------------------------------------
void VstForxPlug::load(std::istream &is) {
	
	::processing::Graph::Ptr alt = graph; // hold old until loosing scope
	::com::iArchive ar(is);
	ar.register_type<HostInfoAdapter>();
	register_types(ar);
	ar & hostInfoAdapter;
	dynamic_cast<HostInfoAdapter*>
		(hostInfoAdapter.get())->hostInfo = this;
	ar & graph;
	ctrl->setGraph(graph);
	loadEditor(ar);
	// reinit graph
	::processing::Graph::Janitor::Ptr janitor = graph->getJanitor();
	if ( sampleRate != 0.0 && blockSize != 0 ) {
		janitor->hostBaseConfigChanged();
	}
	initHostParameter();
}
//-----------------------------------------------------------------------------
sambag::dsp::IEditor * VstForxPlug::getEditor() {
	return host->getEditor();
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