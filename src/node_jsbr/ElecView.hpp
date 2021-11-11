#pragma once

#include <napi.h>

#include <qsys/Scene.hpp>
#include <qsys/View.hpp>
#include <qsys/qsys.hpp>
#include <qlib/MatrixND.hpp>

namespace node_jsbr {

class ElecDisplayContext;
using Matrix4F = qlib::MatrixND<4, float>;

class ElecView : public qsys::View
{
private:
    ElecDisplayContext *m_pCtxt;

    /// JS-side WebGL display manager
    Napi::ObjectReference m_peerObjRef;

    Matrix4F m_modelMat, m_projMat;

    Napi::ObjectReference m_modelArrayBuf, m_projArrayBuf;

public:
    ElecView();

    ElecView(const ElecView &r);

    virtual ~ElecView();

    //////////

public:
    bool init();

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

    inline Napi::Object getPeerObj() {
        return m_peerObjRef.Value();
    }

private:
    void clear(const gfx::ColorPtr &col);
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
