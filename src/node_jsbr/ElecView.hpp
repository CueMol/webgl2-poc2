#pragma once

#include <napi.h>

#include <qsys/Scene.hpp>
#include <qsys/View.hpp>
#include <qsys/qsys.hpp>

namespace node_jsbr {

class ElecDisplayContext;

class ElecView : public qsys::View
{
private:
    ElecDisplayContext *m_pCtxt;

public:
    ElecView();

    ElecView(const ElecView &r);

    virtual ~ElecView();

    //////////

public:
    virtual LString toString() const;

    /// Setup the projection matrix for stereo (View interface)
    virtual void setUpModelMat(int nid);

    /// Setup projection matrix (View interface)
    virtual void setUpProjMat(int w, int h);

    /// Draw current scene
    virtual void drawScene();

    virtual gfx::DisplayContext *getDisplayContext();

    //////////

    void bindPeer(Napi::Object peer);
};

class ElecViewFactory : public qsys::ViewFactory
{
public:
    ElecViewFactory() {}
    virtual ~ElecViewFactory() {}
    virtual qsys::View *create()
    {
        return MB_NEW ElecView();
    }
};

void registerViewFactory();

}  // namespace node_jsbr
