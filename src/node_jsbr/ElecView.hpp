#pragma once

#include <napi.h>

#include <qlib/MatrixND.hpp>
#include <qsys/Scene.hpp>
#include <qsys/View.hpp>
#include <qsys/qsys.hpp>

#include "MouseEventHandler.hpp"

namespace node_jsbr {

class ElecDisplayContext;
using Matrix4F = qlib::MatrixND<4, float>;

class ElecView : public qsys::View
{
    MC_SCRIPTABLE;

private:
    ElecDisplayContext *m_pCtxt;

    /// JS-side WebGL display manager
    Napi::ObjectReference m_peerObjRef;

    Matrix4F m_modelMat, m_projMat;

    Napi::ObjectReference m_modelArrayBuf, m_projArrayBuf;

    MouseEventHandler m_meh;

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

    void onMouseDown(double clientX, double clientY, double screenX, double screenY,
                     int modif);
    void onMouseUp(double clientX, double clientY, double screenX, double screenY,
                   int modif);
    void onMouseMove(double clientX, double clientY, double screenX, double screenY,
                     int modif);

    //////////

    void bindPeer(Napi::Object peer);

    inline Napi::Object getPeerObj()
    {
        return m_peerObjRef.Value();
    }

private:
    void clear(const gfx::ColorPtr &col);

    static const int DME_MOUSE_DOWN = 0;
    static const int DME_MOUSE_MOVE = 1;
    static const int DME_MOUSE_UP = 2;
    static const int DME_WHEEL = 3;
    static const int DME_DBCHK_TIMEUP = 4;

    void setupInDevEvent(double clientX, double clientY, double screenX, double screenY,
                         int modif, qsys::InDevEvent &ev);
    void dispatchMouseEvent(int nType, qsys::InDevEvent &ev);
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
