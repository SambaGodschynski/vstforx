/*
 * GetPluginFactory.cpp — VST3 DLL entry point for VSTforx
 *
 * Cross-platform (Linux + Windows).  Mac is handled by a separate bundle target.
 *
 * VST3 bundle layout:
 *   vstforx3.vst3/Contents/x86_64-linux/vstforx3.so   (Linux)
 *   vstforx3.vst3/Contents/x86_64-win/vstforx3.vst3   (Windows)
 *   vstforx3.vst3/Contents/Resources/images|styles|scripts/
 */

#include <processing/VstForxPlug.hpp>
#include <sambag/dsp/VST3xPluginWrapper.hpp>
#include <sambag/dsp/VST3xPluginFactory.hpp>
#include "VstForxPlugView.hpp"
#include <com/Settings.h>
#include <sambag/disco/FileResourceManager.hpp>
#include <sambag/disco/IResourceManager.hpp>
#include <sambag/com/Common.hpp>
#include <sambag/disco/components/WindowToolkit.hpp>
#include "PluginApps/com/FrxPlugSettings.hpp"

#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
static HMODULE g_hModule = nullptr;
// frx_core's win32 glue (win_one4All.cpp) references this as extern;
// the VST2 entry point normally defines it — we define it here for VST3.
void* hInstance = nullptr;
#else
#  include <dlfcn.h>
static void* g_soHandle = nullptr;
#endif

#include <string>
#include <filesystem>

// ─────────────────────────────────────────────────────────────────────────────
// Resolve directory containing this shared library at runtime.
// On Linux we use dladdr; on Windows GetModuleFileName.
// ─────────────────────────────────────────────────────────────────────────────
namespace {

std::string getPluginDirectory() {
#ifdef _WIN32
    if (!g_hModule) return ".";
    char path[MAX_PATH] = {};
    GetModuleFileNameA(g_hModule, path, MAX_PATH);
    return std::filesystem::path(path).parent_path().string();
#else
    // Use a function pointer from within this .so so dladdr returns this library's path.
    // g_soHandle is a dlopen handle (not a symbol address) and must not be passed to dladdr.
    Dl_info info{};
    if (dladdr(reinterpret_cast<void*>(&getPluginDirectory), &info) && info.dli_fname) {
        return std::filesystem::path(info.dli_fname).parent_path().string();
    }
    return ".";
#endif
}

// Resources live at <bundle>/Contents/Resources/ (VST3 standard).
// The .so/.vst3 is at <bundle>/Contents/<arch>/; one parent_path() gives Contents/.
std::string getResourceRoot() {
    namespace fs = std::filesystem;
    std::string soDir = getPluginDirectory();
    fs::path contentsDir = fs::path(soDir).parent_path();
    fs::path resourcesDir = contentsDir / "Resources";
    if (fs::exists(resourcesDir / "images")) return resourcesDir.string();
    // Fallback: bundle root (old layout) or .so directory (development)
    fs::path bundleRoot = contentsDir.parent_path();
    if (fs::exists(bundleRoot / "images")) return bundleRoot.string();
    return soDir;
}

} // namespace

// ─────────────────────────────────────────────────────────────────────────────
// Plugin typedef
// ─────────────────────────────────────────────────────────────────────────────
typedef sambag::dsp::vst::VST3xPluginWrapper<
    frx::processing::VstForxPlug,
    sambag::dsp::StdPluginTraits<
        PlugSettings::PluginInputs,
        PlugSettings::PluginOutputs,
        PlugSettings::IsInstrument,
        ::com::Settings::PROGRAM_PARAMETER
    >,
    frx::vst3::CreateVstForxEditorVST3
> VstForx3Plugin;

// ─────────────────────────────────────────────────────────────────────────────
// VST3 component class ID (16 bytes, must be unique and stable)
// Generated specifically for VSTforx VST3.
// ─────────────────────────────────────────────────────────────────────────────
// {A1B2C3D4-E5F6-7890-ABCD-EF1234567890}  — VSTforx Fx
static const Steinberg::TUID kVstForx3ClassId = {
    '\xA1', '\xB2', '\xC3', '\xD4',
    '\xE5', '\xF6', '\x78', '\x90',
    '\xAB', '\xCD', '\xEF', '\x12',
    '\x34', '\x56', '\x78', '\x90'
};

// ─────────────────────────────────────────────────────────────────────────────
// One-time plugin init (called on first GetPluginFactory)
#ifndef _WIN32
namespace frx { namespace vst3 {
int VstForxPlugView::_boostTimerRefCount = 0;
}} // namespace frx::vst3
#endif

// ─────────────────────────────────────────────────────────────────────────────
namespace {

bool g_initialized = false;

void ensureInit() {
    if (g_initialized) return;
    g_initialized = true;

    std::string resRoot = getResourceRoot();
    ::sambag::com::addLogFile(resRoot + "/VSTForx.log");

    try {
        sambag::disco::FileResourceManager::init(resRoot);
        sambag::disco::installResourceManager(
            sambag::disco::FileResourceManager::instance());
    } catch (const std::exception& ex) {
        SAMBAG_LOG_ERR<<"vstforx: ResourceManager init failed: "<<ex.what();
    }

    try {
        ::com::initSettings(resRoot);
    } catch (const std::exception& ex) {
        SAMBAG_LOG_ERR<<"vstforx: initSettings failed: "<<ex.what();
    }

    sambag::disco::components::getWindowToolkit()->useWithoutMainloop();
}

} // namespace

// ─────────────────────────────────────────────────────────────────────────────
// GetPluginFactory — VST3 DLL entry point
// ─────────────────────────────────────────────────────────────────────────────
extern "C" {

#ifdef _WIN32
SMTG_EXPORT_SYMBOL Steinberg::IPluginFactory* PLUGIN_API GetPluginFactory()
#else
__attribute__((visibility("default")))
Steinberg::IPluginFactory* GetPluginFactory()
#endif
{
    ensureInit();
    return new sambag::dsp::vst::VST3xPluginFactory<VstForx3Plugin>(
        "Samba Godschynski",
        "https://vstforx.de",
        "samba.godschynski@gmail.com",
        kVstForx3ClassId,
        "VSTForx",
        "Fx"
    );
}

#ifdef _WIN32
BOOL WINAPI DllMain(HINSTANCE hInst, DWORD /*reason*/, LPVOID /*reserved*/) {
    g_hModule = reinterpret_cast<HMODULE>(hInst);
    hInstance  = reinterpret_cast<void*>(hInst);
    return TRUE;
}
#else
// Linux VST3 requires ModuleEntry/ModuleExit instead of DllMain.
// The host calls ModuleEntry right after dlopen(), passing the dl handle.
__attribute__((visibility("default")))
bool ModuleEntry(void* sharedLibraryHandle) {
    g_soHandle = sharedLibraryHandle;
    return true;
}

__attribute__((visibility("default")))
bool ModuleExit() {
    g_soHandle = nullptr;
    return true;
}
#endif

} // extern "C"
