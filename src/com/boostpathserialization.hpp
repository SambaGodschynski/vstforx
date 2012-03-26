/*
 * Author: Johan Råde
 * http://boost.2283326.n4.nabble.com/serialization-boost-serialization-path-hpp-td2649784.html
 */
#ifndef BOOST_SERIALIZATION_PATH_HPP
#define BOOST_SERIALIZATION_PATH_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif


#include <boost/config.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/serialization/level.hpp>
#include <string>

namespace boost { namespace serialization {

template<class Archive>
void serialize(Archive& ar, boost::filesystem::path& p,
                const unsigned int version)
{
	std::string s;
     if(Archive::is_saving::value)
         s = p.string();
     ar & boost::serialization::make_nvp("string", s);
     if(Archive::is_loading::value)
         p = s;
}

}}

#endif 
