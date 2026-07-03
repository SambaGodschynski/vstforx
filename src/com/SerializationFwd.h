/*
 * SerializationFwd.h
 *
 * Lightweight serialization header for use in .h/.hpp files.
 * Provides forward declarations of archive types and the boost/serialization
 * support headers needed for serialize() method bodies.
 *
 * Only include com/Serialization.h (the heavy version, with boost/archive/*)
 * in .cpp files that actually read or write archives.
 */

#ifndef SERIALIZATION_FWD_H
#define SERIALIZATION_FWD_H

// Forward-declare concrete archive types so that serialize() overloads using
// com::oArchive / com::iArchive can be declared in headers without pulling in
// the full boost/archive/*.hpp headers.
namespace boost { namespace archive {
    class text_oarchive;
    class text_iarchive;
    class binary_oarchive;
    class binary_iarchive;
}}

// Lightweight helpers needed in serialize() template bodies and for
// BOOST_CLASS_EXPORT_KEY / BOOST_CLASS_EXPORT macros.
#include <boost/serialization/access.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/weak_ptr.hpp>
#include <com/boostpathserialization.hpp>

namespace com {
typedef boost::archive::text_oarchive oArchive;
typedef boost::archive::text_iarchive iArchive;

class Serializable {
public:
    virtual ~Serializable() {}
};
} // namespace com

#endif /* SERIALIZATION_FWD_H */
