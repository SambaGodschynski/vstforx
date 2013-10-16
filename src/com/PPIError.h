/*
 * ===========================================================================================================
 * PPIError.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#ifndef PPIERROR
#define PPIERROR

#include <string>
#include "Events.h"
#include <exception>

namespace com{
namespace ppiError{
//------------------------------------------------------------------------------------------------------------
// Basis Klasse fuer alle PPI Fehler
class PPIError;
//------------------------------------------------------------------------------------------------------------
class StackUnderflow;
class NullPointer;
class SystemError;
class GraphIncomplete;
class AlgorithmError;
class PushFrameError;
class FileIOException;
class SettingsNotInitialized;
class StageBufferException;
class IteratorError;
class IllegalOperation;
class IllegalGraph;
class DllError;
class TypeError;
class IndexOutOfBoundException;
class DeadlockException;
class DataError;
class SizeError;
class MapError;
class InvalidBlockSize;
class InvalidSampleRate;
class OwnerLock;
class SettingsException;
class SerializationError;
}//namespace ppiError
}//namespace com

//TODO: ueberall zweite konstruktor variante (src, line, addinfo) hinzufuegen.

//============================================================================================================
// Klasse: PPIError.
// Basisklasse fuer alle Fehler.
//============================================================================================================
namespace com{
namespace ppiError{

#define UKWNSRC "unkown source."

class PPIError : public std::exception {
public:
	std::string error;
	std::string src;
    mutable std::string tmp;
	long linenumber;
	//--------------------------------------------------------------------------------------------------------
	std::string src2String() const; 
	//--------------------------------------------------------------------------------------------------------
	std::string toString() const {
		return "! Exception: " + error + " throwed in: " + src2String();
	}
	//--------------------------------------------------------------------------------------------------------
	PPIError(std::string err="unkown error.", std::string source=UKWNSRC, long line=-1);
	//--------------------------------------------------------------------------------------------------------
	void showMessage();
    //-------------------------------------------------------------------------------------------------------
	virtual const char* what() const throw();
	//-------------------------------------------------------------------------------------------------------
	virtual ~PPIError() throw()
	{
	}
};
//============================================================================================================
// Klasse: StackUnderflow.
//============================================================================================================
class StackUnderflow : public PPIError{
public:
	//--------------------------------------------------------------------------------------------------------
	StackUnderflow(std::string err="Stack Underflow.", std::string src=UKWNSRC, long line=-1) : PPIError(err,src,line){};
};
//============================================================================================================
// Klasse: DeadlockException.
//============================================================================================================
class DeadlockException : public PPIError{
public:
	//--------------------------------------------------------------------------------------------------------
	DeadlockException(std::string err="DeadlockException.", 
		              std::string src=UKWNSRC, long line=-1) : PPIError(err,src,line){};
};
//============================================================================================================
// Klasse: NullPointer.
//============================================================================================================
class NullPointer : public PPIError{
public:
	//--------------------------------------------------------------------------------------------------------
	NullPointer(std::string err="NullPointer", std::string src=UKWNSRC, long line=-1) : PPIError(err,src,line){};
};

//============================================================================================================
// Klasse: GraphIncomplete.
//============================================================================================================
class GraphIncomplete : public PPIError{
public:
	//--------------------------------------------------------------------------------------------------------
	GraphIncomplete(std::string err="Graph Incomplete", std::string src=UKWNSRC, long line=-1) : PPIError(err,src,line){};
};
//============================================================================================================
// Klasse: AlgorithmError.
//============================================================================================================
class AlgorithmError : public PPIError{
public:
	//--------------------------------------------------------------------------------------------------------
	AlgorithmError(std::string err="AlgorithmError", std::string src=UKWNSRC, long line=-1) : PPIError(err,src,line){};
};
//============================================================================================================
// Klasse: IllegalGraph.
//============================================================================================================
class IllegalGraph : public PPIError{
public:
	//--------------------------------------------------------------------------------------------------------
	IllegalGraph(std::string err="Illegal Graph", std::string src=UKWNSRC, long line=-1) : PPIError(err,src,line){};
};
//============================================================================================================
// Klasse: FileIOException.
//============================================================================================================
class FileIOException : public PPIError{
public:
	//--------------------------------------------------------------------------------------------------------
	FileIOException(std::string err="PushFrameError", std::string src=UKWNSRC, long line=-1) : PPIError(err,src,line){};
};
//============================================================================================================
// Klasse: PushFrameError.
//============================================================================================================
class PushFrameError : public PPIError{
public:
	//--------------------------------------------------------------------------------------------------------
	PushFrameError(std::string err="PushFrameError", std::string src=UKWNSRC, long line=-1) : PPIError(err,src,line){};
};

//============================================================================================================
// Klasse: StageBufferException.
//============================================================================================================
class StageBufferException : public PPIError{
public:
	//--------------------------------------------------------------------------------------------------------
	StageBufferException(std::string err="StageBufferException", std::string src=UKWNSRC, long line=-1) : PPIError(err,src,line){};
};

//============================================================================================================
// Klasse: IteratorError.
//============================================================================================================
class IteratorError : public StageBufferException {
public:
	//--------------------------------------------------------------------------------------------------------
	IteratorError(std::string err="StageBufferException: Iterator Error", std::string src=UKWNSRC, long line=-1) : StageBufferException(err,src,line){};
};

//============================================================================================================
// Klasse: IteratorError.
//============================================================================================================
class SettingsNotInitialized : public PPIError {
public:
	//--------------------------------------------------------------------------------------------------------
	SettingsNotInitialized(std::string err="SettingsNotInitialized", std::string src=UKWNSRC, long line=-1) : PPIError(err,src,line){};
};

//============================================================================================================
// Klasse: IllegalOperation.
//============================================================================================================
class IllegalOperation : public PPIError {
public:
	//--------------------------------------------------------------------------------------------------------
	IllegalOperation(std::string err="IllegalOperation", std::string src=UKWNSRC, long line=-1) : PPIError(err,src,line){};
};

//============================================================================================================
// Klasse: IllegalOperation.
//============================================================================================================
class TypeError : public PPIError {
public:
	//--------------------------------------------------------------------------------------------------------
	TypeError(std::string err="TypeError", std::string src=UKWNSRC, long line=-1) : PPIError(err,src,line){};
};

//============================================================================================================
// Klasse: IndexOutOfBoundException.
//============================================================================================================
class IndexOutOfBoundException : public PPIError {
public:
	//--------------------------------------------------------------------------------------------------------
	IndexOutOfBoundException(std::string err="IndexOutOfBoundException", std::string src=UKWNSRC, long line=-1) : PPIError(err,src,line){};
};

//============================================================================================================
// Klasse: DllError.
//============================================================================================================
class DllError : public PPIError {
public:
	//--------------------------------------------------------------------------------------------------------
	DllError(std::string err="DllError", std::string src=UKWNSRC, long line=-1) : PPIError (err,src,line){};
};

//============================================================================================================
// Klasse: DataError.
//============================================================================================================
class DataError : public PPIError {
public:
	//--------------------------------------------------------------------------------------------------------
	DataError(std::string err="DataError", std::string src=UKWNSRC, long line=-1) : PPIError (err,src,line){};
};

//============================================================================================================
// Klasse: SizeError.
//============================================================================================================
class SizeError : public PPIError {
public:
	//--------------------------------------------------------------------------------------------------------
	SizeError(std::string err="SizeError", std::string src=UKWNSRC, long line=-1) : PPIError (err,src,line){};
};

//============================================================================================================
// Klasse: MapError.
//============================================================================================================
class MapError : public PPIError {
public:
	//--------------------------------------------------------------------------------------------------------
	MapError(std::string err="MapError", std::string src=UKWNSRC, long line=-1) : PPIError (err,src,line){};
};
//============================================================================================================
// Klasse: SystemError.
//============================================================================================================
class SystemError : public PPIError {
public:
	//--------------------------------------------------------------------------------------------------------
	SystemError(std::string err="SystemError", std::string src=UKWNSRC, long line=-1) : PPIError (err,src,line){};
};
//============================================================================================================
// Klasse: InvalidBlockSize.
//============================================================================================================
class InvalidBlockSize : public PPIError {
public:
	//--------------------------------------------------------------------------------------------------------
	InvalidBlockSize(std::string err="InvalidBlockSize", std::string src=UKWNSRC, long line=-1) : PPIError (err,src,line){};
};
//============================================================================================================
// Klasse: InvalidSampleRate.
//============================================================================================================
class InvalidSampleRate : public PPIError {
public:
	//--------------------------------------------------------------------------------------------------------
	InvalidSampleRate(std::string err="InvalidSampleRate", std::string src=UKWNSRC, long line=-1) : PPIError (err,src,line){};
};
//============================================================================================================
// Klasse: OwnerLock.
//============================================================================================================
class OwnerLock : public PPIError {
public:
	//--------------------------------------------------------------------------------------------------------
	OwnerLock(std::string err="OwnerLock", std::string src=UKWNSRC, long line=-1) : PPIError (err,src,line){};
};
//============================================================================================================
// Klasse: SettingsException.
//============================================================================================================
class SettingsException : public PPIError {
public:
	//--------------------------------------------------------------------------------------------------------
	SettingsException(std::string err="SettingsException", std::string src=UKWNSRC, long line=-1) : PPIError (err,src,line){};
};
//============================================================================================================
// Klasse: SerializationError.
//============================================================================================================
class SerializationError : public PPIError {
public:
	//--------------------------------------------------------------------------------------------------------
	SerializationError(std::string err="SerializationsError", std::string src=UKWNSRC, long line=-1) : PPIError (err,src,line){};
};
} //namespace ppiError
} //namespace com

#endif


