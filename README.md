# VSTForx


VSTForx is a creative effect network creation tool which comes as a VST plugin. You can load any number of VST-plugins and connect them anyway you want.

## sambag

The main library for vstforx:
* UI Framework using libcairo
* svg rendering
* lua, sqlite, dsp, math, xml helper

---

## Building

vstforx depends on [sambag](https://github.com/SambaGodschynski/sambag). Clone it
separately and pass its path to CMake via `-DSAMBAG_DIR`.

### Linux – Standalone app (quick start)

#### Prerequisites

```bash
sudo apt install \
    cmake build-essential \
    libcairo2-dev \
    liblua5.4-dev \
    libloki-dev \
    libsqlite3-dev \
    libboost-all-dev \
    libx11-dev
```

#### Configure and build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DSAMBAG_DIR=/path/to/sambag
cmake --build build --target standalone -j$(nproc)
```

#### Run

```bash
./build/src/PluginApps/standalone
```

Images, styles and Lua scripts are copied automatically next to the binary
during the build step.

#### CMake options

| Option | Default | Description |
|--------|---------|-------------|
| `SAMBAG_DIR` | *(required)* | Path to the sambag source tree |
| `FRX_VSTSDK2_DIR` | `$SAMBAG_DIR/ext/vstsdk2.4` | VST2 SDK root (override if needed) |
| `FRX_BUILD_STANDALONE` | ON | Build the standalone application |
| `FRX_BUILD_PLUGIN` | OFF | Build VST plugin shared library |
| `FRX_BUILD_VST3` | OFF | Include VST3 hosting support (requires VST3 SDK) |
| `FRX_USE_LOG` | OFF | Enable boost::log |

---

## Architecture notes

### CMake modernisation (2024)

The original 2016 CMake build used:
- `cmake_minimum_required(VERSION 2.6)` — updated to 3.15
- Hand-written `FindXxx.cmake` scripts in `sambag/cmake/` — replaced with
  `pkg_check_modules` (Cairo, Lua, CppUnit) and built-in modules (Boost, SQLite3)
- No `src/CMakeLists.txt` — created; uses `file(GLOB_RECURSE)` with platform
  exclusion filters
- VST SDK was unconditionally required — now optional via `SAMBAG_VST_SUPPORT`
- Old `IF(WIN32) ... ENDIF(WIN32)` syntax — replaced with modern `if/endif`

### Source exclusion by platform

The glob-based source collection in `src/CMakeLists.txt` automatically excludes:
- macOS: Cocoa, Quartz, CoreGraphics files
- Windows: Win32 resource files
- VST SDK absent: `dsp/*Vst*.cpp`
- Always: `tests/`, `testApps/`, `*Test*.cpp`

### X11 / Boost math conflict

`X11/X.h` defines `#define Complex 0` which breaks template parameter names in
`boost/math/tools/roots.hpp`. Fixed by adding `#undef Complex` in
`X11WindowImpl.hpp` right after the X11 includes.
