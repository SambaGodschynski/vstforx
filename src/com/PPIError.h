#ifndef PPIERROR
#define PPIERROR

#include <string>
#include "Events.h"

using namespace std;

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

//============================================================================================================
// Klasse: PPIError.
// Basisklasse fuer alle Fehler.
//============================================================================================================
namespace com{
namespace ppiError{

#define UKWNSRC "unkown source."

class PPIError{
public:
	string error;
	string src;
	long linenumber;
	//--------------------------------------------------------------------------------------------------------
	string src2String() const; 
	//--------------------------------------------------------------------------------------------------------
	string toString() const {
		return "! Exception: " + error + " throwed in: " + src2String();
	}
	//--------------------------------------------------------------------------------------------------------
	PPIError(string err="unkown error.", string source=UKWNSRC, long line=-1);
	//--------------------------------------------------------------------------------------------------------
	void showMessage();
};
//============================================================================================================
// Klasse: StackUnderflow.
//============================================================================================================
class StackUnderflow : public PPIError{
public:
	//--------------------------------------------------------------------------------------------------------
	StackUnderflow(string err="Stack Underflow.", string src=UKWNSRC, long line=-1) : PPIError(err,src,line){};
};
//============================================================================================================
// Klasse: DeadlockException.
//============================================================================================================
class DeadlockException : public PPIError{
public:
	//--------------------------------------------------------------------------------------------------------
	DeadlockException(string err="DeadlockException.", 
		              string src=UKWNSRC, long line=-1) : PPIError(err,src,line){};
};
//============================================================================================================
// Klasse: NullPointer.
//============================================================================================================
class NullPointer : public PPIError{
public:
	//--------------------------------------------------------------------------------------------------------
	NullPointer(string err="NullPointer", string src=UKWNSRC, long line=-1) : PPIError(err,src,line){};
};

//============================================================================================================
// Klasse: GraphIncomplete.
//============================================================================================================
class GraphIncomplete : public PPIError{
public:
	//--------------------------------------------------------------------------------------------------------
	GraphIncomplete(string err="Graph Incomplete", string src=UKWNSRC, long line=-1) : PPIError(err,src,line){};
};
//============================================================================================================
// Klasse: AlgorithmError.
//============================================================================================================
class AlgorithmError : public PPIError{
public:
	//--------------------------------------------------------------------------------------------------------
	AlgorithmError(string err="AlgorithmError", string src=UKWNSRC, long line=-1) : PPIError(err,src,line){};
};
//============================================================================================================
// Klasse: IllegalGraph.
//============================================================================================================
class IllegalGraph : public PPIError{
public:
	//--------------------------------------------------------------------------------------------------------
	IllegalGraph(string err="Illegal Graph", string src=UKWNSRC, long line=-1) : PPIError(err,src,line){};
};
//============================================================================================================
// Klasse: FileIOException.
//============================================================================================================
class FileIOException : public PPIError{
public:
	//--------------------------------------------------------------------------------------------------------
	FileIOException(string err="PushFrameError", string src=UKWNSRC, long line=-1) : PPIError(err,src,line){};
};
//============================================================================================================
// Klasse: PushFrameError.
//============================================================================================================
class PushFrameError : public PPIError{
public:
	//--------------------------------------------------------------------------------------------------------
	PushFrameError(string err="PushFrameError", string src=UKWNSRC, long line=-1) : PPIError(err,src,line){};
};

//============================================================================================================
// Klasse: StageBufferException.
//============================================================================================================
class StageBufferException : public PPIError{
public:
	//--------------------------------------------------------------------------------------------------------
	StageBufferException(string err="StageBufferException", string src=UKWNSRC, long line=-1) : PPIError(err,src,line){};
};

//============================================================================================================
// Klasse: IteratorError.
//============================================================================================================
class IteratorError : public StageBufferException {
public:
	//--------------------------------------------------------------------------------------------------------
	IteratorError(string err="StageBufferException: Iterator Error", string src=UKWNSRC, long line=-1) : StageBufferException(err,src,line){};
};

//============================================================================================================
// Klasse: IteratorError.
//============================================================================================================
class SettingsNotInitialized : public PPIError {
public:
	//--------------------------------------------------------------------------------------------------------
	SettingsNotInitialized(string err="SettingsNotInitialized", string src=UKWNSRC, long line=-1) : PPIError(err,src,line){};
};

//============================================================================================================
// Klasse: IllegalOperation.
//============================================================================================================
class IllegalOperation : public PPIError {
public:
	//--------------------------------------------------------------------------------------------------------
	IllegalOperation(string err="IllegalOperation", string src=UKWNSRC, long line=-1) : PPIError(err,src,line){};
};

//============================================================================================================
// Klasse: IllegalOperation.
//============================================================================================================
class TypeError : public PPIError {
public:
	//--------------------------------------------------------------------------------------------------------
	TypeError(string err="TypeError", string src=UKWNSRC, long line=-1) : PPIError(err,src,line){};
};

//============================================================================================================
// Klasse: IndexOutOfBoundException.
//============================================================================================================
class IndexOutOfBoundException : public PPIError {
public:
	//--------------------------------------------------------------------------------------------------------
	IndexOutOfBoundException(string err="IndexOutOfBoundException", string src=UKWNSRC, long line=-1) : PPIError(err,src,line){};
};

//============================================================================================================
// Klasse: DllError.
//============================================================================================================
class DllError : public PPIError {
public:
	//--------------------------------------------------------------------------------------------------------
	DllError(string err="DllError", string src=UKWNSRC, long line=-1) : PPIError (err,src,line){};
};

//============================================================================================================
// Klasse: DataError.
//============================================================================================================
class DataError : public PPIError {
public:
	//--------------------------------------------------------------------------------------------------------
	DataError(string err="DataError", string src=UKWNSRC, long line=-1) : PPIError (err,src,line){};
};

//============================================================================================================
// Klasse: SizeError.
//============================================================================================================
class SizeError : public PPIError {
public:
	//--------------------------------------------------------------------------------------------------------
	SizeError(string err="SizeError", string src=UKWNSRC, long line=-1) : PPIError (err,src,line){};
};

//============================================================================================================
// Klasse: MapError.
//============================================================================================================
class MapError : public PPIError {
public:
	//--------------------------------------------------------------------------------------------------------
	MapError(string err="MapError", string src=UKWNSRC, long line=-1) : PPIError (err,src,line){};
};
//============================================================================================================
// Klasse: SystemError.
//============================================================================================================
class SystemError : public PPIError {
public:
	//--------------------------------------------------------------------------------------------------------
	SystemError(string err="SystemError", string src=UKWNSRC, long line=-1) : PPIError (err,src,line){};
};
//============================================================================================================
// Klasse: InvalidBlockSize.
//============================================================================================================
class InvalidBlockSize : public PPIError {
public:
	//--------------------------------------------------------------------------------------------------------
	InvalidBlockSize(string err="InvalidBlockSize", string src=UKWNSRC, long line=-1) : PPIError (err,src,line){};
};
//============================================================================================================
// Klasse: InvalidSampleRate.
//============================================================================================================
class InvalidSampleRate : public PPIError {
public:
	//--------------------------------------------------------------------------------------------------------
	InvalidSampleRate(string err="InvalidSampleRate", string src=UKWNSRC, long line=-1) : PPIError (err,src,line){};
};
//============================================================================================================
// Klasse: OwnerLock.
//============================================================================================================
class OwnerLock : public PPIError {
public:
	//--------------------------------------------------------------------------------------------------------
	OwnerLock(string err="OwnerLock", string src=UKWNSRC, long line=-1) : PPIError (err,src,line){};
};
//============================================================================================================
// Klasse: SettingsException.
//============================================================================================================
class SettingsException : public PPIError {
public:
	//--------------------------------------------------------------------------------------------------------
	SettingsException(string err="SettingsException", string src=UKWNSRC, long line=-1) : PPIError (err,src,line){};
};
//============================================================================================================
// Klasse: SerializationError.
//============================================================================================================
class SerializationError : public PPIError {
public:
	//--------------------------------------------------------------------------------------------------------
	SerializationError(string err="SerializationsError", string src=UKWNSRC, long line=-1) : PPIError (err,src,line){};
};


} //namespace ppiError
} //namespace com

#endif


