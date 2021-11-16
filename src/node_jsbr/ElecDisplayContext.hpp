#pragma once

#include <gfx/DisplayContext.hpp>

namespace node_jsbr {

class ElecProgramObject;
class ElecView;

class ElecDisplayContext : public gfx::DisplayContext
{
private:
    typedef gfx::DisplayContext super_t;

    ElecView *m_pView;

    /// Default program object (shader)
    ElecProgramObject *m_pDefPO;

    /// matrix stack
    // TODO: remove?
    std::deque<qlib::Matrix4D> m_matstack;

    qlib::LString m_sectionName;

public:
    ElecDisplayContext() : m_pView(nullptr), m_pDefPO(nullptr) {}
    virtual ~ElecDisplayContext();

    void init(ElecView *pView);

    virtual void drawElem(const gfx::AbstDrawElem &data);

    virtual void startSection(const qlib::LString &section_name);

    virtual void endSection();

    virtual gfx::DisplayContext *createDisplayList();
    virtual bool canCreateDL() const;
    virtual void callDisplayList(DisplayContext *pdl);
    virtual bool isCompatibleDL(DisplayContext *pdl) const;

    //

    virtual bool setCurrent();
    virtual bool isCurrent() const;
    virtual bool isFile() const;

    virtual void vertex(const qlib::Vector4D &);
    virtual void normal(const qlib::Vector4D &);
    virtual void color(const gfx::ColorPtr &c);

    virtual void pushMatrix();
    virtual void popMatrix();
    virtual void multMatrix(const qlib::Matrix4D &mat);
    virtual void loadMatrix(const qlib::Matrix4D &mat);

    virtual void setPolygonMode(int id);
    virtual void startPoints();
    virtual void startPolygon();
    virtual void startLines();
    virtual void startLineStrip();
    virtual void startTriangles();
    virtual void startTriangleStrip();
    virtual void startTriangleFan();
    virtual void startQuadStrip();
    virtual void startQuads();
    virtual void end();

    //

    void clearMatStack()
    {
        m_matstack.erase(m_matstack.begin(), m_matstack.end());
    }

    void xform_vec(Vector4D &v) const
    {
        const Matrix4D &mtop = m_matstack.front();
        v.w() = 1.0;
        mtop.xform4D(v);
    }

    void xform_norm(Vector4D &v) const
    {
        const Matrix4D &mtop = m_matstack.front();
        v.w() = 0.0;
        mtop.xform4D(v);
    }
};

}  // namespace node_jsbr
