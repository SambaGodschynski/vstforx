/*
	ppi.com.One4All :
	hier finden sich Allgemeingueltige Klassen und Definitionen.
*/

#ifndef COM_ONE4ALL_H
#define COM_ONE4ALL_H
#include "Log.h"
#include "Frame.h"
#include <stdio.h>
#include <list>
#include <string>
#include <vector>
#include <set>
#include "MyString.h"
#include "PPIError.h"
#include "Settings.h"
#include "Events.h"
#include <fstream>
#include <sstream>
#include <ostream>
#include <istream>
#include <map>
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "boost/function.hpp"
#include "boost/filesystem.hpp"
#include <sstream>
#include "Serialization.h"
#include "OS_Specific/OS_com.h"
#include <boost/thread.hpp>
#include <float.h>
#include "TList.h"
#include <sambag/com/FileSystem.hpp>
using namespace std;

#define MAX_STR 50 // char str[MAX_STR]

// fuegt string x ins log datei ein. -- NUR DEBUG -- 
#ifdef NDEBUG
#define DEBUG_CMD(X)
#define MESSAGE_BEEP
#else
#define DEBUG_CMD(X) X
#define MESSAGE_BEEP MessageBeep(0xFFFFFFFF);
#endif

#define __STR2__(x) #x
#define __STR1__(x) __STR2__(x)
#define __LOC__ __FILE__ "("__STR1__(__LINE__)") : Warning Msg: "

#define CLASS_NAME(c) typeid(c).name()
 

namespace com {
// Mutex
typedef boost::timed_mutex Mutex;
}

#define DEADLOCK_EXCEPTION com::ppiError::DeadlockException ("DeadlockException.", __FILE__, __LINE__)
#define LOCK_TIMEOUT 1
// scheitert zugriff auf mutex nach LOCK_TIMEOUT sec. wird DEADLOCK_EXCEPTION ausnahme geworfen.
#define TRY_TO_LOCK_TIMED(mutex) boost::unique_lock<boost::timed_mutex> __lock( (mutex), boost::try_to_lock);\
	if (!__lock.owns_lock()) { __lock.timed_lock(boost::get_system_time() + boost::posix_time::seconds(LOCK_TIMEOUT)); }\
	if ( !__lock.owns_lock() ) throw DEADLOCK_EXCEPTION;
#define TRY_TO_LOCK_TIMED2(mutex,timeout) boost::unique_lock<boost::timed_mutex> __lock( (mutex), boost::try_to_lock);\
	if (!__lock.owns_lock()) { __lock.timed_lock(boost::get_system_time() + boost::posix_time::seconds(timeout)); }\
	if ( !__lock.owns_lock() ) throw DEADLOCK_EXCEPTION;
#define TRY_TO_LOCK(mutex, _bool) boost::unique_lock<boost::timed_mutex> __lock( (mutex), boost::try_to_lock);\
								  _bool = __lock.owns_lock();
// Mutex	


namespace com {
	//========================================================================================================
	#define PI 3.14159265
	//========================================================================================================
	//NODEID
	typedef int NodeID;
	//Parameter Index
	typedef int ParameterIndex;
	//--------------------------------------------------------------------------------------------------------
	typedef float VstNumber;
	//========================================================================================================
 	template< class STLContainer >
	inline bool contains( const STLContainer &c, const typename STLContainer::iterator::value_type &obj ) {
		typename STLContainer::const_iterator it = c.begin();
		for ( ; it!=c.end(); ++it ){
			if ( *it==obj ) return true;
		}
		return false;
	}
	//========================================================================================================
 	template< class STLContainer >
	inline typename STLContainer::iterator  // rueckgabetyp
	find ( STLContainer &c, const typename STLContainer::iterator::value_type &obj ) {
		typename STLContainer::iterator it = c.begin();
		for ( ; it!=c.end(); ++it ){
			if ( *it==obj ) return it;
		}
		return it;

	}
	//========================================================================================================
 	// const variante
	template< class STLContainer >
	inline typename STLContainer::const_iterator  // rueckgabetyp
	find_const ( const STLContainer &c, const typename STLContainer::const_iterator::value_type &obj ) {
		typename STLContainer::const_iterator it = c.begin();
		for ( ; it!=c.end(); ++it ){
			if ( *it==obj ) return it;
		}
		return it;
	}
	//========================================================================================================
	// Bildet einen bruch wert von 0..1 auf einen ganzahligen wert von 0..n ab. 
	//========================================================================================================
	inline size_t mapInteger(float value, size_t n){ 
		if (value <= 0. )
			return 0;
		return int( n * min (value,0.99999f) ); 
	}
	//========================================================================================================
 	template<class T>
	inline void xChange( T &x, T &y  ){
		T tmp;
		tmp = x;
		x = y;
		y = tmp;
	}
	//========================================================================================================
	// returns true if flag setted in flags.
	inline bool isFlag ( int flag, int flags ) {
		return ( (flag & flags) == flag );
	}
	//========================================================================================================
 	template<typename T>
	inline bool compareTolerant( const T &x, const T &y, const T &tolereance ){
		T abs = x - y;
		if ( abs < 0 ) abs*=-1;
		return  abs <= tolereance ;
	}
	//========================================================================================================
	// Ordnet Oberklassen Zeiger V o1, o2 Unterklassen Zeiger t und u zu
	template < typename T, typename U, typename V >
	void relate ( T **t, U **u, V * o1, V * o2 ){
		// wenns der eine nicht ist muss es der andere sein
		if ( !( *t = dynamic_cast<T*>( o1 ) ) ) *t = dynamic_cast<T*>( o2 );
		*u = ( *t==o1 ) ? dynamic_cast<U*>( o2 ) : dynamic_cast<U*>( o1 );
	}
	using namespace boost::filesystem;
	//--------------------------------------------------------------------------------------------------------
	template < typename T >
	T getMax( const T& a, const T &b) { return (a > b) ? a : b; }
	//--------------------------------------------------------------------------------------------------------
	template < typename T >
	T getMin( const T& a, const T &b) { return (a < b) ? a : b; }
	//--------------------------------------------------------------------------------------------------------
	typedef sambag::com::Location Filename;
	//--------------------------------------------------------------------------------------------------------
	typedef set<sambag::com::Location> UniquePathList;
	//--------------------------------------------------------------------------------------------------------
	typedef list<sambag::com::Location> PathNameList;
	//--------------------------------------------------------------------------------------------------------
	typedef list<Filename> Filenames;
	//--------------------------------------------------------------------------------------------------------
	typedef list<string> StringList;
	//--------------------------------------------------------------------------------------------------------
	typedef boost::function< void ( const Filename& ) > FileFoundFuncPtr;
	//========================================================================================================
	bool isSubDirectory ( const sambag::com::Location &parent, const sambag::com::Location &sub );
	//========================================================================================================
	//	Klasse IHasState:
	//========================================================================================================
	//--------------------------------------------------------------------------------------------------------
	class IHasState {
	public:
		//----------------------------------------------------------------------------------------------------
		typedef boost::shared_ptr<IHasState> Ptr;
		//----------------------------------------------------------------------------------------------------
		virtual void setState( size_t state ) = 0 ;
		//----------------------------------------------------------------------------------------------------
		virtual size_t getState() const = 0;
	};
	/*//========================================================================================================
	//	Funktion: typeDetector.
	//  Gebeben werden TypeList und zeiger zu objekt .
	//  Geliefert wird der index zum typ aus der typelist vom Objekt.
	//  -1 wenn nicht gefunden
	//========================================================================================================
	template < typename T > template > class _TList  >
	int typeDetector ( typename T *ptr, int count = 0 ) {
		if ( dynamic_cast<_TList::Head*> (ptr) ) return count;
		return typeDetector<_TList::Tail> ( ptr, count + 1 );
	}
	template <>
	int typeDetector<TNull> ( typename T *ptr, int count  ) {
		return 0;
	}*/
};// namespace com
#endif



