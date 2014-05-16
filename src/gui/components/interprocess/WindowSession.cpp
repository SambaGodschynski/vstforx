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
    neu->startProcessThread();
    return neu;
}
//-----------------------------------------------------------------------------
void WindowSessionHost::onOpen() {
    typedef SessionHost::Operations::OnOpen Op;
    MemoryGuard::Ptr g = getMemoryGuard();
    waitForProcess(SessionHost::OpcM::getOPC<Op>(), g);
}
//-----------------------------------------------------------------------------
void WindowSessionHost::onClose() {
    typedef SessionHost::Operations::OnClose Op;
    MemoryGuard::Ptr g = getMemoryGuard();
    waitForProcess(SessionHost::OpcM::getOPC<Op>(), g);
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
    res->startProcessThread();
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
    MemoryGuard::Ptr g = getMemoryGuard();
    waitForProcess(SessionHost::OpcM::getOPC<Op>(), g);
}
//-----------------------------------------------------------------------------
void WindowSessionClient::close() {
    typedef SessionHost::Operations::Close Op;
    MemoryGuard::Ptr g = getMemoryGuard();
    waitForProcess(SessionHost::OpcM::getOPC<Op>(), g);
}
//-----------------------------------------------------------------------------
void WindowSessionClient::setBounds(const sd::Rectangle &r) {
    typedef SessionHost::Operations::SetBounds Op;
    MemoryGuard::Ptr g = getMemoryGuard();
    Op::ArgPtr args = g->getArg<Op::Arg>();
    args->bounds = r;
    waitForProcess(SessionHost::OpcM::getOPC<Op>(), g);
}
//-----------------------------------------------------------------------------
void * WindowSessionClient::getSystemHandle () {
    return NULL;
}
//-----------------------------------------------------------------------------
void WindowSessionClient::setSize (const sd::Dimension &d) {
    typedef SessionHost::Operations::SetSize Op;
    MemoryGuard::Ptr g = getMemoryGuard();
    Op::ArgPtr args = g->getArg<Op::Arg>();
    args->size = d;
    waitForProcess(SessionHost::OpcM::getOPC<Op>(), g);
}
//-----------------------------------------------------------------------------
void WindowSessionClient::setLocation (const sd::Point2D &p) {
    typedef SessionHost::Operations::SetLocation Op;
    MemoryGuard::Ptr g = getMemoryGuard();
    Op::ArgPtr args = g->getArg<Op::Arg>();
    args->loc = p;
    waitForProcess(SessionHost::OpcM::getOPC<Op>(), g);
}
//-----------------------------------------------------------------------------
sd::Rectangle WindowSessionClient::getBounds () const {
    typedef SessionHost::Operations::GetBounds Op;
    MemoryGuard::Ptr g = getMemoryGuard();
    Op::RetPtr rets = g->getRet<Op::Ret>();
    waitForResult(SessionHost::OpcM::getOPC<Op>(), g);
    return rets->bounds;
}
//-----------------------------------------------------------------------------
sd::Rectangle WindowSessionClient::getHostBounds () const {
    typedef SessionHost::Operations::GetHostBounds Op;
    MemoryGuard::Ptr g = getMemoryGuard();
    Op::RetPtr rets = g->getRet<Op::Ret>();
    waitForResult(SessionHost::OpcM::getOPC<Op>(), g);
    return rets->bounds;
}
//-----------------------------------------------------------------------------
sd::Dimension WindowSessionClient::getSize () const {
    typedef SessionHost::Operations::GetSize Op;
    MemoryGuard::Ptr g = getMemoryGuard();
    Op::RetPtr rets = g->getRet<Op::Ret>();
    waitForResult(SessionHost::OpcM::getOPC<Op>(), g);
    return rets->size;
}
//-----------------------------------------------------------------------------
sd::Point2D WindowSessionClient::getLocation () const {
    typedef SessionHost::Operations::GetLocation Op;
    MemoryGuard::Ptr g = getMemoryGuard();
    Op::RetPtr rets = g->getRet<Op::Ret>();
    waitForResult( SessionHost::OpcM::getOPC<Op>(), g);
    return rets->loc;
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
    MemoryGuard::Ptr g = getMemoryGuard();
    Op::RetPtr rets = g->getRet<Op::Ret>();
    waitForResult(SessionHost::OpcM::getOPC<Op>(), g);
    return rets->value;
}
//-----------------------------------------------------------------------------
void WindowSessionClient::setTitle (const std::string &title) {
    typedef SessionHost::Operations::SetTitle Op;
    MemoryGuard::Ptr g = getMemoryGuard();
    Op::ArgPtr args = g->getArg<Op::Arg>();
    fpi::shm_cpystr(args->title, title);
    waitForProcess(SessionHost::OpcM::getOPC<Op>(), g);
}
//-----------------------------------------------------------------------------
std::string WindowSessionClient::getTitle () const {
    typedef SessionHost::Operations::GetTitle Op;
    MemoryGuard::Ptr g = getMemoryGuard();
    Op::RetPtr rets = g->getRet<Op::Ret>();
    waitForResult(SessionHost::OpcM::getOPC<Op>(), g);
    return rets->title;
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
