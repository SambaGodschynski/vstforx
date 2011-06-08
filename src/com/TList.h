#ifndef TLIST
#define TLIST

#define TLIST_1(x) TList< x, TNull >
#define TLIST_2(x1,x2) TList< x1, TLIST_1(x2) >
#define TLIST_3(x1,x2,x3) TList< x1, TLIST_2(x2,x3) > 
#define TLIST_4(x1,x2,x3,x4) TList< x1, TLIST_3(x2,x3,x4) >
#define TLIST_5(x1,x2,x3,x4,x5) TList< x1, TLIST_4(x2,x3,x4,x5) >
#define TLIST_6(x1,x2,x3,x4,x5,x6) TList< x1, TLIST_5(x2,x3,x4,x5,x6) >
#define TLIST_7(x1,x2,x3,x4,x5,x6,x7) TList< x1, TLIST_6(x2,x3,x4,x5,x6,x7) >
#define TLIST_8(x1,x2,x3,x4,x5,x6,x7,x8) TList< x1, TLIST_7(x2,x3,x4,x5,x6,x7,x8) >
#define TLIST_9(x1,x2,x3,x4,x5,x6,x7,x8,x9) TList< x1, TLIST_8(x2,x3,x4,x5,x6,x7,x8,x9) >  
#define TLIST_10(x1,x2,x3,x4,x5,x6,x7,x8,x9,x10) TList< x1, TLIST_9(x2,x3,x4,x5,x6,x7,x8,x9,x10) >  

namespace com {
//=============================================================================
// TNull: Typenliste Null Eintrag
//=============================================================================
struct TNull {
	typedef void* Head;
 	enum { NUM = 0 };
	virtual ~TNull(){}
};
//=============================================================================
// TList: Typenliste
//=============================================================================
template < class H, class T >
struct TList {
	//-------------------------------------------------------------------------
	enum { NUM = 1 + T::NUM };
	//-------------------------------------------------------------------------
	typedef H Head;
	//-------------------------------------------------------------------------
	typedef T Tail;
};
} //namespace com
#endif
