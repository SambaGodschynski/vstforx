#pragma once

#include <atomic>
#include <cstring>

#include "pluginterfaces/gui/iplugview.h"
#include "gui/components/VstForxEditor.hpp"
#include <processing/VstForxPlug.hpp>

#ifndef _WIN32
#include <sambag/disco/components/windowImpl/X11WindowToolkit.hpp>
#endif

namespace frx { namespace vst3 {

// ─────────────────────────────────────────────────────────────────────────────
// IPlugView wrapping VstForxEditor.
//
// On Linux, also pumps X11 events via IRunLoop so the embedded window renders.
// Bitwig (and most Linux hosts) pass the X11 Window XID directly as void*
// (not as uint64_t* per VST3 spec), so we store it as unsigned long and pass
// a pointer to it — sambag's initAsNestedWindow dereferences it.
// ─────────────────────────────────────────────────────────────────────────────
class VstForxPlugView : public Steinberg::IPlugView {
    frx::gui::components::VstForxEditor* _editor;
    Steinberg::IPlugFrame*               _frame{nullptr};
    std::atomic<Steinberg::uint32>       _refCount{1};
    unsigned long                        _xid{0};

#ifndef _WIN32
    // ── Linux: minimal IEventHandler / ITimerHandler wrappers ────────────────
    // Separate objects avoid the multiple-FUnknown diamond; they have trivial
    // ref-counting because VstForxPlugView owns them.
    struct EventHandler : Steinberg::Linux::IEventHandler {
        VstForxPlugView* _view;
        explicit EventHandler(VstForxPlugView* v) : _view(v) {}
        void PLUGIN_API onFDIsSet(Steinberg::Linux::FileDescriptor) override {
            _view->pumpX11Events();
        }
        Steinberg::uint32 PLUGIN_API addRef()  override { return 1; }
        Steinberg::uint32 PLUGIN_API release() override { return 1; }
        Steinberg::tresult PLUGIN_API queryInterface(
                const Steinberg::TUID, void**) override {
            return Steinberg::kNoInterface;
        }
    };
    struct TimerHandler : Steinberg::Linux::ITimerHandler {
        VstForxPlugView* _view;
        explicit TimerHandler(VstForxPlugView* v) : _view(v) {}
        void PLUGIN_API onTimer() override {
            _view->redraw();
        }
        Steinberg::uint32 PLUGIN_API addRef()  override { return 1; }
        Steinberg::uint32 PLUGIN_API release() override { return 1; }
        Steinberg::tresult PLUGIN_API queryInterface(
                const Steinberg::TUID, void**) override {
            return Steinberg::kNoInterface;
        }
    };

    EventHandler                    _eventHandler{this};
    TimerHandler                    _timerHandler{this};
    Steinberg::Linux::IRunLoop*     _runLoop{nullptr};

    void pumpX11Events() {
        sambag::disco::components::X11WindowToolkit::processEvents();
    }

    void redraw() {
        sambag::disco::components::X11WindowToolkit::processEvents();
    }

    void registerRunLoop() {
        if (!_frame) return;
        _frame->queryInterface(Steinberg::Linux::IRunLoop::iid,
                               reinterpret_cast<void**>(&_runLoop));
        if (!_runLoop) return;
        using namespace sambag::disco::components;
        ::Display* dsp = X11WindowToolkit::getToolkit()->getGlobals().display;
        if (dsp)
            _runLoop->registerEventHandler(&_eventHandler, XConnectionNumber(dsp));
        _runLoop->registerTimer(&_timerHandler, 16); // ~60 fps
        sambag::com::BoostTimerImpl::startUpTimer();
    }

    void unregisterRunLoop() {
        if (!_runLoop) return;
        _runLoop->unregisterEventHandler(&_eventHandler);
        _runLoop->unregisterTimer(&_timerHandler);
        _runLoop->release();
        _runLoop = nullptr;
        sambag::com::BoostTimerImpl::tearDownTimer();
    }
#endif // !_WIN32

public:
    explicit VstForxPlugView(frx::processing::VstForxPlug* plugin)
        : _editor(new frx::gui::components::VstForxEditor(nullptr))
    {
        _editor->setPlugin(plugin);
    }

    ~VstForxPlugView() {
#ifndef _WIN32
        unregisterRunLoop();
#endif
        delete _editor;
    }

    // ── IUnknown ─────────────────────────────────────────────────────────────
    Steinberg::tresult PLUGIN_API queryInterface(
            const Steinberg::TUID iid, void** obj) override
    {
        if (std::memcmp(iid, Steinberg::IPlugView::iid,
                        sizeof(Steinberg::TUID)) == 0) {
            addRef();
            *obj = static_cast<Steinberg::IPlugView*>(this);
            return Steinberg::kResultOk;
        }
        *obj = nullptr;
        return Steinberg::kNoInterface;
    }
    Steinberg::uint32 PLUGIN_API addRef()  override { return ++_refCount; }
    Steinberg::uint32 PLUGIN_API release() override {
        auto r = --_refCount;
        if (r == 0) delete this;
        return r;
    }

    // ── IPlugView ─────────────────────────────────────────────────────────────
    Steinberg::tresult PLUGIN_API isPlatformTypeSupported(
            Steinberg::FIDString type) override
    {
#ifdef _WIN32
        if (std::strcmp(type, Steinberg::kPlatformTypeHWND) == 0)
            return Steinberg::kResultTrue;
#else
        if (std::strcmp(type, Steinberg::kPlatformTypeX11EmbedWindowID) == 0)
            return Steinberg::kResultTrue;
#endif
        return Steinberg::kResultFalse;
    }

    Steinberg::tresult PLUGIN_API attached(
            void* parent, Steinberg::FIDString /*type*/) override
    {
        _xid = reinterpret_cast<unsigned long>(parent);
        _editor->open(static_cast<void*>(&_xid));
#ifndef _WIN32
        registerRunLoop();
#endif
        return Steinberg::kResultOk;
    }

    Steinberg::tresult PLUGIN_API removed() override {
#ifndef _WIN32
        unregisterRunLoop();
#endif
        _editor->close();
        return Steinberg::kResultOk;
    }

    Steinberg::tresult PLUGIN_API getSize(Steinberg::ViewRect* size) override {
        if (!size) return Steinberg::kInvalidArgument;
        auto dim = _editor->getEditorSize();
        size->left   = 0;
        size->top    = 0;
        size->right  = static_cast<Steinberg::int32>(dim.width());
        size->bottom = static_cast<Steinberg::int32>(dim.height());
        return Steinberg::kResultOk;
    }

    Steinberg::tresult PLUGIN_API setFrame(Steinberg::IPlugFrame* f) override {
        _frame = f;
        return Steinberg::kResultOk;
    }

    Steinberg::tresult PLUGIN_API canResize() override { return Steinberg::kResultFalse; }
    Steinberg::tresult PLUGIN_API onSize(Steinberg::ViewRect*) override { return Steinberg::kResultFalse; }
    Steinberg::tresult PLUGIN_API onFocus(Steinberg::TBool) override { return Steinberg::kResultFalse; }
    Steinberg::tresult PLUGIN_API onWheel(float) override { return Steinberg::kResultFalse; }
    Steinberg::tresult PLUGIN_API onKeyDown(
            Steinberg::char16, Steinberg::int16, Steinberg::int16) override { return Steinberg::kResultFalse; }
    Steinberg::tresult PLUGIN_API onKeyUp(
            Steinberg::char16, Steinberg::int16, Steinberg::int16) override { return Steinberg::kResultFalse; }
    Steinberg::tresult PLUGIN_API checkSizeConstraint(
            Steinberg::ViewRect*) override { return Steinberg::kResultOk; }
};

// ─────────────────────────────────────────────────────────────────────────────
struct CreateVstForxEditorVST3 {
    template <class IPlugViewT, class Plugin>
    IPlugViewT* createEditor(Plugin* plugin) {
        return static_cast<IPlugViewT*>(
            new VstForxPlugView(static_cast<frx::processing::VstForxPlug*>(plugin)));
    }
};

}} // namespace frx::vst3
