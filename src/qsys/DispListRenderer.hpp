// -*-Mode: C++;-*-
//
// Adoptor class for renderers with display list cache support
//
// $Id: DispListRenderer.hpp,v 1.7 2011/04/17 06:16:17 rishitani Exp $
//

#ifndef QSYS_DISPLIST_RENDERER_HPP_INCLUDE_
#define QSYS_DISPLIST_RENDERER_HPP_INCLUDE_

#include "DispCacheRenderer.hpp"
#include "qsys.hpp"

namespace gfx {
class DisplayContext;
}

namespace qsys {

using gfx::DisplayContext;

///
///  Adoptor class for renderers with display list cache support
///
class QSYS_API DispListRenderer : public DispCacheRenderer
{
private:
    typedef DispCacheRenderer super_t;

    DispListCacheImpl m_dlcache;

public:
    DispListRenderer();
    DispListRenderer(const DispListRenderer &r);
    virtual ~DispListRenderer();

    //////////////////////////////////////////////////////
    // Renderer implementation

    virtual void display(DisplayContext *pdc);

    virtual void invalidateDisplayCache();

    //
    // Hittest implementation
    //

    /// render Hittest object
    virtual void displayHit(DisplayContext *pdc);

    virtual void invalidateHittestCache();
};
}  // namespace qsys

#endif
