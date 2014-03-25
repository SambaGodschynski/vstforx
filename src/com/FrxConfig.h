#ifndef FRX_CONFIG_H
#define FRX_CONFIG_H 


// following values will be set automatically.
#define FRX_VERSION_MAJOR 1
#define FRX_VERSION_MINOR 0
#define FRX_VERSION_MICRO 5
#define FRX_VERSION_BUILD "93bf61b" 
#define FRX_VERSION_BRANCH "NEXT"

#define FRX_USE_FONTCACHE 0


// async dsp timer config
#define FRX_TOOLKIT_TIMER 0
#define FRX_BOOST_TIMER 1
#define FRX_ASYNC_DSP_TIMER FRX_BOOST_TIMER

#define FRX_MAX_DEMO_MODULES 4
#define FRX_MAX_CM_MODULES 12
#define FRX_SHMSESS_MAX_PATH_LENGTH 512
#define FRX_SHMSESS_MAX_STR_LENGTH 128
#define FRX_SHMSESS_MAX_DATA_LENGTH 128
#define FRX_SHMSESS_DEFAULT_TIMEOUT_MS 30 * 1000

// the boost archive version. Set as HostInfoAdapter 
// BOOST_CLASS_VERSION in VstForxPlug.cpp, because
// this is the first object which will be loaded.
#define FRX_ARCHIVE_VERSION 1

#endif // FRX_CONFIG_H
