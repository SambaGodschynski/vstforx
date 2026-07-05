/*
 * FrxConcreteIO.hpp
 *
 *  Created on: Mon Aug 27 10:35:35 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXCONCRETEIO_H
#define SAMBAG_FRXCONCRETEIO_H

#include <memory>
#include "FrxIO.hpp"
#include <sambag/disco/components/ui/ALookAndFeel.hpp>

namespace frx { namespace gui { namespace components {
//-----------------------------------------------------------------------------
template <class _ProcessorType>
std::string getIOName();

template <class _ProcessorType>
std::string getIOType();
//=============================================================================
/** 
  * @class FrxConcreteIO.
  */
template <class _IOType>
class FrxConcreteIO : public FrxIO, public _IOType {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef FrxIO Super;
	//-------------------------------------------------------------------------
	typedef std::shared_ptr<FrxConcreteIO> Ptr;
	//-------------------------------------------------------------------------
	typedef _IOType IOType;
protected:
	//-------------------------------------------------------------------------
	virtual void postConstructor() {
		IOType::init(getPtr());
        __setTypeId_(getIOType<IOType>());
	}
	//-------------------------------------------------------------------------
	typedef FrxConcreteIO<IOType> ThisClassType;
	//-------------------------------------------------------------------------
	FrxConcreteIO() {
		setName(getIOName<IOType>());
	}
private:
	///////////////////////////////////////////////////////////////////////////
	// Archive:
	//-------------------------------------------------------------------------
	friend class boost::serialization::access;
	//-------------------------------------------------------------------------
	template <typename Archive> 
	void serialize(Archive &ar, const unsigned int version) { 
		ar & boost::serialization::base_object<Super>(*this); 
	} 
public:
	//-------------------------------------------------------------------------
	sdcu::AComponentUIPtr createComponentUI(sdcu::ALookAndFeelPtr laf) const
	{
		return laf->getUI<FrxConcreteIO>();
	}
	//-------------------------------------------------------------------------
	SAMBAG_STD_STATIC_COMPONENT_CREATOR(ThisClassType)
}; // FrxConcreteIO
//=============================================================================
// Types
//=============================================================================
namespace ioTypes {
	struct IOTypeBase{ void init( FrxIO::Ptr ){} };
	struct Input : IOTypeBase{};
	struct Output : IOTypeBase{};
	struct Entry : IOTypeBase{};
	struct Exit : IOTypeBase{};
}
typedef FrxConcreteIO<ioTypes::Input> FrxInputNode;
typedef FrxConcreteIO<ioTypes::Output> FrxOutputNode;
typedef FrxConcreteIO<ioTypes::Entry> FrxEntryNode;
typedef FrxConcreteIO<ioTypes::Exit> FrxExitNode;
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
template <class _ProcessorType>
std::string getIOName() {return "unkonwn iotype";}
template <>
inline std::string getIOName<ioTypes::Input>() {return "FrxInput";}
template <>
inline std::string getIOName<ioTypes::Output>() {return "FrxOutput";}
template <>
inline std::string getIOName<ioTypes::Entry>() {return "FrxEntry";}
template <>
inline std::string getIOName<ioTypes::Exit>() {return "FrxExit";}

template <class _ProcessorType>
std::string getIOType() {return "unkonwn iotype";}
template <>
inline std::string getIOType<ioTypes::Input>() {return "frx.gui.io.Input";}
template <>
inline std::string getIOType<ioTypes::Output>() {return "frx.gui.io.Output";}
template <>
inline std::string getIOType<ioTypes::Entry>() {return "frx.gui.io.Entry";}
template <>
inline std::string getIOType<ioTypes::Exit>() {return "frx.gui.io.Exit";}
}}} // namespace(s)

#endif /* SAMBAG_FRXCONCRETEIO_H */
