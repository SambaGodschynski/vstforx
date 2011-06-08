#include "Resources.h"

extern CBitmap ** loadResources();

namespace ppiGui {
//============================================================================================================
//class Resources 
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
Resources::Ptr Resources::singleton;
//------------------------------------------------------------------------------------------------------------
Resources::Ptr Resources::initResources(AEffGUIEditor *editor) {
	if (!editor) return Resources::Ptr();
	if (Resources::singleton) return Resources::singleton;
	Resources::singleton = Resources::Ptr( new Resources() );
	return Resources::singleton;
}
//------------------------------------------------------------------------------------------------------------
Resources::~Resources() {
	for ( size_t i=0; i<NUM_BITMAPS; ++i ) {
		delete bitmaps[i];
	}
	delete bitmaps;
}
//------------------------------------------------------------------------------------------------------------
Resources::Resources() {
	bitmaps = loadResources();
}
} // namespace