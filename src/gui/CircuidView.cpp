#include "CircuidView.h"
#include "math.h"
namespace ppiGui{

//============================================================================================================
//	Klasse CircuidView:
//  Haupt View in der die ProcessorNode Schaltung erstellt, bearbeitet, entfernt werden kann.
//============================================================================================================

//------------------------------------------------------------------------------------------------------------
CircuidView::CircuidView ( const VSTGUI::CRect &size ) : 
	CView ( size ),
	background( resources->getResourceBitmap( Resources::BACKGROUND ) )
{
	offset (0,0);
	TOLOG ( com::MyString("+CircuidView.") );
}
//------------------------------------------------------------------------------------------------------------
void CircuidView::addGObject( const GObject::Ptr &gObj, int stage ){
	TRY_TO_LOCK_TIMED(updateLock);
	gObj->EventSender<OnMouseClick>::addEventListener ( this );
	gObjectBuffer.insert( pair <U, V> (stage, gObj) );
	gObj->setZPos ( stage );
}
//------------------------------------------------------------------------------------------------------------
void CircuidView::removeGObject( const GObject::Ptr &gObj ){
	gObj->EventSender<OnMouseClick>::removeEventListener ( this );
	gObj->EventSender<OnRemove>::notifyEventListeners ( this, OnRemove ( gObj.get() ) );
	GObjectStageBuffer::iterator it = gObjectBuffer.begin();
	TRY_TO_LOCK_TIMED(updateLock);
	while ( it!=gObjectBuffer.end() ) {
		GObjectStageBuffer::iterator del = it++;
		if ( (*del).second == gObj ) gObjectBuffer.erase( del );
	}
}
//------------------------------------------------------------------------------------------------------------
CircuidView::~CircuidView() {
	clear();
	TOLOG ( com::MyString ("-CircuidView.") );
}
//------------------------------------------------------------------------------------------------------------
void CircuidView::clear(){
	TRY_TO_LOCK_TIMED(updateLock);
	GObjectStageBuffer::iterator it = gObjectBuffer.begin();
	while ( it != gObjectBuffer.end() ){
		GObject::Ptr gObj = it->second;
		gObj->EventSender< OnDestroy<GObject> >::notifyEventListeners ( this, OnDestroy<GObject>( gObj.get() ) );
		gObjectBuffer.erase (it++);
	}
}
//------------------------------------------------------------------------------------------------------------
void CircuidView::draw(CDrawContext *pContext){
	TRY_TO_LOCK_TIMED(updateLock);
	// view zeichnen
	drawBackground(pContext);
	GObjectStageBuffer::iterator it = gObjectBuffer.begin();
	while ( it != gObjectBuffer.end() ){
		if ( (*it).second->isVisible() ) (*it).second->render ( pContext );
		++it;
	}
	setDirty(false);
}
//------------------------------------------------------------------------------------------------------------
GObject::Ptr CircuidView::getGObjectAt ( CPoint &p, int start, int end ){
	if ( gObjectBuffer.empty() ) return GObject::Ptr();
	if ( start <= end ){
		for ( int i = start; i<=end; ++i ){
			GObject::Ptr obj = getGObjectAtStage ( p, i );
			if ( obj ) return obj;
		}
	} else {
		for ( int i=start; i>=end; --i ){
			GObject::Ptr obj = getGObjectAtStage ( p, i );
			if ( obj ) return obj;
		}
	}
	return GObject::Ptr();
}
//------------------------------------------------------------------------------------------------------------
GObject::Ptr CircuidView::getGObjectAtStage ( CPoint &p, int stage ){
	if ( gObjectBuffer.empty() ) return GObject::Ptr();
	pair< GObjectStageBuffer::iterator, GObjectStageBuffer::iterator > ret;
	ret = gObjectBuffer.equal_range ( stage );
	GObjectStageBuffer::iterator it = ret.first;
	for ( ; it!=ret.second; ++it ){
		if ( (*it).second->hitTest(p) ){
			if ( (*it).second->isVisible() ) return (*it).second;
		}
	}
	return GObject::Ptr();
}

//------------------------------------------------------------------------------------------------------------
void CircuidView::mouse(CDrawContext *cc, CPoint &p, long btn) {
	GObject::Ptr src = getGObjectAt ( p, getHighestStage(),  getLowestStage() ); //hole object an position p
	if (src) { src->onMouse ( cc, p, btn ); }
	else sendMouseEvents ( cc, p, btn, this, this, NULL );
}
//------------------------------------------------------------------------------------------------------------
void CircuidView::eventHandler ( void *src, const OnMouseClick &ev ){
	EventSender<OnMouseClick>::notifyEventListeners ( this, OnMouseClick ( ev.src, ev.cD, ev.p, ev.btn ) );
}
//------------------------------------------------------------------------------------------------------------
inline void fillBackground ( CDrawContext *cC, 
							CBitmap *bmp,
							const VSTGUI::CRect &size, 
							const CPoint &offset 
							) {
	VSTGUI::CRect rect( 0, 0, bmp->getWidth(), bmp->getHeight() ); 
	cC->setFillColor ( white );
	cC->fillRect ( size );

	CCoord ox = offset.x % 15; // 15 == gridsize
	CCoord oy = offset.y % 15;
	
	for ( int y=oy; y<=size.height(); y+=bmp->getHeight() ) {
		for ( int x=ox; x<=size.width(); x+=bmp->getWidth() ) {
			VSTGUI::CRect rect( x, y, x + bmp->getWidth(), y + bmp->getHeight() ); 
			bmp->draw( cC, rect );
		}
	}
}
//------------------------------------------------------------------------------------------------------------
inline void CircuidView::drawBackground(CDrawContext *cC) {
	//draw bk
	fillBackground( cC, background, size, offset );
	string stamp =  Settings::versionToString() + "++" + __DATE__ + " " + __TIME__ + "++";
	stamp += " // (c)2011 www.vstforx.de";
	VSTGUI::CRect text( 5, size.top , stamp.length()*6, size.top + 17 );
	cC->setFont ( kNormalFontSmall );
	cC->setFontColor ( ppiGui::black );
	cC->drawString ( stamp.c_str(), text, false, kLeftText );
}
//------------------------------------------------------------------------------------------------------------
void CircuidView::offsetView( const CPoint &point ){
	GObjectStageBuffer::iterator it = gObjectBuffer.begin();
	while ( it != gObjectBuffer.end() ){
		(*it).second->offset ( point.x, point.y );
		++it;
	}
	offset.offset ( point.x, point.y );
	CView::setDirty();
}

//------------------------------------------------------------------------------------------------------------
void CircuidView::setDirty(ppiGui::GObject *gObj){
	CView::setDirty();
}
//------------------------------------------------------------------------------------------------------------
void CircuidView::getGObjectsAtStage ( list<V> &l, int stage ) {
	pair < GObjectStageBuffer::iterator, GObjectStageBuffer::iterator > ret;
	ret = gObjectBuffer.equal_range ( stage );
	GObjectStageBuffer::iterator it = ret.first;
	for ( ; it!=ret.second; ++it ) l.push_back ( it->second );
}
//------------------------------------------------------------------------------------------------------------
void CircuidView::getObjectsInArea(const VSTGUI::CRect &rect, std::list<GObject::Ptr> &v){
	if ( gObjectBuffer.empty() ) return;
	GObjectStageBuffer::iterator it = gObjectBuffer.end();
	do {
		--it;
		if ( !(*it).second->isVisible() ) continue;
		if ( rect.rectOverlap ( (*it).second->getSize() ) ) v.push_back ( (*it).second );
	} while ( it != gObjectBuffer.begin() );
}

} // namespace CircuidView


