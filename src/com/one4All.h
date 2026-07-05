/*
 * ===========================================================================================================
 * one4All.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#ifndef COM_ONE4ALL_H
#define COM_ONE4ALL_H
#include "Log.h"
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
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <functional>
#include <boost/tuple/tuple.hpp>
#include <sstream>
#include "com/SerializationFwd.h"
#include "OS_Specific/OS_com.h"
#include <mutex>
#include <chrono>
#include <float.h>
#include "TList.h"
#include <sambag/com/FileSystem.hpp>

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
typedef std::timed_mutex Mutex;
}

#define DEADLOCK_EXCEPTION com::ppiError::DeadlockException ("DeadlockException.", __FILE__, __LINE__)
#define LOCK_TIMEOUT 1
#define TRY_TO_LOCK_TIMED(mutex) std::unique_lock<std::timed_mutex> __lock( (mutex), std::try_to_lock);\
	if (!__lock.owns_lock()) { __lock.try_lock_for(std::chrono::seconds(LOCK_TIMEOUT)); }\
	if ( !__lock.owns_lock() ) throw DEADLOCK_EXCEPTION;
#define TRY_TO_LOCK_TIMED2(mutex,timeout) std::unique_lock<std::timed_mutex> __lock( (mutex), std::try_to_lock);\
	if (!__lock.owns_lock()) { __lock.try_lock_for(std::chrono::seconds(timeout)); }\
	if ( !__lock.owns_lock() ) throw DEADLOCK_EXCEPTION;
#define TRY_TO_LOCK(mutex, _bool) std::unique_lock<std::timed_mutex> __lock( (mutex), std::try_to_lock);\
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
		return int( n * std::min (value,0.99999f) ); 
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
    /**
     * maps form number of channels to number of in/output:
     * 1xstereo = 2 channels -> 1 in/output
     * 1xstereo+1xmono = 3 channels -> 2 in/output
     */
    inline size_t numChannels2Xputs(size_t numChannel) {
        return numChannel/2 + numChannel%2;
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
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @brief exists only because std::max fucks up sometimes on msvc with compile
	 * errors and I have no clue why.
	 */
	template < typename T >
	T getMax( const T& a, const T &b) { return (a > b) ? a : b; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @brief exists only because std::max fucks up sometimes on msvc with compile
	 * errors and I have no clue why.
	 */
	template < typename T >
	T getMin( const T& a, const T &b) { return (a < b) ? a : b; }
	//--------------------------------------------------------------------------------------------------------
	typedef sambag::com::Location Filename;
	//--------------------------------------------------------------------------------------------------------
	typedef std::set<sambag::com::Location> UniquePathList;
	//--------------------------------------------------------------------------------------------------------
	typedef std::list<sambag::com::Location> PathNameList;
	//--------------------------------------------------------------------------------------------------------
	typedef std::list<Filename> Filenames;
	//--------------------------------------------------------------------------------------------------------
	typedef std::list<std::string> StringList;
	//--------------------------------------------------------------------------------------------------------
	typedef std::function< void ( const Filename& ) > FileFoundFuncPtr;
    //--------------------------------------------------------------------------------------------------------
	/**
	 * @param fileName
	 * @return file name e.g:
	 * C:/VSTPlugin.dll => VSTPlugin
	 */
	std::string getFileNameFromPath (const std::string &fileName);
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
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @note If Plugin is shellplugtype specific shell id comes with filename(eg): "plugin.dll@172832".
	 * @return pair<filename, shellid>
	 */
	std::pair<std::string, std::string> extractVSTPluginFilename(const std::string &filename);
	//--------------------------------------------------------------------------------------------------------
    typedef boost::tuple<std::string, // namespace
                         std::string, // type
                         std::string, // name
                         int,         // numInputs , -1 for undefined
                         int,         // numOutputs, -1 for undefined
                         std::string  // details
            > IdData;
   /**
     * @brief parses creator ids. A creator id is composed of frx.$namespace.$type.$name[$detail][($numInputs, $numInputs)]
     * @note the descriptor "type" is a bit missleading its more thought as subnamespace 
	 * @note: eg. frx.processing.vst2x.FrxTestplugin(2,2) -> tuple(processing, vst2x, FrxTestplugin, 2, 2)
     *            frx.processing.vst2x.DelayX -> tuple(processing, vst2x, DelayX, -1, -1)
     *            frx.processing.internal.FrxADSR -> tuple(processing, internal, FrxADSR, -1, -1)
     *            frx.processing.vst2x.location('/home/plugins/plugin.vst') -> tuple(processing, vst2x, location, -1, -1, /home/plugins/plugin.vst)
	 */
    struct IdParser : public IdData
    {
        typedef IdData Data;
        IdParser(const std::string &str = "");
        IdParser(const Data &data) : Data(data) {}
        IdParser(const std::string &ns,
                            const std::string &tp,
                            const std::string &name,
                            int ni = -1,
                            int no = -1,
                            const std::string &dt = "") : Data(ns, tp, name, ni, no, dt)
        {
        }
        
        const Data & data() const { return *this; }
        void data(const Data &data) { *this = data; }
        
        const std::string & namespace_() const { return boost::get<0>(*this); }
        const std::string &       type() const { return boost::get<1>(*this); }
        const std::string &       name() const { return boost::get<2>(*this); }
        int                  numInputs() const { return boost::get<3>(*this); }
        int                 numOutputs() const { return boost::get<4>(*this); }
        const std::string &    details() const { return boost::get<5>(*this); }
        IdParser & namespace_(const std::string &val) { boost::get<0>(*this) = val; return *this; }
        IdParser &       type(const std::string &val) { boost::get<1>(*this) = val; return *this; }
        IdParser &       name(const std::string &val) { boost::get<2>(*this) = val; return *this; }
        IdParser &                 numInputs(int val) { boost::get<3>(*this) = val; return *this; }
        IdParser &                numOutputs(int val) { boost::get<4>(*this) = val; return *this; }
        IdParser &    details(const std::string &val) { boost::get<5>(*this) = val; return *this; }
        std::string toString() const;
        bool operator==(const IdParser &descr) const;
        bool operator!=(const IdParser &descr) const;
    };
    //------------------------------------------------------------------------------------------------------
    extern const IdParser FRX_NULL_ID;
    //------------------------------------------------------------------------------------------------------
    std::ostream & operator << (std::ostream &os, const IdParser &pd);
	//------------------------------------------------------------------------------------------------------
	std::string createVSTPluginFilename(const std::string &filename, const std::string &shellId);
	/*//====================================================================================================
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



