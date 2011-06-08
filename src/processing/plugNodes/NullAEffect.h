#ifndef NULL_AEFFECT_H
#define NULL_AEFFECT_H

#include "audioeffectx.h"

namespace processing {
//============================================================================================================
// Klasse: NullAEffect.
// Wird als proxy eingesetzt fuer Plugins die nicht geladen werden konnten.
//============================================================================================================
class NullAEffect : public AEffect {
	//--------------------------------------------------------------------------------------------------------
	static VstIntPtr _aEffectDispatcherProc(AEffect* effect, VstInt32 opcode, VstInt32 index, 
		                                    VstIntPtr value, void* ptr, float opt);
	//--------------------------------------------------------------------------------------------------------
	static void _aEffectProcessProc(AEffect* effect, float** inputs, float** outputs, VstInt32 sampleFrames);
	//--------------------------------------------------------------------------------------------------------
	static void _aEffectProcessDoubleProc(AEffect* effect, double** inputs, 
		                                  double** outputs, VstInt32 sampleFrames);
	//--------------------------------------------------------------------------------------------------------
	static void _aEffectSetParameterProc(AEffect* effect, VstInt32 index, float parameter);
	//--------------------------------------------------------------------------------------------------------
	static float _aEffectGetParameterProc(AEffect* effect, VstInt32 index);
public:
	//--------------------------------------------------------------------------------------------------------
	NullAEffect();
};
extern NullAEffect nullAEff;
} // namespace
#endif