#include <limits> 
#include "parameter.h" 

namespace processing {
namespace parameter {
//============================================================================================================
// Schnitstelle: ConnectionOperator.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
ConnectionOperator::ConnectionOperator (Parameter *u, Parameter *v) : u(u), v(v) { }
//============================================================================================================
// Klasse: Parameter.
// Repraesentiert alle PPI VST-Parameter die als schnitstelle zum
// Host dienen. 
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
Parameter::Parameter( int index ):
updateLock(false), 
_min(0.0f),
_max(1.0f)
{
	nr = instances++;
	Parameter::index = index;
	setName ("Parameter:" + MyString(nr) );
	setValue (0.0);
}

//------------------------------------------------------------------------------------------------------------
Parameter::~Parameter() {
	instances--;
	// kollegen bescheid sagen dass ich n schwan mach.
	ParameterConnection::iterator it = connections.begin();
	for ( ;it!=connections.end(); ++it ) {
		(*it).first->_removeConnection (this);
	}
}

//------------------------------------------------------------------------------------------------------------
int Parameter::instances = 0;
//------------------------------------------------------------------------------------------------------------
// durchlaeuft alle Parameter in connections, setzt dort eigenen
// Parameter wert und ruft operate() methoden
// der Assoziierten ConnectionOperator Objekte auf.
void Parameter::updateConnections(){
	TRY_TO_LOCK_TIMED(mutex);

	updateLock = true; // sperrt setValue() aufrufe.
	ParameterConnection::iterator it = connections.begin();
	while ( it!=connections.end() ){
		Parameter *p = (*it).first;
		ConnectionOperator::Container &liPP = (*it).second;
		VstNumber tmp = value;
		ConnectionOperator::Container::iterator it2 = liPP.begin();
		while ( it2!=liPP.end() ){ // zu p gehoerige ProcessParameter Objekte
			tmp = (*it2).get()->operate ( tmp );
			++it2;
		}
		*p = tmp;
		++it;
	}
	updateLock = false; // schaltet setValue() wieder frei
}

//------------------------------------------------------------------------------------------------------------
void Parameter::_removeConnection(processing::parameter::Parameter *dst){
	TRY_TO_LOCK_TIMED(mutex);	
	ParameterConnection::iterator it = connections.find ( dst );
		if ( it == connections.end() ) return;
		//parameter processoren entfernen.
		(*it).second.clear();
		connections.erase ( it );
}

//------------------------------------------------------------------------------------------------------------
void Parameter::removeBiConnection(processing::parameter::Parameter *dst){
	_removeConnection ( dst );
	dst->_removeConnection ( this );
}
} // namespace parameter
} // namespace processing



