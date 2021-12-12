#include "EmDisplayContext.hpp"

#include <gfx/DrawAttrArray.hpp>
#include <qsys/SceneManager.hpp>

#include "EmDisplayList.hpp"
#include "EmProgramObject.hpp"
#include "EmView.hpp"

namespace embr {

EmDisplayContext::~EmDisplayContext()
{
    if (m_pDefPO) delete m_pDefPO;
}

void EmDisplayContext::init(EmView *pView)
{
    m_pView = pView;
    setTargetView(pView);

    m_pDefPO = new EmProgramObject(pView, "default");
    qlib::MapTable<qlib::LString> file_names;
    file_names.set("vertex", "shaders/vertex_shader.glsl");
    file_names.set("fragment", "shaders/fragment_shader.glsl");
    m_pDefPO->loadShaders(file_names);
}

void EmDisplayContext::drawElem(const gfx::AbstDrawElem &data)
{
    // auto pda = static_cast<const gfx::AbstDrawAttrs *>(&data);
    // auto pImpl = static_cast<EmVBOImpl *>(data.getVBO());
    // if (!pImpl) {
    //     auto name = LString::format("%s_%p", m_sectionName.c_str(), pda);
    //     pImpl = new EmVBOImpl(m_pView, name, *pda);
    //     data.setVBO(pImpl);
    // }
    // pImpl->drawBuffer(m_pView, data.isUpdated());
    // // printf("EmDisplayContext::drawElem\n");
    // data.setUpdated(false);
}

void EmDisplayContext::startSection(const qlib::LString &section_name)
{
    m_sectionName = section_name;
    if (!m_pDefPO) return;
    m_pDefPO->enable();
    // m_pDefPO->setUniformF("frag_alpha", getAlpha());
}

void EmDisplayContext::endSection()
{
    m_sectionName = "";
    if (!m_pDefPO) return;
    // m_pDefPO->setUniformF("frag_alpha", 1.0);
    m_pDefPO->disable();
}

bool EmDisplayContext::setCurrent()
{
    return true;
}
bool EmDisplayContext::isCurrent() const
{
    return true;
}
bool EmDisplayContext::isFile() const
{
    return false;
}

gfx::DisplayContext *EmDisplayContext::createDisplayList()
{
    EmDisplayList *pdl = MB_NEW EmDisplayList();

    // Targets the same view as this
    pdl->setTargetView(getTargetView());

    printf("createDisplayList OK\n");
    return pdl;
}

bool EmDisplayContext::canCreateDL() const
{
    return true;
}

bool EmDisplayContext::isCompatibleDL(DisplayContext *pdl) const
{
    EmDisplayList *psrc = dynamic_cast<EmDisplayList *>(pdl);
    if (psrc == nullptr) return false;
    return true;
}

void EmDisplayContext::callDisplayList(DisplayContext *pdl)
{
    // printf("callDisplayList called\n");
    EmDisplayList *psrc = dynamic_cast<EmDisplayList *>(pdl);
    if (psrc == nullptr || !psrc->isValid()) return;

    // Lines
    auto *pLines = psrc->getLineArray();
    if (pLines != nullptr) {
        drawElem(*pLines);
    }

    // Triangles
    auto *pTrigs = psrc->getTrigArray();
    if (pTrigs != nullptr) {
        drawElem(*pTrigs);
    }

    // printf("callDisplayList OK\n");
}

//////////

void EmDisplayContext::vertex(const qlib::Vector4D &) {}
void EmDisplayContext::normal(const qlib::Vector4D &) {}
void EmDisplayContext::color(const gfx::ColorPtr &c) {}

void EmDisplayContext::pushMatrix()
{
    if (m_matstack.size() <= 0) {
        Matrix4D m;
        m_matstack.push_front(m);
        return;
    }
    const Matrix4D &top = m_matstack.front();
    m_matstack.push_front(top);
}
void EmDisplayContext::popMatrix()
{
    if (m_matstack.size() <= 1) {
        LString msg("POVWriter> FATAL ERROR: cannot popMatrix()!!");
        LOG_DPRINTLN(msg);
        MB_THROW(qlib::RuntimeException, msg);
        return;
    }
    m_matstack.pop_front();
}
void EmDisplayContext::multMatrix(const qlib::Matrix4D &mat)
{
    Matrix4D top = m_matstack.front();
    top.matprod(mat);
    m_matstack.front() = top;

    // check unitarity
    // checkUnitary();
}
void EmDisplayContext::loadMatrix(const qlib::Matrix4D &mat)
{
    m_matstack.front() = mat;

    // check unitarity
    // checkUnitary();
}

void EmDisplayContext::setPolygonMode(int id) {}
void EmDisplayContext::startPoints() {}
void EmDisplayContext::startPolygon() {}
void EmDisplayContext::startLines() {}
void EmDisplayContext::startLineStrip() {}
void EmDisplayContext::startTriangles() {}
void EmDisplayContext::startTriangleStrip() {}
void EmDisplayContext::startTriangleFan() {}
void EmDisplayContext::startQuadStrip() {}
void EmDisplayContext::startQuads() {}
void EmDisplayContext::end() {}

}  // namespace embr
