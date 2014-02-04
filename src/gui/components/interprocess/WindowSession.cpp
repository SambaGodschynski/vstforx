/*
 * WindowSession.cpp
 *
 *  Created on: Tue Dec  3 13:30:20 2013
 *      Author: Johannes Unger
 */

#include "WindowSession.hpp"
#include <sambag/com/exceptions/IllegalArgumentException.hpp>
#include <algorithm>
#include <processing/interprocess/SessionManager.hpp>
#include <sambag/disco/components/FramedWindow.hpp>
#include <gui/components/FrxPluginEditor.hpp>
#include <sambag/disco/components/Timer.hpp>
#include <sambag/disco/components/WindowToolkit.hpp>


namespace frx { namespace gui { namespace components { namespace interprocess {
//=============================================================================
//  Class WindowSessionHost
//=============================================================================
//-----------------------------------------------------------------------------
WindowSessionHost::WindowSessionHost()
    :  fpi::Session( "ws-"+fpi::SessionManager::createUniqueName(),
                ChannelSize(OpcM::MaxArgmemSize, OpcM::MaxRetmemSize),
                ChannelSize(OpcM::MaxArgmemSize, OpcM::MaxRetmemSize)
        )
{
}
//-----------------------------------------------------------------------------
WindowSessionHost::Ptr WindowSessionHost::create() {
    Ptr neu = Ptr( new WindowSessionHost() );
    neu->self=neu;
    return neu;
}
//-----------------------------------------------------------------------------
void WindowSessionHost::onOpen() {
    typedef SessionHost::Operations::OnOpen Op;
    waitForProcess( SessionHost::OpcM::getOPC<Op>() );
}
//-----------------------------------------------------------------------------
void WindowSessionHost::onClose() {
    typedef SessionHost::Operations::OnClose Op;
    waitForProcess( SessionHost::OpcM::getOPC<Op>() );
    idleTimer->stop();
    window.reset();
    idleTimer.reset();
}
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
FrxPluginEditorPtr WindowSessionHost::getWindow() {
    if (window) {
        return window;
    }
    window = FrxPluginEditor::create();
    window->getWindowImpl()->sce::EventSender<sdc::OnCloseEvent>::addTrackedEventListener(
        boost::bind(&WindowSessionHost::onClose, this),
        self
    );
    window->getWindowImpl()->sce::EventSender<sdc::OnOpenEvent>::addTrackedEventListener(
        boost::bind(&WindowSessionHost::onOpen, this),
        self
    );
    idleTimer = sdc::Timer::create(25);
    idleTimer->setNumRepetitions(-1);
    idleTimer->start();
    return window;
}
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(WindowSessionHost, Open) {
    getWindow()->open();
}
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(WindowSessionHost, Close) {
    getWindow()->close();
}
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(WindowSessionHost, SetBounds) {
    getWindow()->setWindowBounds( arg->bounds );
}
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(WindowSessionHost, GetBounds) {
    ret->bounds = getWindow()->getWindowBounds();
}
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(WindowSessionHost, IsVisible) {
    ret->value = getWindow()->isVisible();
}
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(WindowSessionHost, SetTitle) {
    getWindow()->setTitle(arg->title);
}
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(WindowSessionHost, GetTitle) {
    fpi::shm_cpystr(ret->title, getWindow()->getTitle());
}
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(WindowSessionHost, GetHostBounds) {
    ret->bounds = getWindow()->getWindowImpl()->getHostBounds();
}
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(WindowSessionHost, GetSize) {
    ret->size = getWindow()->getWindowSize();
}
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(WindowSessionHost, SetSize) {
    getWindow()->setWindowSize( arg->size );
}
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(WindowSessionHost, GetLocation) {
    ret->loc = getWindow()->getWindowLocation();
}
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(WindowSessionHost, SetLocation) {
    getWindow()->setWindowLocation( arg->loc );
}
//=============================================================================
//  Class WindowSessionClient
//=============================================================================
//-----------------------------------------------------------------------------
WindowSessionClient::WindowSessionClient(const std::string &id) : Session(id) {
}
//-----------------------------------------------------------------------------
WindowSessionClient::Ptr WindowSessionClient::create(const std::string &id) {
    Ptr res = Ptr( new WindowSessionClient(id) );
    res->self = res;
    return res;
}
//-----------------------------------------------------------------------------
sce::EventSender< sdc::events::MouseEvent > *
WindowSessionClient::getMouseEventCreator ()
{
    return NULL;
}
//-----------------------------------------------------------------------------
void WindowSessionClient::open() {
    typedef SessionHost::Operations::Open Op;
    waitForProcess(SessionHost::OpcM::getOPC<Op>());
}
//-----------------------------------------------------------------------------
void WindowSessionClient::close() {
    typedef SessionHost::Operations::Close Op;
    waitForProcess(SessionHost::OpcM::getOPC<Op>());
}
//-----------------------------------------------------------------------------
void WindowSessionClient::setBounds(const sd::Rectangle &r) {
    typedef SessionHost::Operations::SetBounds Op;
    Op::Arg arg;
    arg.bounds = r;
    waitForProcess(SessionHost::OpcM::getOPC<Op>(), arg);
}
//-----------------------------------------------------------------------------
void * WindowSessionClient::getSystemHandle () {
    return NULL;
}
//-----------------------------------------------------------------------------
void WindowSessionClient::setSize (const sd::Dimension &d) {
    typedef SessionHost::Operations::SetSize Op;
    Op::Arg arg;
    arg.size = d;
    waitForProcess(SessionHost::OpcM::getOPC<Op>(), arg);
}
//-----------------------------------------------------------------------------
void WindowSessionClient::setLocation (const sd::Point2D &p) {
    typedef SessionHost::Operations::SetLocation Op;
    Op::Arg arg;
    arg.loc = p;
    waitForProcess(SessionHost::OpcM::getOPC<Op>(), arg);

}
//-----------------------------------------------------------------------------
sd::Rectangle WindowSessionClient::getBounds () const {
    typedef SessionHost::Operations::GetBounds Op;
    Op::Ret ret;
    waitForResult(SessionHost::OpcM::getOPC<Op>(), ret);
    return ret.bounds;
}
//-----------------------------------------------------------------------------
sd::Rectangle WindowSessionClient::getHostBounds () const {
    typedef SessionHost::Operations::GetHostBounds Op;
    Op::Ret ret;
    waitForResult(SessionHost::OpcM::getOPC<Op>(), ret);
    return ret.bounds;
}
//-----------------------------------------------------------------------------
sd::Dimension WindowSessionClient::getSize () const {
    typedef SessionHost::Operations::GetSize Op;
    Op::Ret ret;
    waitForResult(SessionHost::OpcM::getOPC<Op>(), ret);
    return ret.size;
}
//-----------------------------------------------------------------------------
sd::Point2D WindowSessionClient::getLocation () const {
    typedef SessionHost::Operations::GetLocation Op;
    Op::Ret ret;
    waitForResult( SessionHost::OpcM::getOPC<Op>(), ret);
    return ret.loc;
}
//-----------------------------------------------------------------------------
void WindowSessionClient::setEnabled (bool b) {
}
//-----------------------------------------------------------------------------
bool WindowSessionClient::isEnabled () const {
    return true;
}
//-----------------------------------------------------------------------------
void WindowSessionClient::invalidateWindow (const sd::Rectangle &area) {
}
//-----------------------------------------------------------------------------
void WindowSessionClient::setRootPane (sdc::RootPanePtr root) {
}
//-----------------------------------------------------------------------------
void WindowSessionClient::setParentWindow (sdc::WindowPtr parent) {
}
//-----------------------------------------------------------------------------
void WindowSessionClient::setFlag (sdc::WindowFlags::Flag flag, bool val) {
}
//-----------------------------------------------------------------------------
bool WindowSessionClient::getFlag (sdc::WindowFlags::Flag flag) const {
    return false;
}
//-----------------------------------------------------------------------------
bool WindowSessionClient::isVisible () const {
    typedef SessionHost::Operations::IsVisible Op;
    Op::Ret ret;
    waitForResult(SessionHost::OpcM::getOPC<Op>(), ret);
    return ret.value;
}
//-----------------------------------------------------------------------------
void WindowSessionClient::setTitle (const std::string &title) {
    typedef SessionHost::Operations::SetTitle Op;
    Op::Arg arg;
    fpi::shm_cpystr(arg.title, title);
    waitForProcess(SessionHost::OpcM::getOPC<Op>(), arg);
}
//-----------------------------------------------------------------------------
std::string WindowSessionClient::getTitle () const {
    typedef SessionHost::Operations::GetTitle Op;
    Op::Ret ret;
    waitForResult(SessionHost::OpcM::getOPC<Op>(), ret);
    return ret.title;
}
//-----------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
FRX_OP_CALLBACK_METHOD_IMPL(WindowSessionClient, OnOpen) {
    sdc::getWindowToolkit()->invokeLater(
        boost::bind(&WindowSessionClient::doSendEvent<sdc::OnOpenEvent>, this)
    );
}
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(WindowSessionClient, OnClose) {
    sdc::getWindowToolkit()->invokeLater(
        boost::bind(&WindowSessionClient::doSendEvent<sdc::OnCloseEvent>, this)
    );
}
}}}} // namespace(s)
