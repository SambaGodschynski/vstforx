/*
 * WindowSession.hpp
 *
 *  Created on: Tue Dec  3 13:30:20 2013
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_WINDOWSESSION_H
#define SAMBAG_WINDOWSESSION_H

#include <boost/shared_ptr.hpp>
#include <processing/interprocess/Session.hpp>
#include <processing/interprocess/ShmCom.hpp>
#include <sambag/disco/components/windowImpl/AWindowImpl.hpp>
#include <sambag/disco/components/Timer.hpp>
#include <gui/HandyNamespaces.hpp>
#include <sambag/com/Thread.hpp>

namespace frx { namespace gui { namespace components {
class FrxPluginEditor;
typedef boost::shared_ptr<FrxPluginEditor> FrxPluginEditorPtr;
namespace interprocess { // namespace interprocess
class WindowSessionClient;
namespace fpi = frx::processing::interprocess;
namespace sci = sambag::com::interprocess;
//=============================================================================
/** 
  * @class PluginSession.
  */
class WindowSessionHost : public fpi::Session {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<WindowSessionHost> Ptr;
	//-------------------------------------------------------------------------
	typedef boost::weak_ptr<WindowSessionHost> WPtr;
    //-------------------------------------------------------------------------
    typedef WindowSessionClient SessionHost; // host for session calls
    //-------------------------------------------------------------------------
    FRX_OP_BEGIN_OPERATIONS
        FRX_OP_OPERATION(Open, FRX_OP_ARG(), FRX_OP_RET());
        FRX_OP_OPERATION(Close, FRX_OP_ARG(), FRX_OP_RET());
        FRX_OP_OPERATION(SetBounds, FRX_OP_ARG_1(sd::Rectangle bounds), FRX_OP_RET());
        FRX_OP_OPERATION(GetBounds, FRX_OP_ARG(), FRX_OP_RET_1(sd::Rectangle bounds));
        FRX_OP_OPERATION(GetHostBounds, FRX_OP_ARG(), FRX_OP_RET_1(sd::Rectangle bounds));
        FRX_OP_OPERATION(SetSize, FRX_OP_ARG_1(sd::Dimension size), FRX_OP_RET());
        FRX_OP_OPERATION(GetSize, FRX_OP_ARG(), FRX_OP_RET_1(sd::Dimension size));
        FRX_OP_OPERATION(SetLocation, FRX_OP_ARG_1(sd::Point2D loc), FRX_OP_RET());
        FRX_OP_OPERATION(GetLocation, FRX_OP_ARG(), FRX_OP_RET_1(sd::Point2D loc));
        FRX_OP_OPERATION(IsVisible, FRX_OP_ARG(), FRX_OP_RET_1(bool value));
        FRX_OP_OPERATION(SetTitle, FRX_OP_ARG_1(char title[FRX_SHMSESS_MAX_STR_LENGTH]), FRX_OP_RET());
        FRX_OP_OPERATION(GetTitle, FRX_OP_ARG(), FRX_OP_RET_1(char title[FRX_SHMSESS_MAX_STR_LENGTH]));
        //---------------------------------------------------------------------
        typedef LOKI_TYPELIST_12(Open,
            Close,
            SetBounds,
            GetBounds,
            GetHostBounds,
            GetSize,
            SetSize,
            GetLocation,
            SetLocation,
            IsVisible,              // 10
            SetTitle,
            GetTitle
        ) OPs;
    FRX_OP_END_OPERATIONS_AND_IMPL_PROCESS(OPs)
    //-------------------------------------------------------------------------
    WindowSessionHost();
private:
    //-------------------------------------------------------------------------
    sdc::Timer::Ptr idleTimer;
    //-------------------------------------------------------------------------
    FrxPluginEditorPtr window;
protected:
    //-------------------------------------------------------------------------
    void onOpen();
    //-------------------------------------------------------------------------
    void onClose();
    //-------------------------------------------------------------------------
    WPtr self;
public:
    //-------------------------------------------------------------------------
    sdc::Timer::Ptr getIdleTimer() const {
        return idleTimer;
    }
    //-------------------------------------------------------------------------
    FrxPluginEditorPtr getWindow();
    //-------------------------------------------------------------------------
    /**
     * creates new session
     */
    static Ptr create();
    //-------------------------------------------------------------------------
    ///////////////////////////////////////////////////////////////////////////
    //-------------------------------------------------------------------------
    FRX_OP_CALLBACK_METHOD(Open);
    FRX_OP_CALLBACK_METHOD(Close);
    FRX_OP_CALLBACK_METHOD(SetBounds);
    FRX_OP_CALLBACK_METHOD(GetBounds);
    FRX_OP_CALLBACK_METHOD(IsVisible);
    FRX_OP_CALLBACK_METHOD(SetTitle);
    FRX_OP_CALLBACK_METHOD(GetTitle);
    FRX_OP_CALLBACK_METHOD(GetHostBounds);
    FRX_OP_CALLBACK_METHOD(GetSize);
    FRX_OP_CALLBACK_METHOD(SetSize);
    FRX_OP_CALLBACK_METHOD(GetLocation);
    FRX_OP_CALLBACK_METHOD(SetLocation);
};
//=============================================================================
/** 
  * @class WindowSessionClient.
  */
class WindowSessionClient : public fpi::Session,
    public sdc::AWindowImpl
{
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<WindowSessionClient> Ptr;
	//-------------------------------------------------------------------------
	typedef boost::weak_ptr<WindowSessionClient> WPtr;
   //-------------------------------------------------------------------------
    typedef WindowSessionHost SessionHost; // host for session calls
    //-------------------------------------------------------------------------
    FRX_OP_BEGIN_OPERATIONS
        FRX_OP_OPERATION(OnOpen, FRX_OP_ARG(), FRX_OP_RET());
        FRX_OP_OPERATION(OnClose, FRX_OP_ARG(), FRX_OP_RET());
        //---------------------------------------------------------------------
        typedef LOKI_TYPELIST_2(OnOpen,
            OnClose
        ) OPs;
    FRX_OP_END_OPERATIONS_AND_IMPL_PROCESS(OPs)
    FRX_OP_CALLBACK_METHOD(OnOpen);
    FRX_OP_CALLBACK_METHOD(OnClose);
private:
protected:
    //-------------------------------------------------------------------------
    WPtr self;
    //-------------------------------------------------------------------------
    WindowSessionClient(const std::string &id);
    //-------------------------------------------------------------------------
    /**
     * @brief since Session channel communication can be blocked when
     * both channels are in use we notify events delayed to prevent running
     * into mutual blocking. 
     */
    template <class Event>
    void doSendEvent() {
        sce::EventSender<Event>::notifyListeners(this, Event());
    }
public:
    //-------------------------------------------------------------------------
    /**
     * @brief creates new session
     */
    static Ptr create(const std::string &id);
    //-------------------------------------------------------------------------
    ///////////////////////////////////////////////////////////////////////////
    virtual sce::EventSender< sdc::events::MouseEvent > * getMouseEventCreator ();
    virtual void open();
    virtual void close();
    virtual void setBounds(const sd::Rectangle &r);
    virtual void * getSystemHandle ();
    virtual void setSize (const sd::Dimension &d);
    virtual void setLocation (const sd::Point2D &p);
    virtual sd::Rectangle getBounds () const;
    virtual sd::Rectangle getHostBounds () const;
    virtual sd::Dimension getSize () const;
    virtual sd::Point2D getLocation () const;
    virtual void setEnabled (bool b);
    virtual bool isEnabled () const;
    virtual void invalidateWindow (const sd::Rectangle &area=NULL_RECTANGLE);
    virtual void setRootPane (sdc::RootPanePtr root);
    virtual void setParentWindow (sdc::WindowPtr parent);
    virtual void setFlag (sdc::WindowFlags::Flag flag, bool val);
    virtual bool getFlag (sdc::WindowFlags::Flag flag) const;
    virtual void setFlags(unsigned int flags);
    virtual bool isVisible () const;
    virtual void setTitle (const std::string &title);
    virtual std::string getTitle () const;
	sambag::com::ThreadId getThreadId() const { return sambag::com::ThreadId(); }
    // AWindowImpl
};
}}}} // namespace(s)

#endif /* SAMBAG_WINDOWSESSION_H */
