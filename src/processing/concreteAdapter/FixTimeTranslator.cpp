                                        /*
 * ===========================================================================================================
 * FixTimeTranslator.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#include "FixTimeTranslator.h"


namespace processing{
void FixTimeValue::serialize ( ::com::iArchive &ar, const unsigned int version ){
	ar & boost::serialization::base_object < ValueTranslator > ( *this );
	ar & oneMsInSamples;
	ar & minInSampl;
	ar & fak;
	ar & minInMs;
}
void FixTimeValue::serialize ( ::com::oArchive &ar, const unsigned int version ){
	ar & boost::serialization::base_object < ValueTranslator > ( *this );
	ar & oneMsInSamples;
	ar & minInSampl;
	ar & fak;
	ar & minInMs;
}
}// namespace processing
