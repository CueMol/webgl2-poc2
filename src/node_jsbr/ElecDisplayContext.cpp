#include "ElecDisplayContext.hpp"

#include "ElecProgramObject.hpp"

namespace node_jsbr {
ElecDisplayContext::~ElecDisplayContext()
{
    if (m_pDefPO) delete m_pDefPO;
}

void ElecDisplayContext::init(ElecView *pView)
{
    m_pView = pView;

    m_pDefPO = new ElecProgramObject(pView);
    qlib::MapTable<qlib::LString> file_names;
    file_names.set("vertex", "shaders/vertex_shader.glsl");
    file_names.set("fragment", "shaders/fragment_shader.glsl");
    m_pDefPO->loadShaders(file_names);
}

void ElecDisplayContext::drawElem(const gfx::AbstDrawElem &data)
{
    printf("ElecDisplayContext::drawElem\n");
}


bool ElecDisplayContext::setCurrent()
{
    return true;
}
bool ElecDisplayContext::isCurrent() const
{
    return true;
}
bool ElecDisplayContext::isFile() const
{
    return false;
}

void ElecDisplayContext::vertex(const qlib::Vector4D &) {}
void ElecDisplayContext::normal(const qlib::Vector4D &) {}
void ElecDisplayContext::color(const gfx::ColorPtr &c) {}

void ElecDisplayContext::pushMatrix() {}
void ElecDisplayContext::popMatrix() {}
void ElecDisplayContext::multMatrix(const qlib::Matrix4D &mat) {}
void ElecDisplayContext::loadMatrix(const qlib::Matrix4D &mat) {}

void ElecDisplayContext::setPolygonMode(int id) {}
void ElecDisplayContext::startPoints() {}
void ElecDisplayContext::startPolygon() {}
void ElecDisplayContext::startLines() {}
void ElecDisplayContext::startLineStrip() {}
void ElecDisplayContext::startTriangles() {}
void ElecDisplayContext::startTriangleStrip() {}
void ElecDisplayContext::startTriangleFan() {}
void ElecDisplayContext::startQuadStrip() {}
void ElecDisplayContext::startQuads() {}
void ElecDisplayContext::end() {}

}  // namespace node_jsbr
