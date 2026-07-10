#pragma once

#include <atomic>
#include <cstring>

#include "pluginterfaces/gui/iplugview.h"
#include "gui/components/VstForxEditor.hpp"
#include <processing/VstForxPlug.hpp>

namespace frx { namespace vst3 {

// ─────────────────────────────────────────────────────────────────────────────
// IPlugView implementation that wraps the existing VstForxEditor.
//
// The VST3 host calls:
//   1. attached(parent, type) — embed plugin window into host-provided handle
//   2. getSize()              — query initial size
//   3. removed()              — detach and close
// ─────────────────────────────────────────────────────────────────────────────
class VstForxPlugView : public Steinberg::IPlugView {
    frx::gui::components::VstForxEditor* _editor;
    Steinberg::IPlugFrame*               _frame{nullptr};
    std::atomic<Steinberg::uint32>       _refCount{1};
public:
    // Plugin inherits VstForxPlug → AudioEffectX → AudioEffect
    explicit VstForxPlugView(frx::processing::VstForxPlug* plugin)
        : _editor(new frx::gui::components::VstForxEditor(nullptr))
    {
        _editor->setPlugin(plugin);
    }

    ~VstForxPlugView() { delete _editor; }

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
        _editor->open(parent);
        return Steinberg::kResultOk;
    }

    Steinberg::tresult PLUGIN_API removed() override {
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

    Steinberg::tresult PLUGIN_API canResize() override {
        return Steinberg::kResultFalse;
    }

    Steinberg::tresult PLUGIN_API onSize(Steinberg::ViewRect*) override {
        return Steinberg::kResultFalse;
    }
    Steinberg::tresult PLUGIN_API onFocus(Steinberg::TBool) override {
        return Steinberg::kResultFalse;
    }
    Steinberg::tresult PLUGIN_API onWheel(float) override {
        return Steinberg::kResultFalse;
    }
    Steinberg::tresult PLUGIN_API onKeyDown(
            Steinberg::char16, Steinberg::int16, Steinberg::int16) override {
        return Steinberg::kResultFalse;
    }
    Steinberg::tresult PLUGIN_API onKeyUp(
            Steinberg::char16, Steinberg::int16, Steinberg::int16) override {
        return Steinberg::kResultFalse;
    }
    Steinberg::tresult PLUGIN_API checkSizeConstraint(
            Steinberg::ViewRect*) override {
        return Steinberg::kResultOk;
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// Policy for VST3xPluginWrapper: creates VstForxPlugView when host asks for editor.
// ─────────────────────────────────────────────────────────────────────────────
struct CreateVstForxEditorVST3 {
    template <class IPlugViewT, class Plugin>
    IPlugViewT* createEditor(Plugin* plugin) {
        return static_cast<IPlugViewT*>(
            new VstForxPlugView(static_cast<frx::processing::VstForxPlug*>(plugin)));
    }
};

}} // namespace frx::vst3
