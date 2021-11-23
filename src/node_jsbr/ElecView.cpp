#include <common.h>

#include "ElecView.hpp"

#include "ElecDisplayContext.hpp"

namespace node_jsbr {

ElecView::ElecView() : m_pCtxt(new ElecDisplayContext()) {}

ElecView::ElecView(const ElecView &r) {}

ElecView::~ElecView() {}

//////////

bool ElecView::init()
{
    setUpProjMat(-1, -1);
    // setUpLightColor();

    m_pCtxt->init(this);
}

LString ElecView::toString() const
{
    return LString("ElecView");
}

/// Setup the projection matrix for stereo (View interface)
void ElecView::setUpModelMat(int nid)
{
    qlib::Matrix4D mat;

    mat.matprod(qlib::Matrix4D::makeTransMat(qlib::Vector4D(0, 0, -getViewDist())));
    mat.matprod(getRotQuat().toRotMatrix());
    mat.matprod(qlib::Matrix4D::makeTransMat(-getViewCenter()));

    constexpr size_t buf_size = 4 * 4;
    for (size_t i = 1; i <= buf_size; i++) m_modelMat.ai(i) = float(mat.ai(i));

    auto peer = m_peerObjRef.Value();
    auto env = peer.Env();
    auto method = peer.Get("setUpModelMat").As<Napi::Function>();
    method.Call(peer, {m_modelArrayBuf.Value()});

    // printf("setUpModelMat OK\n");
}

/// Setup projection matrix (View interface)
void ElecView::setUpProjMat(int cx, int cy)
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

    MB_DPRINTLN("ElecView.setUpProjMat> CX=%d, CY=%d, Vw=%f, Fasp=%f", cx, cy, vw,
                fasp);

    int bcx = convToBackingX(cx);
    int bcy = convToBackingY(cy);

    MB_DPRINTLN("ElecView.setUpProjMat> BCX=%d, BCY=%d", bcx, bcy);

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

        m_projMat.aij(1, 1) = a;
        m_projMat.aij(2, 1) = 0;
        m_projMat.aij(3, 1) = 0;
        m_projMat.aij(4, 1) = 0;

        m_projMat.aij(1, 2) = 0;
        m_projMat.aij(2, 2) = b;
        m_projMat.aij(3, 2) = 0;
        m_projMat.aij(4, 2) = 0;

        m_projMat.aij(1, 3) = 0;
        m_projMat.aij(2, 3) = 0;
        m_projMat.aij(3, 3) = c;
        m_projMat.aij(4, 3) = 0;

        m_projMat.aij(1, 4) = tx;
        m_projMat.aij(2, 4) = ty;
        m_projMat.aij(3, 4) = tz;
        m_projMat.aij(4, 4) = 1;
    }

    float *pbuf = &m_projMat.ai(1);

    constexpr size_t buf_size = 4 * 4;
    auto peer = m_peerObjRef.Value();
    auto env = peer.Env();
    auto method = peer.Get("setUpProjMat").As<Napi::Function>();
    method.Call(peer, {Napi::Number::New(env, bcx), Napi::Number::New(env, bcy),
                       m_projArrayBuf.Value()});

    resetProjChgFlag();

    // printf("setUpProjMat OK\n");
}

/// Draw current scene
void ElecView::drawScene()
{
    qsys::ScenePtr pScene = getScene();
    if (pScene.isnull()) {
        MB_DPRINTLN("DrawScene: invalid scene %d !!", getSceneID());
        return;
    }

    gfx::DisplayContext *pdc = getDisplayContext();
    pdc->setCurrent();

    if (isProjChange()) setUpProjMat(-1, -1);
    setUpModelMat(MM_NORMAL);

    gfx::ColorPtr pBgCol = pScene->getBgColor();
    clear(pBgCol);

    try {
        pScene->display(pdc);
    } catch (const std::exception &e) {
        printf("uncaught exception %s\n", e.what());
    } catch (...) {
        printf("uncaught exception!!!\n");
        throw;
    }
}

gfx::DisplayContext *ElecView::getDisplayContext()
{
    return m_pCtxt;
}

void ElecView::bindPeer(Napi::Object peer)
{
    printf("ElecView::bindPeer called\n");
    m_peerObjRef = Napi::Persistent(peer);
    auto env = peer.Env();

    constexpr size_t buf_size = 4 * 4;
    {
        float *pbuf = &m_modelMat.ai(1);
        Napi::Object array_buf = Napi::ArrayBuffer::New(
            env, pbuf, buf_size * sizeof(float), [](Napi::Env, void *finalizeData) {
                printf("finalizer called for %p\n", finalizeData);
                // delete [] static_cast<float*>(finalizeData);
            });
        m_modelArrayBuf = Napi::Persistent(array_buf);
    }
    {
        float *pbuf = &m_projMat.ai(1);
        Napi::Object array_buf = Napi::ArrayBuffer::New(
            env, pbuf, buf_size * sizeof(float), [](Napi::Env, void *finalizeData) {
                printf("finalizer called for %p\n", finalizeData);
                // delete [] static_cast<float*>(finalizeData);
            });
        m_projArrayBuf = Napi::Persistent(array_buf);
    }

    init();
}

void ElecView::clear(const gfx::ColorPtr &col)
{
    auto peer = m_peerObjRef.Value();
    auto env = peer.Env();
    auto method = peer.Get("clear").As<Napi::Function>();
    method.Call(peer,
                {Napi::Number::New(env, col->fr()), Napi::Number::New(env, col->fg()),
                 Napi::Number::New(env, col->fb())});
}

void ElecView::onMouseDown(double clientX, double clientY, double screenX,
                           double screenY, int modif)
{
    // printf("onMouseDown (%f, %f) (%f, %f) %x\n", clientX, clientY, screenX, screenY, modif);
    qsys::InDevEvent ev;
    setupInDevEvent(clientX, clientY, screenX, screenY, modif, ev);
    dispatchMouseEvent(DME_MOUSE_DOWN, ev);
}

void ElecView::onMouseUp(double clientX, double clientY, double screenX, double screenY,
                         int modif)
{
    // printf("onMouseUp (%f, %f) (%f, %f) %x\n", clientX, clientY, screenX, screenY,modif);
    qsys::InDevEvent ev;
    setupInDevEvent(clientX, clientY, screenX, screenY, modif, ev);
    dispatchMouseEvent(DME_MOUSE_UP, ev);
}

void ElecView::onMouseMove(double clientX, double clientY, double screenX,
                           double screenY, int modif)
{
    // printf("onMouseMove (%f, %f) (%f, %f) %x\n", clientX, clientY, screenX, screenY,modif);
    qsys::InDevEvent ev;
    setupInDevEvent(clientX, clientY, screenX, screenY, modif, ev);
    dispatchMouseEvent(DME_MOUSE_MOVE, ev);
}

void ElecView::setupInDevEvent(double clientX, double clientY, double screenX,
                               double screenY, int amodif, qsys::InDevEvent &ev)
{
    ev.setX(int(clientX));
    ev.setY(int(clientY));

    ev.setRootX(int(screenX));
    ev.setRootY(int(screenY));

    int modif = 0;

    if (amodif & 32) modif |= qsys::InDevEvent::INDEV_CTRL;
    if (amodif & 64) modif |= qsys::InDevEvent::INDEV_SHIFT;
    if (amodif & 1) modif |= qsys::InDevEvent::INDEV_LBTN;
    if (amodif & 2) modif |= qsys::InDevEvent::INDEV_RBTN;
    if (amodif & 4) modif |= qsys::InDevEvent::INDEV_MBTN;

    ev.setModifier(modif);
}

void ElecView::dispatchMouseEvent(int nType, qsys::InDevEvent &ev)
{
    switch (nType) {
            // mouse down event
        case DME_MOUSE_DOWN:
            m_meh.buttonDown(ev);
            break;

            // mouse move/dragging event
        case DME_MOUSE_MOVE:
            if (!m_meh.move(ev)) return;  // skip event invokation
            break;

            // mouse up event
        case DME_MOUSE_UP:
            if (!m_meh.buttonUp(ev)) {
                return;  // skip event invokation
            }
            break;

        case DME_WHEEL:
            // wheel events
            break;

            // should not be happen
        default:
            MB_DPRINTLN("unknown nType %d", nType);
            return;
    }

    fireInDevEvent(ev);
}

void registerViewFactory()
{
    qsys::View::setViewFactory(MB_NEW ElecViewFactory());
}

}  // namespace node_jsbr
