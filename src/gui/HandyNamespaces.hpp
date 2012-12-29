#ifndef FRX_HANDY_SAMBAG_NAMESPACES
#define FRX_HANDY_SAMBAG_NAMESPACES

namespace sambag { 
	namespace com {
		namespace events {
		}
	}
	namespace disco {
		namespace components {
			namespace ui {
			}
			namespace events {
			}		
		}	
		namespace svg {
			namespace graphicElements {
			}
		}
	}
}

namespace frx { 
	namespace gui {
		namespace components {
		}
	}
	namespace processing {
	}
	namespace sc = sambag::com;
	namespace sce = sc::events;
	namespace sd = sambag::disco;
	namespace sdc = sd::components;
	namespace sdce = sdc::events;	
	namespace sdcu = sdc::ui;
	namespace sds = sd::svg;
	namespace sdsg = sds::graphicElements;
	namespace fgc = frx::gui::components;
	namespace fp = frx::processing;
}


#endif // FRX_HANDY_SAMBAG_NAMESPACES
