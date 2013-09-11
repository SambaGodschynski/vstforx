#include "frxPluginDK/FrxPluginWrapper.hpp"


namespace  {
struct PluginProcessor {
};
} // namespace(s)
    



RawFrxPlugin* createFrxPluginInstance (FrxHostCallback host) {
    using namespace sambag::dsp;
    typedef frx::FrxPluginWrapper<PluginProcessor> Plugin;
    Plugin *plug = new Plugin();
    return plug->getRawFrxPlugin();
}