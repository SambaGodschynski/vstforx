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
    window = FrxPluginEditor::create();
    window->getWindowImpl()->sce::EventSender<sdc::OnCloseEvent>::addEventListener(
        boost::bind(&WindowSessionHost::onClose, this)
    );
    window->getWindowImpl()->sce::EventSender<sdc::OnOpenEvent>::addEventListener(
        boost::bind(&WindowSessionHost::onOpen, this)
    );
}
//-----------------------------------------------------------------------------
WindowSessionHost::Ptr WindowSessionHost::create() {
    Ptr neu = Ptr( new WindowSessionHost() );
    return neu;
}
//-----------------------------------------------------------------------------
void WindowSessionHost::onOpen() {
    typedef SessionHost::Operations::OnOpen Op;
    waitForResult( SessionHost::OpcM::getOPC<Op>() );
}
//-----------------------------------------------------------------------------
void WindowSessionHost::onClose() {
    typedef SessionHost::Operations::OnClose Op;
    waitForResult( SessionHost::OpcM::getOPC<Op>() );
}
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(WindowSessionHost, Open) {
    window->open();
}
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(WindowSessionHost, Close) {
    window->close();
}
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(WindowSessionHost, SetBounds) {
    window->setWindowBounds( arg->bounds );
}
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(WindowSessionHost, GetBounds) {
    ret->bounds = window->getWindowBounds();
}
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(WindowSessionHost, IsVisible) {
    ret->value = window->isVisible();
}
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(WindowSessionHost, SetTitle) {
    window->setTitle(arg->title);
}
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(WindowSessionHost, GetTitle) {
    fpi::shm_cpystr(ret->title, window->getTitle());
}
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(WindowSessionHost, GetHostBounds) {
    ret->bounds = window->getWindowImpl()->getHostBounds();
}
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(WindowSessionHost, GetSize) {
    ret->size = window->getWindowSize();
}
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(WindowSessionHost, SetSize) {
    window->setWindowSize( arg->size );
}
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(WindowSessionHost, GetLocation) {
    ret->loc = window->getWindowLocation();
}
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(WindowSessionHost, SetLocation) {
    window->setWindowLocation( arg->loc );
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
    waitForResult<Op::RetPtr>( SessionHost::OpcM::getOPC<Op>());
}
//-----------------------------------------------------------------------------
void WindowSessionClient::close() {
    typedef SessionHost::Operations::Close Op;
    waitForResult<Op::RetPtr>( SessionHost::OpcM::getOPC<Op>());
}
//-----------------------------------------------------------------------------
void WindowSessionClient::setBounds(const sd::Rectangle &r) {
    typedef SessionHost::Operations::SetBounds Op;
    Op::ArgPtr arg = static_cast<Op::ArgPtr>( getArgmem() );
    arg->bounds = r;
    waitForResult<Op::RetPtr>( SessionHost::OpcM::getOPC<Op>());
}
//-----------------------------------------------------------------------------
void * WindowSessionClient::getSystemHandle () {
    return NULL;
}
//-----------------------------------------------------------------------------
void WindowSessionClient::setSize (const sd::Dimension &d) {
    typedef SessionHost::Operations::SetSize Op;
    Op::ArgPtr arg = static_cast<Op::ArgPtr>( getArgmem() );
    arg->size = d;
    waitForResult<Op::RetPtr>( SessionHost::OpcM::getOPC<Op>());
}
//-----------------------------------------------------------------------------
void WindowSessionClient::setLocation (const sd::Point2D &p) {
    typedef SessionHost::Operations::SetLocation Op;
    Op::ArgPtr arg = static_cast<Op::ArgPtr>( getArgmem() );
    arg->loc = p;
    waitForResult<Op::RetPtr>( SessionHost::OpcM::getOPC<Op>());

}
//-----------------------------------------------------------------------------
sd::Rectangle WindowSessionClient::getBounds () const {
    typedef SessionHost::Operations::GetBounds Op;
    Op::RetPtr ret = waitForResult<Op::RetPtr>( SessionHost::OpcM::getOPC<Op>());
    return ret->bounds;
}
//-----------------------------------------------------------------------------
sd::Rectangle WindowSessionClient::getHostBounds () const {
    typedef SessionHost::Operations::GetHostBounds Op;
    Op::RetPtr ret = waitForResult<Op::RetPtr>( SessionHost::OpcM::getOPC<Op>());
    return ret->bounds;
}
//-----------------------------------------------------------------------------
sd::Dimension WindowSessionClient::getSize () const {
    typedef SessionHost::Operations::GetSize Op;
    Op::RetPtr ret = waitForResult<Op::RetPtr>( SessionHost::OpcM::getOPC<Op>());
    return ret->size;
}
//-----------------------------------------------------------------------------
sd::Point2D WindowSessionClient::getLocation () const {
    typedef SessionHost::Operations::GetLocation Op;
    Op::RetPtr ret = waitForResult<Op::RetPtr>( SessionHost::OpcM::getOPC<Op>());
    return ret->loc;
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
    Op::RetPtr ret = waitForResult<Op::RetPtr>( SessionHost::OpcM::getOPC<Op>());
    return ret->value;
}
//-----------------------------------------------------------------------------
void WindowSessionClient::setTitle (const std::string &title) {
    typedef SessionHost::Operations::SetTitle Op;
    Op::ArgPtr arg = static_cast<Op::ArgPtr>( getArgmem() );
    fpi::shm_cpystr(arg->title, title);
    waitForResult<Op::RetPtr>( SessionHost::OpcM::getOPC<Op>());
}
//-----------------------------------------------------------------------------
std::string WindowSessionClient::getTitle () const {
    typedef SessionHost::Operations::GetTitle Op;
    Op::RetPtr ret = waitForResult<Op::RetPtr>( SessionHost::OpcM::getOPC<Op>());
    return ret->title;
}
//-----------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
FRX_OP_CALLBACK_METHOD_IMPL(WindowSessionClient, OnOpen) {
    sdc::Timer::Ptr tm = sdc::Timer::create(50);
    tm->sce::EventSender<sdc::TimerEvent>::addTrackedEventListener(
        boost::bind(&WindowSessionClient::doSendEvent<sdc::OnOpenEvent>, this),
        self
    );
    tm->start();
}
//-----------------------------------------------------------------------------
FRX_OP_CALLBACK_METHOD_IMPL(WindowSessionClient, OnClose) {
    sdc::Timer::Ptr tm = sdc::Timer::create(50);
    tm->sce::EventSender<sdc::TimerEvent>::addTrackedEventListener(
        boost::bind(&WindowSessionClient::doSendEvent<sdc::OnCloseEvent>, this),
        self
    );
    tm->start();
}
}}}} // namespace(s)
