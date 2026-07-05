/*
 * ===========================================================================================================
 * TestAEffect.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#include "TestAEffect.hpp"
#include <memory>
#include <vector>

namespace processing {

//------------------------------------------------------------------------------------------------------------
TestAEffect testAEff;

//============================================================================================================
// Klasse: TestAEffect.
// Wird als proxy eingesetzt fuer Plugins die nicht geladen werden konnten.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
TestAEffect::TestAEffect() {
	AEffect::dispatcher = &_aEffectDispatcherProc;
	AEffect::flags = effFlagsCanReplacing;
	AEffect::getParameter = &_aEffectGetParameterProc;
	AEffect::initialDelay = 0;
	AEffect::magic='VstP';
	AEffect::numInputs = 0;
	AEffect::numOutputs = 0;
	AEffect::numParams = 0;
	AEffect::numPrograms = 0;
	AEffect::object = NULL;
	AEffect::processDoubleReplacing = &_aEffectProcessDoubleProc;
	AEffect::processReplacing = &_aEffectProcessProc;
	AEffect::setParameter = &_aEffectSetParameterProc;
	AEffect::uniqueID = 0;
	AEffect::user = NULL;
	AEffect::version = 0;
	AEffect::DECLARE_VST_DEPRECATED(process) = &_aEffectProcessProc;
}
//------------------------------------------------------------------------------------------------------------
VstIntPtr TestAEffect::_aEffectDispatcherProc(
AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt )
{
	return 0;
}
//------------------------------------------------------------------------------------------------------------
void TestAEffect::_aEffectProcessProc(
AEffect* effect, float** inputs, float** outputs, VstInt32 numSamples)
{
}
//------------------------------------------------------------------------------------------------------------
void TestAEffect::_aEffectProcessDoubleProc(
AEffect* effect, double** inputs, double** outputs, VstInt32 numSamples)
{
}
//------------------------------------------------------------------------------------------------------------
void TestAEffect::_aEffectSetParameterProc(AEffect* effect, VstInt32 index, float parameter)
{
}
//------------------------------------------------------------------------------------------------------------
float TestAEffect::_aEffectGetParameterProc(AEffect* effect, VstInt32 index) {
	return 0;
}
//------------------------------------------------------------------------------------------------------------
namespace {
    typedef std::shared_ptr<TestAEffect> EffPtr;
    typedef std::vector<EffPtr> EffPtrs;
    EffPtrs holder;
    

} // namespace
TestAEffect * TestAEffect::createLongevity() {
    EffPtr neu( new TestAEffect() );
    holder.push_back(neu);
    return neu.get();
}

}// namespace
