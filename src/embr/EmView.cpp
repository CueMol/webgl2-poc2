#include <common.h>

#include "EmView.hpp"

#include <emscripten/html5.h>

#ifdef __APPLE__
#include <OpenGLES/ES3/gl.h>
#else
#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#endif


// #include "EmDisplayContext.hpp"

namespace embr {

// EmView::EmView() : m_pCtxt(new EmDisplayContext()) {}
EmView::EmView() : m_pCtxt(nullptr) {}

EmView::EmView(const EmView &r) {}

EmView::~EmView() {}

//////////

bool EmView::init()
{
    setUpProjMat(-1, -1);
    // setUpLightColor();

    // m_pCtxt->init(this);
    return true;
}

LString EmView::toString() const
{
    return LString("EmView");
}

/// Setup the projection matrix for stereo (View interface)
void EmView::setUpModelMat(int nid) {}

/// Setup projection matrix (View interface)
void EmView::setUpProjMat(int cx, int cy) {}

/// Draw current scene
void EmView::drawScene() {}

gfx::DisplayContext *EmView::getDisplayContext()
{
    return nullptr;
    // return m_pCtxt;
}

void EmView::bind(const LString &id)
{
    LOG_DPRINTLN("EmView::bindPeer(%s) called", id.c_str());

    EmscriptenWebGLContextAttributes attr;
    emscripten_webgl_init_context_attributes(&attr);
    attr.alpha = attr.depth = attr.stencil = attr.antialias =
        attr.preserveDrawingBuffer = attr.failIfMajorPerformanceCaveat = false;
    attr.enableExtensionsByDefault = true;
    attr.premultipliedAlpha = false;
    attr.majorVersion = 2;
    attr.minorVersion = 0;
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx =
        emscripten_webgl_create_context(id.c_str(), &attr);
    emscripten_webgl_make_context_current(ctx);

    glClearColor(0, 0, 1, 1);
    // glDrawBuffer(GL_FRONT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    init();
    LOG_DPRINTLN("EmView::bindPeer(%s) OK", id.c_str());
}

void registerViewFactory()
{
    qsys::View::setViewFactory(MB_NEW EmViewFactory());
}

}  // namespace embr
