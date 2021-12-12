#include <common.h>

#include "EmView.hpp"

#include <emscripten/html5.h>

#include "EmDisplayContext.hpp"
#include "EmProgramObject.hpp"

namespace embr {

EmView::EmView() : m_pCtxt(nullptr) {}

EmView::EmView(const EmView &r) {}

EmView::~EmView() {}

//////////

bool EmView::init()
{
    // glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE);

    glClearDepthf(1.0f);

    // glEnable(GL_NORMALIZE);
    // glShadeModel(GL_SMOOTH);

    // glEnable(GL_LINE_SMOOTH);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    setUpProjMat(-1, -1);
    // setUpLightColor();

    return true;
}

LString EmView::toString() const
{
    return LString("EmView");
}

/// Setup the projection matrix for stereo (View interface)
void EmView::setUpModelMat(int nid)
{
    m_modelMat = qlib::Matrix4D();

    m_modelMat.matprod(
        qlib::Matrix4D::makeTransMat(qlib::Vector4D(0, 0, -getViewDist())));
    m_modelMat.matprod(getRotQuat().toRotMatrix());
    m_modelMat.matprod(qlib::Matrix4D::makeTransMat(-getViewCenter()));

    // LOG_DPRINTLN("modelMat: %s", m_modelMat.toString().c_str());
    auto pdef = m_pCtxt->getProgramObject("default");
    pdef->enable();
    pdef->setMatrix("model", m_modelMat);
}

/// Setup projection matrix (View interface)
void EmView::setUpProjMat(int cx, int cy)
{
    if (cx < 0 || cy < 0) {
        cx = getWidth();
        cy = getHeight();
    }

    float zoom = (float)getZoom(), dist = (float)getViewDist();
    float slabdepth = (float)getSlabDepth();
    if (slabdepth <= 0.1f) slabdepth = 0.1f;

    float slabnear = dist - slabdepth / 2.0f;
    float slabfar = dist + slabdepth;
    // truncate near slab by camera distance
    if (slabnear < 0.1f) slabnear = 0.1f;

    float fognear = dist;
    float fogfar = dist + slabdepth / 2.0f;
    if (fognear < 1.0f) fognear = 1.0;

    // TODO: impl
    // glFogf(GL_FOG_START, (GLfloat)fognear);
    // glFogf(GL_FOG_END, (GLfloat)fogfar);
    // setFogColorImpl();

    // MB_DPRINTLN("Zoom=%f", zoom);
    float vw = zoom / 2.0f;
    float fasp = float(cx) / float(cy);

    printf("ElecView.setUpProjMat> CX=%d, CY=%d, Vw=%f, Fasp=%f\n", cx, cy, vw,
                 fasp);

    int bcx = convToBackingX(cx);
    int bcy = convToBackingY(cy);

    printf("ElecView.setUpProjMat> BCX=%d, BCY=%d\n", bcx, bcy);

    glViewport(0, 0, bcx, bcy);

    {
        const float left = -vw * fasp;
        const float right = vw * fasp;
        const float bottom = -vw;
        const float top = vw;
        const float near = slabnear;
        const float far = slabfar;

        const float a = 2.0f / (right - left);
        const float b = 2.0f / (top - bottom);
        const float c = -2.0f / (far - near);

        const float tx = -(right + left) / (right - left);
        const float ty = -(top + bottom) / (top - bottom);
        const float tz = -(far + near) / (far - near);

        GLfloat m[16] = {
            a, 0, 0, tx,
            0, b, 0, ty,
            0, 0, c, tz,
            0, 0, 0, 1
            // tx, ty, tz, 1
        };

        auto pdef = m_pCtxt->getProgramObject("default");
        pdef->enable();
        auto uloc = pdef->getUniformLocation("projection");
        glUniformMatrix4fv(uloc, 1, GL_FALSE, m);
        // pdef->setMatrix("projection", m_projMat);
        // pdef->setMatrix4fv("projection", 1, GL_FALSE, m);
        
        // m_projMat.aij(1, 1) = a;
        // m_projMat.aij(2, 1) = 0;
        // m_projMat.aij(3, 1) = 0;
        // m_projMat.aij(4, 1) = 0;

        // m_projMat.aij(1, 2) = 0;
        // m_projMat.aij(2, 2) = b;
        // m_projMat.aij(3, 2) = 0;
        // m_projMat.aij(4, 2) = 0;

        // m_projMat.aij(1, 3) = 0;
        // m_projMat.aij(2, 3) = 0;
        // m_projMat.aij(3, 3) = c;
        // m_projMat.aij(4, 3) = 0;

        // m_projMat.aij(1, 4) = tx;
        // m_projMat.aij(2, 4) = ty;
        // m_projMat.aij(3, 4) = tz;
        // m_projMat.aij(4, 4) = 1;
    }

    // LOG_DPRINTLN("projMat: %s", m_projMat.toString().c_str());

    // float *pbuf = &m_projMat.ai(1);
    // constexpr size_t buf_size = 4 * 4;
    // auto peer = m_peerObjRef.Value();
    // auto env = peer.Env();
    // auto method = peer.Get("setUpProjMat").As<Napi::Function>();
    // method.Call(peer, {Napi::Number::New(env, bcx), Napi::Number::New(env, bcy),
    //                    m_projArrayBuf.Value()});

    resetProjChgFlag();
}

/// Draw current scene
void EmView::drawScene()
{
    // if (!m_bInitOK) return;
    // if (!safeSetCurrent()) return;

    qsys::ScenePtr pScene = getScene();
    if (pScene.isnull()) {
        MB_DPRINTLN("DrawScene: invalid scene %d !!", getSceneID());
        return;
    }

    auto *pdc = getDisplayContext();
    pdc->setCurrent();

    gfx::ColorPtr pBgCol = pScene->getBgColor();
    glClearColor(float(pBgCol->fr()), float(pBgCol->fg()), float(pBgCol->fb()), 1.0f);
    // setFogColorImpl();

    // pdc->setLighting(false);

    ////////////////////////////////////////////////

    if (isProjChange()) setUpProjMat(-1, -1);

    setUpModelMat(MM_NORMAL);
    // glDrawBuffer(GL_BACK);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw main 3D objects
    pScene->display(pdc);
}

gfx::DisplayContext *EmView::getDisplayContext()
{
    return m_pCtxt;
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

    if (m_pCtxt == nullptr) {
        m_pCtxt = MB_NEW EmDisplayContext();
        m_pCtxt->init(this);
    }
    m_pCtxt->attach(ctx);
    m_pCtxt->setCurrent();

    init();
    LOG_DPRINTLN("EmView::bind(%s) OK", id.c_str());
}

void registerViewFactory()
{
    qsys::View::setViewFactory(MB_NEW EmViewFactory());
}

}  // namespace embr
