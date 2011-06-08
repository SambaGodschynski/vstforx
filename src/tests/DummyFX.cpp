#include "DummyFX.h"



//=============================================================================
AudioEffect * createEffectInstance ( audioMasterCallback audioMaster ){
//=============================================================================
	return new processing::DummyFX( audioMaster );
}
