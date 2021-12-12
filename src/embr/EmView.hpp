#pragma once

#include <qlib/MatrixND.hpp>
#include <qsys/Scene.hpp>
#include <qsys/View.hpp>
#include <qsys/qsys.hpp>

#include "embr.hpp"

// #include "MouseEventHandler.hpp"

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
