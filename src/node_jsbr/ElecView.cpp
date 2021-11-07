#include <common.h>

#include "ElecView.hpp"

#include "ElecDisplayContext.hpp"

namespace node_jsbr {

ElecView::ElecView() : m_pCtxt(new ElecDisplayContext()) {}

ElecView::ElecView(const ElecView &r) {}

ElecView::~ElecView() {}

//////////

LString ElecView::toString() const
{
    return LString("ElecView");
}

/// Setup the projection matrix for stereo (View interface)
void ElecView::setUpModelMat(int nid) {}

/// Setup projection matrix (View interface)
void ElecView::setUpProjMat(int w, int h) {}

/// Draw current scene
void ElecView::drawScene()
{
    printf("ElecView::drawScene called!!\n");
    qsys::ScenePtr pScene = getScene();
    if (pScene.isnull()) {
        MB_DPRINTLN("DrawScene: invalid scene %d !!", getSceneID());
        return;
    }

    gfx::DisplayContext *pdc = getDisplayContext();
    pdc->setCurrent();
    pScene->display(pdc);
}

gfx::DisplayContext *ElecView::getDisplayContext()
{
    return m_pCtxt;
}

void ElecView::bindPeer(Napi::Object peer)
{
    printf("ElecView::bindPeer called\n");
}

void registerViewFactory()
{
    qsys::View::setViewFactory(MB_NEW ElecViewFactory());
}

}  // namespace node_jsbr
