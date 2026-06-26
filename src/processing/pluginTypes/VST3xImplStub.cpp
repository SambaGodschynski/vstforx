// Stubs for features not built: VST3 hosting and interprocess bridge.
#include <processing/pluginTypes/PluginFactory.hpp>

namespace frx { namespace processing {

PluginFactory::ProductPtr createVST3xPluginImpl(
    IHostInfo::Ptr, PluginFactory::Parameters*, const std::string&)
{
    return nullptr;
}

PluginFactory::ProductPtr createBridgedPluginImpl(
    IHostInfo::Ptr, PluginFactory::Parameters*, const std::string&)
{
    return nullptr;
}

}} // namespace frx::processing
