#pragma once

#include <qlib/MatrixND.hpp>
#include <qsys/MouseEventHandler.hpp>
#include <qsys/Scene.hpp>
#include <qsys/View.hpp>
#include <qsys/qsys.hpp>

#include "embr.hpp"

namespace embr {

class EmDisplayContext;
// using Matrix4F = qlib::MatrixND<4, float>;

class EmView : public qsys::View
{
    MC_SCRIPTABLE;

private:
    EmDisplayContext *m_pCtxt;

    // MouseEventHandler m_meh;

    qlib::Matrix4D m_projMat, m_modelMat;

public:
    EmView();

    EmView(const EmView &r);

    virtual ~EmView();

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

    void bind(const LString &id);

    void onMouseDown(double clientX, double clientY, double screenX, double screenY,
                     int modif);
    void onMouseUp(double clientX, double clientY, double screenX, double screenY,
                   int modif);
    void onMouseMove(double clientX, double clientY, double screenX, double screenY,
                     int modif);

private:
    qsys::MouseEventHandler m_meh;

    static const int DME_MOUSE_DOWN = 0;
    static const int DME_MOUSE_MOVE = 1;
    static const int DME_MOUSE_UP = 2;
    static const int DME_WHEEL = 3;
    static const int DME_DBCHK_TIMEUP = 4;

    void setupInDevEvent(double clientX, double clientY, double screenX, double screenY,
                         int modif, qsys::InDevEvent &ev);
    void dispatchMouseEvent(int nType, qsys::InDevEvent &ev);
};

class EmViewFactory : public qsys::ViewFactory
{
public:
    EmViewFactory() {}
    virtual ~EmViewFactory() {}
    virtual qsys::View *create()
    {
        return MB_NEW EmView();
    }
};

void registerViewFactory();

}  // namespace embr
