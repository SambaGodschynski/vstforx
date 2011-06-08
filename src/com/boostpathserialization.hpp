#ifndef BOOST_SERIALIZATION_PATH_HPP
#define BOOST_SERIALIZATION_PATH_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

//#define BOOST_FILESYSTEM_VERSION 2

#include <boost/config.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/serialization/level.hpp>

namespace boost { namespace serialization {

template<class Archive, class String, class Traits>
void serialize(Archive& ar, boost::filesystem::basic_path<String, Traits>& p,
                const unsigned int version)
{
     String s;
     if(Archive::is_saving::value)
         s = p.string();
     ar & boost::serialization::make_nvp("string", s);
     if(Archive::is_loading::value)
         p = s;
}

}}

#endif 