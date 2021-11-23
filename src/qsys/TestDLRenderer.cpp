
#include <common.h>

#include "TestDLRenderer.hpp"

#include <gfx/DisplayContext.hpp>
#include <gfx/Hittest.hpp>
#include <gfx/SolidColor.hpp>

namespace qsys {

using gfx::DisplayContext;
using gfx::SolidColor;

TestDLRenderer::TestDLRenderer() : super_t()
{
    resetAllProps();
}

TestDLRenderer::~TestDLRenderer()
{
    LOG_DPRINTLN("TestDLRenderer(%p) destructed\n", this);
}

LString TestDLRenderer::toString() const
{
    return LString::format("TestDLRenderer(name=%s)", getName().c_str());
}

////////////////////////////////////////////

const char *TestDLRenderer::getTypeName() const
{
    return "dltest";
}

void TestDLRenderer::render(DisplayContext *pdl)
{
    printf("TestDLRenderer::render called %p\n", pdl);

    pdl->startTriangles();
    pdl->color(1, 0, 0);
    pdl->vertex(3, 3, 0);

    pdl->color(0, 1, 0);
    pdl->vertex(13, 3, 0);

    pdl->color(0, 0, 1);
    pdl->vertex(13, 13, 10);
    pdl->end();

    pdl->startLines();
    printf("TestDLRenderer::render 1\n");
    pdl->color(1, 0, 0);
    pdl->vertex(0, 0, 0);
    pdl->color(0, 1, 0);
    pdl->vertex(0, 10, 0);
    printf("TestDLRenderer::render 1\n");

    pdl->vertex(0, 10, 0);
    pdl->color(0, 0, 1);
    pdl->vertex(10, 10, 0);
    printf("TestDLRenderer::render 1\n");

    pdl->vertex(10, 10, 0);
    pdl->color(1, 0, 1);
    pdl->vertex(10, 0, 0);
    printf("TestDLRenderer::render 1\n");

    pdl->vertex(10, 0, 0);
    pdl->color(1, 1,  1);
    pdl->vertex(0, 0, 0);
    pdl->end();

    printf("TestDLRenderer::render OK\n");
}


Vector4D TestDLRenderer::getCenter() const
{
    return Vector4D(0, 0, 0);
}
}  // namespace qsys