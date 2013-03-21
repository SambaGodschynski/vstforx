/*
 * ===========================================================================================================
 * Log.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#ifndef LOGGING_H
#define LOGGING_H

#include <string>
#include <sambag/com/Common.hpp>

#define TOLOG(X) SAMBAG_LOG_INFO<<X
#define LOG_ASSERT(X) if (!(X)) { SAMBAG_LOG_FATAL<<"!ASSERTION ERROR in File:"<<__FILE__<<" ln: "<<__LINE__; }

#endif //LOGGING_H



