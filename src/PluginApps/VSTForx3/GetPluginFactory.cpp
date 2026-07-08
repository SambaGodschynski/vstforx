/*
 * GetPluginFactory.cpp — VST3 DLL entry point for VSTforx
 *
 * Cross-platform (Linux + Windows).  Mac is handled by a separate bundle target.
 *
 * The VST3 bundle layout on Linux:
 *   vstforx3.vst3/Contents/x86_64-linux/vstforx3.so
 *
 * Resources (images/, styles/, scripts/) are expected alongside the .so's
 * grandparent directory:
 *   vstforx3.vst3/Contents/x86_64-linux/../../../   →  vstforx3.vst3/
 */

#include <processing/VstForxPlug.hpp>
#include <sambag/dsp/VST3xPluginWrapper.hpp>
#include <sambag/dsp/VST3xPluginFactory.hpp>
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
    Dl_info info{};
    if (dladdr(reinterpret_cast<void*>(&getPluginDirectory), &info) && info.dli_fname) {
        return std::filesystem::path(info.dli_fname).parent_path().string();
    }
    return ".";
#endif
}

// .so is at <bundle>/Contents/x86_64-linux/; resources are at <bundle>/
std::string getResourceRoot() {
    namespace fs = std::filesystem;
    fs::path soDir = getPluginDirectory();
    // Walk up two directories: x86_64-linux -> Contents -> <bundle>
    fs::path root = soDir.parent_path().parent_path();
    if (fs::exists(root / "images")) return root.string();
    // Fallback: same directory as the .so (development / flat install)
    return soDir.string();
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
    >
    // CreateNoEditor is the default — VST3 editor requires IPlugView which
    // VstForxEditor doesn't implement yet.
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
// ─────────────────────────────────────────────────────────────────────────────
namespace {

bool g_initialized = false;

void ensureInit() {
    if (g_initialized) return;
    g_initialized = true;

    std::string resRoot = getResourceRoot();
    ::sambag::com::addLogFile(resRoot + "/VSTForx.log");
    SAMBAG_LOG_INFO << "VST3 factory loaded, resRoot=" << resRoot;

    try {
        sambag::disco::FileResourceManager::init(resRoot);
        sambag::disco::installResourceManager(
            sambag::disco::FileResourceManager::instance());
    } catch (const std::exception& ex) {
        SAMBAG_LOG_ERR << "ResourceManager init failed: " << ex.what();
    }

    ::com::initSettings(resRoot);
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
#endif

} // extern "C"
