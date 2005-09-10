//
// $Id$
// 

#include "Image.h"

#include "IDisplayEngine.h"
#include "Player.h"
#include "ISurface.h"

#include "../graphics/Bitmap.h"
#include "../graphics/Filtercolorize.h"
#include "../graphics/Filterfliprgb.h"

#include "../base/Logger.h"
#include "../base/ScopeTimer.h"
#include "../base/XMLHelper.h"

#include <iostream>
#include <sstream>

using namespace std;

namespace avg {

Image::Image ()
    : m_Hue(-1),
      m_Saturation(-1)
{
}

Image::Image (const xmlNodePtr xmlNode, Container * pParent)
    : RasterNode(xmlNode, pParent)
{
    m_href = getRequiredStringAttr (xmlNode, "href");
    m_Hue = getDefaultedIntAttr (xmlNode, "hue", -1);
    m_Saturation = getDefaultedIntAttr (xmlNode, "saturation", -1);
    
}

Image::~Image ()
{
}

void Image::init (IDisplayEngine * pEngine, Container * pParent,
        Player * pPlayer)
{
    Node::init(pEngine, pParent, pPlayer);
    m_Filename = m_href;
    initFilename(pPlayer, m_Filename);
    AVG_TRACE(Logger::PROFILE, "Loading " << m_Filename);

    Bitmap TempBmp(m_Filename);

    PixelFormat pf;
    pf = R8G8B8;
    if (TempBmp.hasAlpha()) {
        pf = R8G8B8A8;
    }
    getSurface()->create(TempBmp.getSize(), pf);
    getSurface()->getBmp()->copyPixels(TempBmp);
/*    
    if (m_Saturation != -1) {
        FilterColorize(m_Hue, m_Saturation).applyInPlace(
                getSurface()->getBmp());
    }
*/
    if (!(pEngine->hasRGBOrdering())) {
        FilterFlipRGB().applyInPlace(getSurface()->getBmp());
    }
}

const std::string& Image::getHRef() const {
    return m_href;
}

static ProfilingZone RenderProfilingZone("  Image::render");

void Image::render (const DRect& Rect)
{
    ScopeTimer Timer(RenderProfilingZone);
    getEngine()->blt32(getSurface(), &getAbsViewport(), getEffectiveOpacity(), 
            getAngle(), getPivot(), getBlendMode());
}

bool Image::obscures (const DRect& Rect, int z) 
{
    return (isActive() && getEffectiveOpacity() > 0.999
            && !getSurface()->getBmp()->hasAlpha() 
            && getZ() > z && getVisibleRect().Contains(Rect));
}

string Image::getTypeStr ()
{
    return "Image";
}

DPoint Image::getPreferredMediaSize()
{
    return DPoint(getSurface()->getBmp()->getSize());
}

}
