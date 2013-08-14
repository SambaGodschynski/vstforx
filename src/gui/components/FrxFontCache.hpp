/*
 * FrxFontCache.hpp
 *
 *  Created on: Mon Aug 12 10:29:56 2013
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXFONTCACHE_H
#define SAMBAG_FRXFONTCACHE_H

#include <sambag/disco/FontCache.hpp>
#include <sambag/disco/IDrawContext.hpp>
#include <com/FrxConfig.h>
#include <string>
#include <gui/HandyNamespaces.hpp>


namespace frx { namespace gui { namespace components {

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
template <int UseFontCache>
inline void
drawTextImpl(sambag::disco::IDrawContext::Ptr cn, const std::string &txt)
{
    using namespace sambag::disco;
    Rectangle b = cn->textExtends(txt);
    //cn->translate( Point2D(0, b.height()) );
    Point2D p = cn->getCurrentPoint();
    p.y( p.y() + b.height() );
    cn->moveTo(p);
    cn->textPath(txt);
    cn->fill();
}

template <>
inline void
drawTextImpl<1>(sambag::disco::IDrawContext::Ptr cn, const std::string &txt)
{
    sambag::disco::FontCache &fc = sambag::disco::FontCache::instance();
    fc.drawText(cn ,txt);
}

inline void
drawText(sambag::disco::IDrawContext::Ptr cn, const std::string &txt)
{
    drawTextImpl<FRX_USE_FONTCACHE>(cn, txt);
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
template <int UseFontCache>
inline sd::Rectangle
getTextBoundsImpl(sambag::disco::IDrawContext::Ptr cn,
    const std::string &txt)
{
    return cn->textExtends(txt);
}

template <>
inline sd::Rectangle
getTextBoundsImpl<1>(sambag::disco::IDrawContext::Ptr cn,
    const std::string &txt)
{
    sambag::disco::FontCache &fc = sambag::disco::FontCache::instance();
    return fc.getTextBounds(cn ,txt);
}

inline sd::Rectangle
getTextBounds(sambag::disco::IDrawContext::Ptr cn,
    const std::string &txt)
{
    return getTextBoundsImpl<FRX_USE_FONTCACHE>(cn, txt);
}

}}} // namespace(s)

#endif /* SAMBAG_FRXFONTCACHE_H */
