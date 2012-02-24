/*
 * ===========================================================================================================
 * NullAEffect.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#include "NullAEffect.h"

namespace processing {

//------------------------------------------------------------------------------------------------------------
NullAEffect nullAEff;

//============================================================================================================
// Klasse: NullAEffect.
// Wird als proxy eingesetzt fuer Plugins die nicht geladen werden konnten.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
NullAEffect::NullAEffect() {
	AEffect::dispatcher = &_aEffectDispatcherProc;
	AEffect::flags = 0;
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
VstIntPtr NullAEffect::_aEffectDispatcherProc(
AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt )
{
	return 0;
}
//------------------------------------------------------------------------------------------------------------
void NullAEffect::_aEffectProcessProc(
AEffect* effect, float** inputs, float** outputs, VstInt32 numSamples)
{
}
//------------------------------------------------------------------------------------------------------------
void NullAEffect::_aEffectProcessDoubleProc(
AEffect* effect, double** inputs, double** outputs, VstInt32 numSamples)
{
}
//------------------------------------------------------------------------------------------------------------
void NullAEffect::_aEffectSetParameterProc(AEffect* effect, VstInt32 index, float parameter)
{
}
//------------------------------------------------------------------------------------------------------------
float NullAEffect::_aEffectGetParameterProc(AEffect* effect, VstInt32 index) {
	return 0;
}
}// namespace
