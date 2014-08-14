/*
 * ===========================================================================================================
 * SyncTranslator.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#include "SyncTranslator.h"

namespace processing {
//============================================================================================================
// @class SyncTranslator
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
int SyncTranslator::translate ( float v ) {
    frx::processing::IHostInfo::Ptr hI = hostInfo.lock();
    if (!hI) {
        SAMBAG_THROW(sambag::com::exceptions::IllegalStateException,
            "Hostinfo == NULL"
        );
    }
    using namespace frx::processing;
    int n = com::mapInteger ( v, musicalValues::NUM_STDNOTES );
    TimeInfo *inf = hI->getHostTimeInfo( TimeInfo::FrxTempo );
    return note2Sample ( musicalValues::noteLengthTable[n].val, inf->tempo, inf->sampleRate );
}
//------------------------------------------------------------------------------------------------------------
com::MyString  SyncTranslator::translateAsString ( float v ) {
    int n = com::mapInteger ( v, musicalValues::NUM_STDNOTES );
    return musicalValues::noteLengthTable[n].str;
}
}// namespace processing
