#include "PpiGui.h"
#include "ConcreteGObjects.h"
#include "PpiEditor.h"
#include "processing/parameter/ConnectionOperators.h"
#include "MainCtrl.h"
#include "CircuidView.h"

namespace ppiGui {

const CColor red = { 255, 0, 0, 255 };
const CColor white = { 255, 255, 255, 255 };
const CColor darkRed = { 155, 0, 0, 255 };
const CColor black = { 12, 12, 12, 255 };
const CColor blue = { 0, 0, 255, 255 };
const CColor lightBlue = { 144, 162, 191, 255 };
const CColor darkBlue = { 104, 122, 151, 255 };
const CColor green = { 0, 255, 0, 255 };
const CColor yellow = { 255, 255, 0, 255 };
const CColor grey = { 155, 155, 155, 255 };
const CColor lightGrey = { 170, 170, 170, 255 };
const CColor darkGrey = { 80, 80, 80, 255 };
const CColor bk = { 255, 255, 255, 255 };
const CColor grid = { 0, 0, 0, 255 };
const CColor alphaGrey = { 100, 100, 100, 255 };
const CColor alphaBlue = { 144, 162, 191, 255 };

//============================================================================================================
//	Klasse GSelection:
//  Auswahl eines oder mehrerer GObjecte.
//  Objekte lassen sich ueber auswahl bewegen.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void GSelection::offset(const CCoord &x, const CCoord &y) {
	GObjectList::iterator it = selection.begin();
	for (; it != selection.end(); ++it ){
		(*it)->offset ( x, y );
		(*it)->setObjectDirty();
	}
	GRect::offset ( x, y );
}
//------------------------------------------------------------------------------------------------------------
void GSelection::eventHandler ( void *src, const OnMouseDrag &ev ){
	if ( ev.btn != kLButton ) return;
	if ( ev.state == OnMouseDrag::DRAG ){
		offset ( MyPoint(ev.p) - MyPoint(ev.dragDirection)  );
		setObjectDirty();
	}
}
//------------------------------------------------------------------------------------------------------------
void GSelection::addGObject ( const GObject::Ptr &gObj ){ 
	if ( gObj.get() == this ) return;
	if ( contains(gObj) ) return;
	if ( dynamic_cast<GConnection*>(gObj.get()) ) return; // keine connections
	selection.push_back ( gObj ); 
}
//------------------------------------------------------------------------------------------------------------
inline void GSelection::drawBorders(CDrawContext *cd) {
	CDrawContextPlus cc(*cd);
	//  TODO: kann man optimieren, indem gleich beim hinzufuegen 
	// von GObjecten bBox aktualsiert wird.
	bBox = getBoundingBoxSelected(); 
	edgeLength = com::getMax<CCoord>( bBox.width()>>4, 14 );
	cc.setLineStyle ( lineStyle );
	cc.setLineWidth ( width );
	cc.setFrameColor ( color );
	cc.setFillColor ( bgColor );
	CPoint p = CPoint ( bBox.left, bBox.top );
	cc.drawLine ( p, CPoint ( p.x, p.y + edgeLength ) );
	cc.drawLine ( p, CPoint ( p.x + edgeLength, p.y ) );
	p.offset ( bBox.getWidth(), 0 );
	cc.drawLine ( p, CPoint ( p.x, p.y + edgeLength ) );
	cc.drawLine ( p, CPoint ( p.x - edgeLength, p.y ) );
	p.offset ( 0, bBox.getHeight() );
	cc.drawLine ( p, CPoint ( p.x, p.y - edgeLength ) );
	cc.drawLine ( p, CPoint ( p.x - edgeLength, p.y ) );
	p.offset ( -bBox.getWidth(), 0 );
	cc.drawLine ( p, CPoint ( p.x, p.y - edgeLength ) );
	cc.drawLine ( p, CPoint ( p.x + edgeLength, p.y ) );
	// create string
	std::string text = 
		selection.size() == 1 ? selection.front()->getName() : MyString( selection.size() ) + " selected objects";
	// draw string
	VSTGUI::CRect textRect ( bBox.left, bBox.bottom , bBox.left + 7 * text.length(), bBox.bottom + 15  );
	cc.setFont ( VSTGUI::kNormalFontSmaller );
	cc.setFontColor ( color );
	cc.drawString ( text.c_str(), textRect, false, kLeftText );
}
//------------------------------------------------------------------------------------------------------------
void GSelection::render(CDrawContext *cd) {
	if ( !isVisible() || selection.empty() ) return;
	drawBorders(cd);
}
//------------------------------------------------------------------------------------------------------------
//Liefert Bounding Box zur Selection.
VSTGUI::CRect GSelection::getBoundingBoxSelected() const {
	if ( selection.empty() ) return VSTGUI::CRect ( 0, 0, 0, 0 );
	VSTGUI::CRect tmp = ppiGui::getBoundingBox ( selection.begin(), selection.end() );
	tmp.inset ( -8, -8 );
	return tmp;
}
//============================================================================================================
//	Klasse GVSTPlugNode:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
GVSTPlugNode::GVSTPlugNode( ppiGui::CircuidView *parent ) : GProcessorNode ( parent ) {
	name = "unnamed VstPlugNode";
	skin = resources->getResourceBitmap(Resources::VSTPLUG_NODE);
	bBox = VSTGUI::CRect ( 0,0, skin->getWidth(), skin->getHeight() );
	focus = CPoint ( skin->getWidth()>>1, skin->getHeight()>>1 );
	radius = skin->getWidth()>>1;
	focus = CPoint (bBox.width()>>1, bBox.height()>>1);
	// EditButton erzeugen und zu subObjects hinzuf¸gen.
	GButton::Ptr edit = GButton::createNewEditButton ( parent );
	edit->moveTo ( CPoint ( skin->getWidth()/2, 
		                    skin->getHeight()/2  ) );
	subObjects.push_back ( edit );
	edit->EventSender<ButtonClicked>::addEventListener ( this );
	subObjects.push_back ( GFlag::create ( parent, this ) );
	getGFlag()->setPos ( 55.0f, 45 );
}
//------------------------------------------------------------------------------------------------------------
GFlag * GVSTPlugNode::getGFlag() const { 
	return dynamic_cast<GFlag*>( subObjects[FLAG].get() ); 
}
//------------------------------------------------------------------------------------------------------------
GButton * GVSTPlugNode::getEButton() { 
	return dynamic_cast<GButton*>( subObjects[EBTN].get() ); 
}
//------------------------------------------------------------------------------------------------------------
GVSTPlugNode::~GVSTPlugNode(){
}
//------------------------------------------------------------------------------------------------------------
void GVSTPlugNode::onMouse(CDrawContext *cc, CPoint &p, long btn) {
	if ( btn == kRButton ) {
		sendMouseEvents( cc, p, btn, this, this, this );
		return;
	}
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
void GVSTPlugNode::setName ( const string &name ) {
	getGFlag()->setUpperLabel ( name );
	GObject::setName ( name );
}
//------------------------------------------------------------------------------------------------------------
void GVSTPlugNode::render(CDrawContext *cc){
	if (!isVisible()) return;
	subObjects[ FLAG ]->render ( cc );
	skin->draw ( cc, bBox );
	subObjects[ EBTN ]->render ( cc );
}
//============================================================================================================
//	Klasse GConnection:
// Verbindung zwischen zwei GNodes dargestellt als Linie.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
GConnection::GConnection( CircuidView *parent, const GObject::Ptr &o1, const GObject::Ptr &o2 ) : 
o1( o1 ), 
o2( o2 ), 
onMouse ( false ),
GLine ( parent ){}
//------------------------------------------------------------------------------------------------------------
void GConnection::render(CDrawContext *cc){
	drawConnection ( cc );
}
//============================================================================================================
// Klasse GConnectionIO
// Standart Connection
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void GConnectionIO::drawConnection(CDrawContext *cc){
	if (!isVisible()) return;
	setStyle ( kLineSolid );
	setColor ( black );
	setWidth ( onMouse ? 6 : 4 );
	lineTo ( o1->getPos(), o2->getPos() );
	GLine::render (cc);
}
//============================================================================================================
// Klasse GConnectionPrIn
// Processor Input
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void GConnectionPrIn::drawConnection(CDrawContext *cc){
	if (!isVisible()) return;
	setStyle ( kLineSolid );
	setColor ( black );
	setWidth ( 6 );
	lineTo ( o1->getPos(), o2->getPos() );
	GLine::render (cc);
}
//============================================================================================================
// Klasse GConnectionPrOut
// Processor Output
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void GConnectionPrOut::drawConnection(CDrawContext *cc){
	if (!isVisible()) return;
	setStyle ( kLineSolid );
	setColor ( black );
	setWidth ( 2 );
	lineTo ( o1->getPos(), o2->getPos() );
	GLine::render (cc);
}
//============================================================================================================
// Klasse GConnectionPaPa
// Parameter Parameter
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void GConnectionPaPa::drawConnection(CDrawContext *cc) {
	if (!isVisible()) return;
	CColor col = { 255, 00, 217, 255 };
	setStyle ( kLineOnOffDash );
	setColor ( col );
	setWidth ( onMouse ? 2 : 1 );
	lineTo ( o1->getPos(), o2->getPos() );
	GLine::render (cc);
}
//============================================================================================================
// Klasse GConnectionPrPa
// Processor Parameter
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void GConnectionPrPa::drawConnection(CDrawContext *cc){
	if (!isVisible()) return;
	setStyle ( kLineOnOffDash );
	setColor ( black );
	setWidth ( 1 );
	lineTo ( o1->getPos(), o2->getPos() );
	GLine::render (cc);
}
//============================================================================================================
// Klasse GConnectionCoPa
// Connection Parameter
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
void GConnectionCoPa::drawConnection(CDrawContext *cc){
	if (!isVisible()) return;
	setStyle ( kLineOnOffDash );
	setColor ( black );
	setWidth ( 1 );
	lineTo ( o1->getPos(), o2->getPos() );
	GLine::render (cc);
}
//============================================================================================================
// Klasse GInputNode:
// Repraesentiert den Eingangs Processor Node
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
GInputNode::GInputNode(CircuidView *parent, Resources::BitmapID skinResID) :
skinResID(skinResID), GIONode ( parent )
{
	skin = resources->getResourceBitmap( skinResID );
	bBox = VSTGUI::CRect ( 0,0, skin->getWidth(), skin->getHeight() );
	focus = CPoint ( skin->getWidth()>>1, skin->getHeight()>>1 );
	radius = skin->getWidth()>>1;
	name = "Exit Node";
	//ConnectionNode verschieben
	focus = CPoint (bBox.width()>>1, bBox.height()>>1);
}
//------------------------------------------------------------------------------------------------------------
GInputNode::~GInputNode(){
}
//------------------------------------------------------------------------------------------------------------
void GInputNode::render(CDrawContext *cc){
	if (!isVisible()) return;
	skin->draw ( cc, bBox );
}
//============================================================================================================
// Klasse GOutputNode:
// Repraesentiert den Ausgangs Processor Node
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
GOutputNode::GOutputNode(ppiGui::CircuidView *parent, Resources::BitmapID skinResID ) : 
skinResID(skinResID), GIONode ( parent ){
	skin = resources->getResourceBitmap( skinResID );
	bBox = VSTGUI::CRect ( 0,0, skin->getWidth(), skin->getHeight() );
	focus = CPoint ( skin->getWidth()>>1, skin->getHeight()>>1 );
	radius = skin->getWidth()>>1;
	name = "Entry Node";
	//ConnectionNode verschieben
	focus = CPoint (bBox.width()>>1, bBox.height()>>1);
}
//------------------------------------------------------------------------------------------------------------
GOutputNode::~GOutputNode() {
}
//------------------------------------------------------------------------------------------------------------
void GOutputNode::render (CDrawContext *cc){
	if (!isVisible()) return;
	skin->draw ( cc, bBox );
}
//============================================================================================================
//	Klasse GVolumeNode:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
GVolumeNode::GVolumeNode(ppiGui::CircuidView *view ) : GProcessorNode ( view ){
	name = "volume";
	skin = resources->getResourceBitmap( Resources::VOLUME_ADAPTER );
	bBox = VSTGUI::CRect ( 0,0, skin->getWidth(), skin->getHeight() );
	focus = CPoint ( skin->getWidth()>>1, skin->getHeight()>>1 );
	radius = skin->getWidth()>>1;
	focus = CPoint (bBox.width()>>1, bBox.height()>>1);
}
//------------------------------------------------------------------------------------------------------------
void GVolumeNode::render ( CDrawContext *cc ){
	skin->draw ( cc, bBox );
}
//------------------------------------------------------------------------------------------------------------
GVolumeNode::~GVolumeNode(){
}
//============================================================================================================
//	Klasse GPanAdapter:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
GPanAdapter::GPanAdapter( ppiGui::CircuidView *view ) : GProcessorNode ( view ){
	name = "pan";
	skin = resources->getResourceBitmap( Resources::CHANNEL_SPLIT_ADAPTER );
	bBox = VSTGUI::CRect ( 0,0, skin->getWidth(), skin->getHeight() );
	focus = CPoint ( skin->getWidth()>>1, skin->getHeight()>>1 );
	radius = skin->getWidth()>>1;
	focus = CPoint (bBox.width()>>1, bBox.height()>>1);
}
//------------------------------------------------------------------------------------------------------------
void GPanAdapter::render ( CDrawContext *cc ){
	skin->draw ( cc, bBox );
}
//------------------------------------------------------------------------------------------------------------
GPanAdapter::~GPanAdapter(){
}
//============================================================================================================
//	Klasse GOutputStepNode:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
GOutputStepNode::GOutputStepNode(ppiGui::CircuidView *view ) : 
GProcessorNode ( view ), 
aONodeID(UINT_MAX),
glow( GBitmap::create ( view, Resources::VSTPLUG_OUTPUT_GLOW ) )
{
	name = "output_step";
	skin = resources->getResourceBitmap( Resources::STEP_ADAPTER );
	bBox = VSTGUI::CRect ( 0,0, skin->getWidth(), skin->getHeight() );
	focus = CPoint ( skin->getWidth()>>1, skin->getHeight()>>1 );
	radius = skin->getWidth()>>1;
	focus = CPoint ( bBox.width()>>1, bBox.height()>>1 );
}
//------------------------------------------------------------------------------------------------------------
void GOutputStepNode::render ( CDrawContext *cd ){
	skin->draw ( cd, bBox );
	const OutputNodeContainer &oc = getOutputNodes();
	// render active ONode mark
	if ( aONodeID >= oc.size() ) return;
	GObject::Ptr o = oc.at(aONodeID);
	CPoint p = o->getPos();
	glow->moveTo (p);
	glow->render(cd);
}
//------------------------------------------------------------------------------------------------------------
GOutputStepNode::~GOutputStepNode(){
}
//============================================================================================================
//	Klasse GInputStepNode:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
GInputStepNode::GInputStepNode(ppiGui::CircuidView *view ) : 
GProcessorNode ( view ), 
aINodeID(UINT_MAX),
glow( GBitmap::create ( view, Resources::VSTPLUG_OUTPUT_GLOW ) )
{
	name = "input_step";
	skin = resources->getResourceBitmap( Resources::STEP_ADAPTER );
	bBox = VSTGUI::CRect ( 0,0, skin->getWidth(), skin->getHeight() );
	focus = CPoint ( skin->getWidth()>>1, skin->getHeight()>>1 );
	radius = skin->getWidth()>>1;
	focus = CPoint ( bBox.width()>>1, bBox.height()>>1 );
}
//------------------------------------------------------------------------------------------------------------
void GInputStepNode::render ( CDrawContext *cd ){
	skin->draw ( cd, bBox );
	const InputNodeContainer &ic = getInputNodes();
	// render active ONode mark
	if ( aINodeID >= ic.size() ) return;
	GObject::Ptr o = ic.at(aINodeID);
	CPoint p = o->getPos();
	glow->moveTo (p);
	glow->render(cd);
}
//------------------------------------------------------------------------------------------------------------
GInputStepNode::~GInputStepNode(){
}
//============================================================================================================
//	Klasse GOutputSwitch:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
GOutputSwitch::GOutputSwitch(ppiGui::CircuidView *view ) : 
GProcessorNode ( view ), 
aONodeID(UINT_MAX),
glow( GBitmap::create ( view, Resources::VSTPLUG_OUTPUT_GLOW ) )
{
	name = "output_switch";
	skin =resources->getResourceBitmap( Resources::OSWITCH_ADAPTER);
	bBox = VSTGUI::CRect ( 0,0, skin->getWidth(), skin->getHeight() );
	focus = CPoint ( skin->getWidth()>>1, skin->getHeight()>>1 );
	radius = skin->getWidth()>>1;
	focus = CPoint ( bBox.width()>>1, bBox.height()>>1 );
}
//------------------------------------------------------------------------------------------------------------
void GOutputSwitch::render ( CDrawContext *cc ){
	skin->draw ( cc, bBox );
	const OutputNodeContainer &oc = getOutputNodes();
	// render active ONode mark
	if ( aONodeID >= oc.size() ) return;
	GObject::Ptr o = oc.at(aONodeID);
	CPoint p = o->getPos();
	glow->moveTo (p);
	glow->render(cc);
}
//------------------------------------------------------------------------------------------------------------
GOutputSwitch::~GOutputSwitch(){
}
//============================================================================================================
//	Klasse GInputSwitch:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
GInputSwitch::GInputSwitch(ppiGui::CircuidView *view ) : 
GProcessorNode ( view ), 
aINodeID(UINT_MAX),
glow( GBitmap::create ( view, Resources::VSTPLUG_OUTPUT_GLOW ) )
{
	name = "input_switch";
	skin =resources->getResourceBitmap( Resources::OSWITCH_ADAPTER);
	bBox = VSTGUI::CRect ( 0,0, skin->getWidth(), skin->getHeight() );
	focus = CPoint ( skin->getWidth()>>1, skin->getHeight()>>1 );
	radius = skin->getWidth()>>1;
	focus = CPoint ( bBox.width()>>1, bBox.height()>>1 );
}
//------------------------------------------------------------------------------------------------------------
void GInputSwitch::render ( CDrawContext *cc ){
	skin->draw ( cc, bBox );
	const InputNodeContainer &oc = getInputNodes();
	// render active ONode mark
	if ( aINodeID >= oc.size() ) return;
	GObject::Ptr o = oc.at(aINodeID);
	CPoint p = o->getPos();
	glow->moveTo (p);
	glow->render(cc);
}
//------------------------------------------------------------------------------------------------------------
GInputSwitch::~GInputSwitch(){
}
//============================================================================================================
//	Klasse GPeakTracker:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
GPeakTracker::GPeakTracker(ppiGui::CircuidView *view ) : GProcessorNode ( view ){
	name = "peak_tracker";
	skin = resources->getResourceBitmap( Resources::PEAK_TRACK_ADAPTER );
	bBox = VSTGUI::CRect ( 0,0, skin->getWidth(), skin->getHeight() );
	focus = CPoint ( skin->getWidth()>>1, skin->getHeight()>>1 );
	radius = skin->getWidth()>>1;
	focus = CPoint (bBox.width()>>1, bBox.height()>>1);
}
//------------------------------------------------------------------------------------------------------------
void GPeakTracker::render ( CDrawContext *cc ){
	skin->draw ( cc, bBox );
}
//------------------------------------------------------------------------------------------------------------
GPeakTracker::~GPeakTracker(){
}
//============================================================================================================
//	Klasse GADSRTrigger:
//  Triggert Eingangssignal als Gate fuer ADSR.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
GADSRTrigger::GADSRTrigger(ppiGui::CircuidView *view ) : GProcessorNode ( view ){
	name = "adsr_trigger";
	skin = resources->getResourceBitmap( Resources::ADSR_ADAPTER);
	bBox = VSTGUI::CRect ( 0,0, skin->getWidth(), skin->getHeight() );
	focus = CPoint ( skin->getWidth()>>1, skin->getHeight()>>1 );
	radius = skin->getWidth()>>1;
	focus = CPoint (bBox.width()>>1, bBox.height()>>1);
}
//------------------------------------------------------------------------------------------------------------
void GADSRTrigger::render ( CDrawContext *cc ){
	skin->draw ( cc, bBox );
}
//------------------------------------------------------------------------------------------------------------
GADSRTrigger::~GADSRTrigger(){
}
//============================================================================================================
//	Klasse GMidiProcessor:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
GMidiProcessor::GMidiProcessor(ppiGui::CircuidView *view ) : GProcessorNode ( view ){
	name = "midi_receiver";
	skin = resources->getResourceBitmap( Resources::MIDI_RECEIVER );
	bBox = VSTGUI::CRect ( 0,0, skin->getWidth(), skin->getHeight() );
	focus = CPoint ( skin->getWidth()>>1, skin->getHeight()>>1 );
	radius = skin->getWidth()>>1;
	focus = CPoint (bBox.width()>>1, bBox.height()>>1);
}
//------------------------------------------------------------------------------------------------------------
void GMidiProcessor::render ( CDrawContext *cc ){
	skin->draw ( cc, bBox );
}
//------------------------------------------------------------------------------------------------------------
GMidiProcessor::~GMidiProcessor(){
}
//============================================================================================================
//	Klasse GKnob:
//  Ein GKnob Objekt ist ein ueber eine Mausaktion regelbares Grafisches Objekt. 
//  Erbt von VSTGUI::CAnimKob.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
GKnob::GKnob ( CircuidView *parent, CAnimKnob *knob ) : 
	CControlWrapper<GCircle> ( parent, knob ), enabled ( true )
{
	name = "Knob";
	gFlag = GFlag::create ( parent, this );
	gFlag->setPos ( 45.0f, 35 ); 
	subObjects.push_back ( gFlag );
}
//------------------------------------------------------------------------------------------------------------
CAnimKnob * GKnob::createNewCAnimKnob ( Resources::BitmapID resId, int subPix ) {
	CBitmap *bmp = resources->getResourceBitmap ( resId );
	VSTGUI::CRect size ( 0, 0, bmp->getWidth(), bmp->getHeight()/subPix );
	CPoint p;
	CAnimKnob *neu = new CAnimKnob ( size, NULL, 0, subPix, bmp->getHeight()/subPix, bmp, p );
	return neu;
}
//------------------------------------------------------------------------------------------------------------
void GKnob::save( oArchive &ar, const unsigned int version ) const {
	ar << boost::serialization::base_object<GObject>(*this);
	string upper, lower;
	upper = getHeader();
	lower = getDisplay();
	ar << upper;
	ar << lower;
	float value = getValue();
	ar << value;
}
//------------------------------------------------------------------------------------------------------------
void GKnob::load( iArchive &ar, const unsigned int version ) {
	ar >> boost::serialization::base_object<GObject>(*this);
	string upper, lower;
	ar >> upper;
	ar >> lower;
	setHeader( upper );
	setDisplay ( lower );
	float value;
	ar >> value;
	setValue( value );
}
//------------------------------------------------------------------------------------------------------------
void GKnob::endEdit (){
	//TODO:
	/*CmdChangeParameter cmd ( parameter, cView->getValue(), cView->getOldValue() );
	cmd.execute();*/
}
//------------------------------------------------------------------------------------------------------------
GKnob::~GKnob(){}
//============================================================================================================
//	Klasse GStdKnob: 
//  Standart Dreh-Knopf.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
GStdKnob::GStdKnob( CircuidView *view ) : 
	GKnob ( view, GKnob::createNewCAnimKnob( Resources::STD_KNOB, Settings::STD_KNOB_SUBPIXMAPS ) )
{
	setRadius(12);
	cView->setListener ( this );
}
//============================================================================================================
//	Klasse GPassiveKnob: 
//  Knopf dessen Wert nicht direkt per Maus eingestellt werden kann. 
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
GPassiveKnob::GPassiveKnob( CircuidView *view ) : 
GKnob ( view, GKnob::createNewCAnimKnob(Resources::PASSIVE_KNOB, Settings::STD_KNOB_SUBPIXMAPS ) )
{
	setRadius(12);
	setEnabled(false);
	cView->setListener ( this );
}
//============================================================================================================
//	Klasse GButton:
//  Erbt von VSTGUI::CMovieButton.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
GButton::GButton ( CircuidView *parent, CMovieButton *btn ) :  CControlWrapper<GCircle> ( parent, btn ) { 
	name = "Button";
}
//------------------------------------------------------------------------------------------------------------
// erzeugt VSTPlugNode Edit-Button.
GButton::Ptr GButton::createNewEditButton(ppiGui::CircuidView *view){
	CBitmap *bmp = resources->getResourceBitmap( Resources::EDIT_BUTTON );
	VSTGUI::CRect size ( 0, 0, bmp->getWidth(), bmp->getHeight() / 2 );
	CPoint p;
	CMovieButton *btn = new CMovieButton ( size, NULL, 0, bmp->getHeight() / 2, bmp, p );
	GButton::Ptr neu ( new GButton( view, btn ) );
	neu->setRadius(15);
	btn->setListener ( neu.get() );
	neu->_setSelfPtr ( neu );
	return neu;
}
//------------------------------------------------------------------------------------------------------------
void GButton::valueChanged ( CDrawContext *cc, CControl *pControl ){
	setObjectDirty();
	EventSender<ButtonClicked>::notifyEventListeners ( this, ButtonClicked ( this ) );
}
//------------------------------------------------------------------------------------------------------------
GButton::~GButton(){
}
//============================================================================================================
//	Klasse GFlag:
//  Ein an einem GObject anliegendes Label
//  mit einer verbindungslinie zum GObject.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
GFlag::GFlag ( CircuidView *view, GObject *parent ): GObject ( view, "Flag" ){
	GFlag::src = parent;
	upper = "";
	lower = "";
	setPos ( 45.0f, 35 );
	focus = CPoint (bBox.left, bBox.top );
}
//------------------------------------------------------------------------------------------------------------
void GFlag::setPos ( float angle, int distance ) {
	CPoint m = src->getPos();
	CPoint x (m);
	x.offset ( 0, -(int)distance );
	rotate ( x, angle, m );
	bBox = VSTGUI::CRect ( x.x, x.y, x.x, x.y + 16 );
}
//------------------------------------------------------------------------------------------------------------
void GFlag::render ( CDrawContext *cd ){
	CDrawContextPlus cc(*cd);
	//laenge ermitteln
	int maxStrLength = com::getMax<size_t>(getUpperLabel().length(), getLowerLabel().length() );
	if ( maxStrLength <= 0 ) return;
	bBox.setWidth ( 6 * maxStrLength );
	// linien zeichnen
	cc.setLineStyle ( kLineSolid );
	cc.setLineWidth ( 1 );
	cc.setFrameColor ( lightGrey );
	cc.drawLine ( src->getPos(), CPoint (bBox.left, bBox.top) );
	cc.drawLine ( CPoint (bBox.left, bBox.top), CPoint (bBox.right, bBox.top) );
	// zeichne strings
	VSTGUI::CRect tmp = bBox;
	cc.setFontColor ( black );
	cc.setFont ( kNormalFontSmall );
	tmp.offset ( 0 , - 13 );
	cc.drawString ( upper.c_str(), tmp, false, kLeftText );
	tmp.offset ( 0 ,   12 );
	cc.drawString ( ( lower + " " ).c_str(), tmp, false, kLeftText );
}
//============================================================================================================
//	Klasse PlaceGObject:
//  Ein Place Objekt implementiert den Vorgang ein Objekt auf der View zu plazieren.
//  Es wird mit einem GObject Initalisiert und der View hinzugefuegt.
//  Dann folgt es solange der Maus bis ein linksklick erfolgt ist. Jetzt wird das enthaltene
//  GObject an Ort und stelle der view hinzugefuegt und das PlaceGObject entfernt sich aus der view.
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
PlaceGObject::PlaceGObject ( CircuidView *parent, const GObject::Ptr &content ) : GObject ( parent ){
	gObjList.push_back(content);
	bBox = content->getSize();
	focus = content->getPos();
	// IdleListener registrieren
	PpiEditor *ed = static_cast<PpiEditor*> ( getParentView()->getEditor() );
	ed->EventSender<OnIdle>::addEventListener ( this );
}
//------------------------------------------------------------------------------------------------------------
PlaceGObject::PlaceGObject ( CircuidView *parent, const GObjList &content ) : GObject ( parent ){
	gObjList = content; 
	bBox = normalizeRect( getBoundingBox( gObjList.begin(), gObjList.end() ) );
	focus = CPoint ( bBox.left + bBox.width()/2, bBox.top + bBox.height()/2 ); 
	// IdleListener registrieren
	PpiEditor *ed = static_cast<PpiEditor*> ( getParentView()->getEditor() );
	ed->EventSender<OnIdle>::addEventListener ( this );
}
//------------------------------------------------------------------------------------------------------------
PlaceGObject::~PlaceGObject(){
	// MouseMotionListener austragen
	PpiEditor *ed = static_cast<PpiEditor*> ( getParentView()->getEditor() );
	ed->EventSender<OnIdle>::removeEventListener ( this );
}
//------------------------------------------------------------------------------------------------------------
void PlaceGObject::onMouse ( CDrawContext *cc, CPoint &p, long btn ){
	// objekte platzieren
	GObjectList::iterator it = gObjList.begin();
	for ( ; it!=gObjList.end(); ) {
		parentView->addGObject ( *it );
		it = gObjList.erase(it);
	}
	PpiEditor *ed = static_cast<PpiEditor*> ( getParentView()->getEditor() );
	ed->EventSender<OnIdle>::removeEventListener ( this );
	parentView->removeGObject ( self.lock() );
	parentView->setDirty(this);
}
//------------------------------------------------------------------------------------------------------------
void PlaceGObject::eventHandler ( void *src, const OnIdle &ev ) {
	CDrawContext *cD = getParentView()->getFrame()->createDrawContext();
	CPoint p; cD->getMouseLocation(p);
	moveTo (p);
	setObjectDirty();
	cD->forget();
}
//------------------------------------------------------------------------------------------------------------
void PlaceGObject::moveTo ( CPoint &p ) {
	GObject::moveTo(p); 
}
//------------------------------------------------------------------------------------------------------------
void PlaceGObject::offset ( const CCoord &x, const CCoord &y ){ 
	GObject::offset(x,y); 
	GObjectList::iterator it = gObjList.begin();
	for ( ; it!=gObjList.end(); ++it ) {
		(*it)->offset(x,y);
	}
}
//------------------------------------------------------------------------------------------------------------
void PlaceGObject::render ( CDrawContext *cd ){
	CDrawContextPlus cc(*cd);
	cc.setLineWidth ( 2 );
	cc.setFrameColor ( red );
	cc.setLineStyle ( kLineOnOffDash );
	VSTGUI::CRect r = bBox;
	r.inset ( -5, -5 );
	cc.drawRect ( r );
	GObjectList::iterator it = gObjList.begin();
	for ( ; it!=gObjList.end(); ++it ) {
		(*it)->render ( cd );	
	}
	cc.setLineStyle ( kLineSolid );
}
} // namespace ppiGui
