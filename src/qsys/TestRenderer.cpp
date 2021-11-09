
#include <common.h>

#include "TestRenderer.hpp"

#include <gfx/DisplayContext.hpp>
#include <gfx/Hittest.hpp>
#include <gfx/SolidColor.hpp>

using namespace qsys;
using gfx::DisplayContext;
using gfx::SolidColor;

TestRenderer::TestRenderer() : Renderer(), m_pDrawData(nullptr)
{
    resetAllProps();
}

TestRenderer::~TestRenderer()
{
    if (m_pDrawData)
        delete m_pDrawData;
    LOG_DPRINTLN("TestRenderer(%p) destructed\n", this);
}

LString TestRenderer::toString() const
{
    return LString::format("TestRenderer(name=%s)", getName().c_str());
}

////////////////////////////////////////////

const char *TestRenderer::getTypeName() const
{
    return "test";
}

void TestRenderer::display(DisplayContext *pdc)
{
    constexpr int VERTEX_NUMS = 1000;

    if (!m_pDrawData) {
        m_pDrawData = new DrawArray();
        m_pDrawData->alloc(VERTEX_NUMS);
        initData();
    }

    updateData();
    pdc->drawElem(*m_pDrawData);
}

// void TestRenderer::displayHit(DisplayContext *pdc)
// {
//     display(pdc);
// }

// LString TestRenderer::interpHit(const gfx::RawHitData &rhit)
// {
//   LString rval;
//   int nface;
//   if (rhit.getData(4, nface)) {
//     rval += LString::format("\"faceid\": %d,\n", nface);
//   }
//   rval += "\"objtype\": \"Test\",\n";
//   return rval;
// }

Vector4D TestRenderer::getCenter() const
{
    return Vector4D(0, 0, 0);
}
