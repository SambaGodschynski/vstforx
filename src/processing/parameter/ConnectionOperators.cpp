#include "ConnectionOperators.h"



namespace processing{
namespace parameter {

//============================================================================================================
// Klasse: ExpConnection.
// Proportionale Verbindung mit Exponentialen anstieg.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
ConnectionOperator * ExpConnection::newInvereseOperator() { 
	LogConnection *neu = new LogConnection(v,u);
	neu->getParameter(0)->addBiConnection ( getParameter(0).get() );
	return neu;
}

//============================================================================================================
// Klasse: LogConnection.
// Proportionale Verbindung mit Log anstieg.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
ConnectionOperator * LogConnection::newInvereseOperator() { 
	ExpConnection *neu = new ExpConnection(v,u);
	neu->getParameter(0)->addBiConnection ( getParameter(0).get() );
	return neu;
}
//============================================================================================================
// Klasse: OffsetConnection.
// adds an offset: -0.5 0 +0.5
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void OffsetConnection::parameterChanged ( void *src, const float &p ) {
	offset->setDisplay( MyString( p - 0.5f ) );
	u->setValue(u->getValue()); 
}
//------------------------------------------------------------------------------------------------------------
ConnectionOperator * OffsetConnection::newInvereseOperator() { 
	OffsetConnection *neu = new OffsetConnection(v,u);
	neu->inverse = true;
	neu->getParameter(0)->addBiConnection ( getParameter(0).get() );
	return neu;
}

}//namespace parameter
}//namespace processing


