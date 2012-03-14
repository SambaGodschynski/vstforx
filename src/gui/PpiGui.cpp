/*
 * ===========================================================================================================
 * PpiGui.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#include "PpiGui.h"
#include "CircuidView.h"
#include "ViewCommand.h"
#include "PpiEditor.h"
#include "MainCtrl.h"
#include "OS_Specific/WindowDef.h"
#include "CircuidView.h"

void releaseCReference ( CReferenceCounter *ref ) {
	ref->forget();
}

namespace ppiGui {
//============================================================================================================
//  Frei stehende Methode:
//  Punkt Rotieren um einen MPunkt. alpha in grad.
//============================================================================================================
void rotate(CPoint &p, float alpha, const CPoint &center) {
	double arcus = (2.0f * PI * alpha) / 360.0f;
	float cosinus = cos( arcus );
	float sinus   = sin( arcus );
	float x = (float)(p.x - center.x);
	float y = (float)(p.y - center.y);
	CPoint n;
	p.x = (CCoord) (cosinus * x - sinus * y + center.x);
	p.y = (CCoord) (sinus * x + cosinus * y + center.y);
}
//------------------------------------------------------------------------------------------------------------
VSTGUI::CRect normalizeRect( const VSTGUI::CRect &rect ){
	VSTGUI::CRect tmp = rect;
	if ( tmp.left > tmp.right ) com::xChange<CCoord>(tmp.left, tmp.right);
	if ( tmp.top > tmp.bottom ) com::xChange<CCoord>(tmp.top, tmp.bottom);
	return tmp;
}
//------------------------------------------------------------------------------------------------------------
VSTGUI::CRect getBoundingBox ( const GObjectIterator &begin, const GObjectIterator &end ) {
	int minLeft   = INT_MAX;
	int minTop    = INT_MAX;
	int maxRight  = INT_MIN;
	int maxBottom = INT_MIN;
	GObjectIterator it(begin);
	while ( it != end ){
		VSTGUI::CRect size = (*it)->getSize(); 
		if ( size.left < minLeft ) minLeft = size.left;
		if ( size.top < minTop ) minTop = size.top;
		if ( size.right > maxRight ) maxRight = size.right;
		if ( size.bottom > maxBottom ) maxBottom = size.bottom;
		++it;
	}
	VSTGUI::CRect tmp( minLeft, minTop, maxRight, maxBottom );
	return tmp;
}
//------------------------------------------------------------------------------------------------------------
void brightness ( CColor &col, float fak ) {
	float r = com::getMax<float> ( 0, com::getMin<float> ( 255.0f, col.red * fak ) );
	float g = com::getMax<float> ( 0, com::getMin<float> ( 255.0f, col.green * fak ) );
	float b = com::getMax<float> ( 0, com::getMin<float> ( 255.0f, col.blue * fak ) );
	col.red = ( unsigned char ) r;
	col.green = ( unsigned char ) g;
	col.blue = ( unsigned char ) b;
}
//============================================================================================================
//	Klasse GObject:
//  Oberklasse fuer alle Graphischen Objekte die in der CircuidView dargestellt werden.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
int GObject::instances = 0;
//------------------------------------------------------------------------------------------------------------
void GObject::setObjectDirty(){
	parentView->setDirty ( this );
}
//------------------------------------------------------------------------------------------------------------
void GObject::onMouse( CDrawContext *cc, CPoint &p, long btn) {
	GObject *obj = this;
	for ( int i=subObjects.size()-1; i>=0; --i ) {
		if ( subObjects[i]->hitTest(p) ) {
			obj = subObjects[i].get();
			break;
		}
	}
	sendMouseEvents( cc, p, btn, obj, obj, obj );
}
//------------------------------------------------------------------------------------------------------------
GObject::GObject(ppiGui::CircuidView *parentView, std::string name ) {
	GObject::parentView = parentView;
	objNr = instances++;
	visible = true;
	GObject::name = name;
}
//------------------------------------------------------------------------------------------------------------
GObject::~GObject() {} 
//------------------------------------------------------------------------------------------------------------
void GObject::save ( oArchive &ar, const unsigned int version ) const {
	com::MethodMessage<GObject> methodMessage ( "save()", *this );
	ar << name;
	ar << description;
	CPoint pos = getPos();
	ar << pos.x;
	ar << pos.y;
	ar << zPos;
	ar << self;
}
//------------------------------------------------------------------------------------------------------------
void GObject::load ( iArchive &ar, const unsigned int version ){
	com::MethodMessage<GObject> methodMessage ( "load()", *this );
	ar >> name;
	ar >> description;
	CPoint pos;
	ar >> pos.x;
	ar >> pos.y;
	ar >> zPos;
	ar >> self;
	moveTo ( pos );
}
//============================================================================================================
//	Klasse GNode:
//  Repraesentiert Knoten Objekte die frei in der Circuid View bewegt werden kann.
//  Kann mit anderen Knoten verbunden werden. 
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
GIONode::GIONode(CircuidView *parent) : GCircle ( parent ){
	name = "GNode";
	radius = 0;
}
//------------------------------------------------------------------------------------------------------------
GIONode::~GIONode(){
}
//============================================================================================================
//	Klasse GLine:
//  Stellt eine Linie dar.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void GLine::draw(CDrawContext *pContext){
	CDrawContextPlus cD (*pContext);
	if (!isVisible()) return;
	cD.setLineStyle ( style );
	cD.setLineWidth ( width );
	cD.setFrameColor ( color );
	cD.drawLine ( CPoint (bBox.left, bBox.top), CPoint (bBox.right, bBox.bottom) );
	cD.setLineStyle ( kLineSolid ); // reset ls
}
//------------------------------------------------------------------------------------------------------------
bool GLine::hitTest ( const CPoint &q ) const {
/*	
	Richtungsvektor ausrechnen
	U.x = P2.x - P1.x
	U.y = P2.y - P1.y
	# Dann den Richtungsvektor normieren
	Len = sqrt (U.x * U.x + U.y * U.y)
	if Len < 0.0001 Fehler!
	U.x = U.x / Len
	U.y = U.y / Len
	# Orthogonal Projizieren
	Lambda = (Q.x - P1.x) * U.x + (Q.y - P1.y) * U.y
	# Fusspunkt ausrechnen
	D.x = P1.x + Lambda * U.x
	D.y = P1.y + Lambda * U.y
*/	
	VSTGUI::CRect tmp = normalizeRect(bBox);
	static const int D = 15; // toleranz

	if ( tmp.width()<D ) { tmp.setWidth(D); tmp.offset (-D/2,0); }
	if ( tmp.height()<D ) { tmp.setHeight(D); tmp.offset (0,-D/2); }
	if ( !tmp.pointInside (q) ) return false;
	
	Vector2D u ( CPoint (bBox.left, bBox.top), CPoint (bBox.right, bBox.bottom) ); 
	float len = Vector2D::abs (u);
	if (len<0.0001) return false;
	u/=len;
	float l = ( q.x - bBox.left ) * u.x + ( q.y - bBox.top ) * u.y;
	Vector2D d;
	d.x =  q.x - ( bBox.left + l * u.x );
	d.y =  q.y - ( bBox.top + l * u.y );
	len = Vector2D::abs (d);
	return len < com::getMax<int>( width, D );
}
//============================================================================================================
//	Klasse GRect:
//  Stellt ein Rechteck dar.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void GRect::draw(CDrawContext *pContext) {
	if (!isVisible()) return;
	CDrawContextPlus cD ( *pContext );
	cD.setLineStyle ( lineStyle );
	cD.setLineWidth ( width );
	cD.setFrameColor ( color );
	cD.setFillColor ( bgColor );
	cD.drawRect ( bBox );
	cD.setLineStyle ( kLineSolid ); // reset ls
}
//============================================================================================================
//	Klasse GBitmap:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void GBitmap::draw( CDrawContext *pContext ) {
	bmp->draw ( pContext, bBox, CPoint() );
}
//============================================================================================================
//	Klasse GCircle:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void GCircle::draw ( CDrawContext *cc, int renderRadius ) {
	cc->setFillColor (color);
	VSTGUI::CRect tmp = bBox;
	int d = radius - renderRadius;
	tmp.inset( d, d );
	cc->fillArc ( tmp, CPoint (), CPoint() );
}
//============================================================================================================
//	Klasse GProcessorNode:
//  Repraesentiert ein Processor Knoten oder ProcessAdapter Objekt aus dem Graph. 
//  Ein GProcessorNode hatt ein oder mehrerer Ein-und Ausgaenge.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
GProcessorNode::GProcessorNode(CircuidView *parent) : GCircle ( parent, "GProcessorNode" ) {}
//------------------------------------------------------------------------------------------------------------
void GProcessorNode::createIONodes ( int numInputs, int numOutputs  )
{
	if ( numInputs==0 && numOutputs==0 ) return;
	// Input Nodes erstellen---------------------------------------
	CPoint p = getPos();
	p.offset ( 0, - bBox.height() );
	CPoint u = p; // start
	float d = 100.0/(float)numInputs;
	for ( int i=0; i<numInputs/2; ++i ) { // eine haelfte nach links faechern
		int f = numInputs/2 - i;
		rotate (p, f*-d, getPos() );
		ins.push_back ( createInNode(p) );
		p = u;
	}
	if (numInputs%2==1) ins.push_back ( createInNode(p) ); // mitte
	for ( int i=0; i<numInputs/2; ++i ) { // andere haelfte nach rechts faechern
		int f = numInputs/2 - i;
		rotate (p, f*d, getPos() );
		ins.push_back ( createInNode(p) );
		p = u;
	}
	// Output Nodes erstellen---------------------------------------
	p = getPos();
	p.offset ( 0, bBox.height() );
	u = p; // start
	d = 100.0/(float)numOutputs;
	for ( int i=0; i<numOutputs/2; ++i ) { // eine haelfte nach links faechern
		int f = numOutputs/2 - i;
		rotate (p, f*d, getPos() );
		outs.push_back ( createOutNode(p) );
		p = u;
	}
	if (numOutputs%2==1)outs.push_back ( createOutNode(p) ); // mitte
	for ( int i=0; i<numOutputs/2; ++i ) { // andere haelfte nach rechts faechern
		int f = numOutputs/2 - i;
		rotate (p, f*-d, getPos() );
		outs.push_back ( createOutNode(p) );
		p = u;
	}
}
//------------------------------------------------------------------------------------------------------------
GObject::Ptr GProcessorNode::createInNode(CPoint &p) {
	GInputNode::Ptr in = GInputNode::create ( parentView, Resources::VSTPLUG_INPUT );
	in->moveTo ( p );
	return in;
}
//------------------------------------------------------------------------------------------------------------
GObject::Ptr GProcessorNode::createOutNode(CPoint &p) {
	GOutputNode::Ptr out = GOutputNode::create ( parentView, Resources::VSTPLUG_OUTPUT );
	out->moveTo ( p );
	return out;
}
//------------------------------------------------------------------------------------------------------------
void GProcessorNode::addIONodesOnView ( GObjectList &createdConnections )
{
	GConnection::Ptr con;
	InputNodeContainer::iterator iit = ins.begin();
	for ( ; iit!=ins.end(); ++iit ) { 
		con = GConnectionPrIn::create ( parentView, self.lock(), *iit );
		createdConnections.push_back ( con );
		parentView->addGObject ( *iit );
		parentView->addGObject ( con, CircuidView::CONNECTIONS );
	}
	OutputNodeContainer::iterator oit = outs.begin();
	for ( ; oit!=outs.end(); ++oit ) {  
		con = GConnectionPrOut::create ( parentView, self.lock(), *oit );
		createdConnections.push_back ( con );
		parentView->addGObject ( *oit );
		parentView->addGObject ( con, CircuidView::CONNECTIONS );
	}
}
} //namespace ppiGui


