#include "ElecDisplayContext.hpp"

namespace node_jsbr {
ElecDisplayContext::~ElecDisplayContext() {}

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
    return true;
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
