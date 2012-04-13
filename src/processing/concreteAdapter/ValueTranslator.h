/*
 * ===========================================================================================================
 * ValueTranslator.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#ifndef FORX_VALUETRANSLATOR_H
#define FORX_VALUETRANSLATOR_H

#include "com/MyString.h"
#include "com/Serialization.h"


namespace processing {
//============================================================================================================
/**
 * @interface ValueTranslator.
 * Transformiert einen Eingabewert im bereich 0..1, in einen Zeitwert in Samples.
 */
class ValueTranslator {
//============================================================================================================
friend class boost::serialization::access;
private:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * (De)Serialisiert ValueTranslator
	 * @param ar boost::Archive-Objekt
	 * @param version
	 */
template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ){}
protected:
public:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * // liefert Wert als String zb. 500 oder 1/4 , ...
	 * @param v
	 * @return MyString-Objekt
	 */
	virtual com::MyString  translateAsString ( float v ) = 0; 
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param v
	 * @return zu einem Eingabewert im bereich 0..1, in eine Zeitangabe in Samples.
	 */
	virtual int translate( float v ) = 0; 
	//--------------------------------------------------------------------------------------------------------
	virtual ~ValueTranslator(){}
};
}// namespace processing

#endif  // FORX_VALUETRANSLATOR_H


