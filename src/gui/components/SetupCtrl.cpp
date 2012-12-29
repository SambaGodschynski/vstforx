/*
 * SetupCtrl.cpp
 *
 *  Created on: Thu Oct 25 13:56:01 2012
 *      Author: Johannes Unger
 */

#include "SetupCtrl.hpp"
#include <com/Settings.h>
#include <boost/foreach.hpp>
#include <com/PluginCollection.h>
#include <sambag/com/exceptions/IllegalStateException.hpp>
#include <boost/thread.hpp>
#include <gui/components/FrxCircuidView.hpp>
#include <sambag/disco/components/Window.hpp>

namespace com {
extern std::string osSelectDirectory(const std::string &wndTitle, 
						    const std::string &startPath);
} // namespace com

namespace frx { namespace gui { namespace components {
//=============================================================================
//  Class SetupCtrl
//=============================================================================
//-----------------------------------------------------------------------------
void SetupCtrl::setHostInfo(::frx::processing::IHostInfo::Ptr hI) {
	hostInfo = hI;
}
//-----------------------------------------------------------------------------
std::string SetupCtrl::selectDirectory(const std::string &startDir) const {
	return ::com::osSelectDirectory("select directory", startDir);
}
//-----------------------------------------------------------------------------
bool SetupCtrl::addPluginFolder(const std::string &path) {
	return ::com::getSettings().addVSTFolder(path);
}
//-----------------------------------------------------------------------------
void SetupCtrl::setView(FrxCircuidViewPtr view) {
	this->view = view;
}
//-----------------------------------------------------------------------------
bool SetupCtrl::removePluginFolder(const std::string &path) {
	return ::com::getSettings().removeVSTFolder(path);
}
//-----------------------------------------------------------------------------
size_t SetupCtrl::getNumPluginFolder() const {
	return ::com::getSettings().getPluginDirectoryList().size();
}
//-----------------------------------------------------------------------------
void SetupCtrl::getPluginFolders(std::list<std::string> &out) {
	BOOST_FOREACH(const std::string &str, 
		::com::getSettings().getPluginDirectoryList())
	{
		out.push_back(str);
	}
}
//-----------------------------------------------------------------------------
void SetupCtrl::saveSettings() {
	::com::getSettings().saveConfigFile();
}
//-----------------------------------------------------------------------------
namespace {
boost::thread scanThread;
typedef SetupCtrl::NotifyFileFunc FileEvF;
typedef SetupCtrl::ScanCompletedFunc ScanComplF;
void onLoadFile(void *src, const ::com::OnLoadFile &ev, const FileEvF &f) 
{
	std::cout<<ev.filename<<"...";
	f(ev.filename, SetupCtrl::OnOpening);
}
void onFileLoaded(void *src, const ::com::OnFileLoaded &ev, const FileEvF &f) 
{
	std::cout<<(ev.info.access == ::processing::PluginInfo::SUCCEED
		? "suceed" : "failed") <<std::endl;
	SetupCtrl::FileStatus s;
	switch (ev.info.access) {
		case ::processing::PluginInfo::SUCCEED:
			s = SetupCtrl::Succeed; break;
		case ::processing::PluginInfo::FAILED:
			s = SetupCtrl::Failed; break;
		case ::processing::PluginInfo::NOT_CHECKED:
			s = SetupCtrl::Skipped; break;
	}
	f(ev.filename, s);
}
void startScanImpl(const FileEvF &f, 
	const ScanComplF &sccF, 
	frx::processing::IHostInfo::Ptr hostInfo) 
{
	typedef ::com::OnLoadFile OnFileLoading;
	typedef ::com::OnFileLoaded OnFileLoaded;
	typedef ::com::PluginCollection::EventSender<OnFileLoading> LoadingEvSender; 
	typedef ::com::PluginCollection::EventSender<OnFileLoaded> LoadedEvSender;
	typedef LoadingEvSender::EventConnection LoadEvConnection;
	typedef LoadedEvSender::EventConnection LoadedEvConnection;

	::com::PluginCollection &db = ::com::getPluginCollection();
	LoadEvConnection loadCn = db.LoadingEvSender::
		addEventListener(boost::bind(&onLoadFile, _1, _2, f));
	LoadedEvConnection loadedCn = db.LoadedEvSender::
		addEventListener(boost::bind(&onFileLoaded, _1, _2, f));
	// scan
	db.update(hostInfo);
	// disconnect
	loadCn.disconnect();
	loadedCn.disconnect();
	sccF(db.getNumSucceed(), db.getNumFailed(), db.getNumNotChecked());
}
} // namespace(s)
//-----------------------------------------------------------------------------
void SetupCtrl::startScan(const NotifyFileFunc &fileEventF, 
	const ScanCompletedFunc &scanCompletedF)
{
	if (!hostInfo) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException, 
			"Tried to start scan with hostInfo == NULL");
	}
	scanThread = boost::thread(
		boost::bind(&startScanImpl, fileEventF, scanCompletedF, hostInfo)	
	);
}
//-----------------------------------------------------------------------------
void SetupCtrl::joinScan() {
	scanThread.join();
}
//-----------------------------------------------------------------------------
void SetupCtrl::stopScanning() {
	::com::PluginCollection &db = ::com::getPluginCollection();
	db.stopScanning();
}
//-----------------------------------------------------------------------------
bool SetupCtrl::getBooleanValue(const std::string &key) const {
	return ::com::getSettings().getBooleanValue(key);
}
//-----------------------------------------------------------------------------
void SetupCtrl::setBooleanValue(const std::string &key, bool val) {
	return ::com::getSettings().setBooleanValue(key, val);
}
//-----------------------------------------------------------------------------
std::string SetupCtrl::getStringValue(const std::string &key) const {
	return ::com::getSettings().getStringValue(key);
}
//-----------------------------------------------------------------------------
void SetupCtrl::setStringValue(const std::string &key, const std::string &val){
	return ::com::getSettings().setStringValue(key, val);
}
//-----------------------------------------------------------------------------
int SetupCtrl::getIntegerValue(const std::string &key) const {
	return ::com::getSettings().getIntegerValue(key);
}
//-----------------------------------------------------------------------------
void SetupCtrl::setIntegerValue(const std::string &key, int val) {
	return ::com::getSettings().setIntegerValue(key, val);
}
//-----------------------------------------------------------------------------
sambag::disco::Dimension SetupCtrl::getEditorSize() const {
	using namespace sambag::disco::components;
	if (!view)
		return NULL_DIMENSION;
	Window::Ptr win = view->getFirstContainer<Window>();
	if (!win)
		return NULL_DIMENSION;
	return win->getWindowSize();
}
//-----------------------------------------------------------------------------
void SetupCtrl::setEditorSize(const sambag::disco::Dimension &size)
{
	view->requestEditorResize(size);
}
}}} // namespace(s)
