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

public:
    ElecDisplayContext() : m_pView(nullptr), m_pDefPO(nullptr) {}
    virtual ~ElecDisplayContext();

    void init(ElecView *pView);

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
};

}  // namespace node_jsbr
