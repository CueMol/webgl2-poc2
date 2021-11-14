#include "ElecDisplayContext.hpp"

#include <napi.h>

#include <gfx/DrawAttrArray.hpp>
#include <qsys/SceneManager.hpp>

#include "ElecDisplayList.hpp"
#include "ElecProgramObject.hpp"
#include "ElecView.hpp"

namespace node_jsbr {

ElecDisplayContext::~ElecDisplayContext()
{
    if (m_pDefPO) delete m_pDefPO;
}

void ElecDisplayContext::init(ElecView *pView)
{
    m_pView = pView;
    setTargetView(pView);

    m_pDefPO = new ElecProgramObject(pView);
    qlib::MapTable<qlib::LString> file_names;
    file_names.set("vertex", "shaders/vertex_shader.glsl");
    file_names.set("fragment", "shaders/fragment_shader.glsl");
    m_pDefPO->loadShaders(file_names);
}

class ElecVBOImpl : public gfx::VBORep
{
private:
    qlib::uid_t m_nViewID;
    int m_nBufID;
    size_t m_nElems;
    Napi::ObjectReference m_arrayBufRef;
    int m_nDrawMode;

public:
    ElecVBOImpl(ElecView *pView, const gfx::AbstDrawAttrs &data)
        : m_nViewID(pView->getUID()),
          m_nElems(data.getSize()),
          m_nDrawMode(data.getDrawMode())
    {
        qlib::LString json_str;
        json_str += "[";
        for (size_t i = 0; i < data.getAttrSize(); ++i) {
            if (i > 0) json_str += ",";
            json_str += "{";
            json_str +=
                LString::format("\"name\": \"%s\",", data.getAttrName(i).c_str());
            json_str += LString::format("\"nelems\": \"%d\",", data.getAttrElemSize(i));
            json_str += LString::format("\"itype\": \"%d\",", data.getAttrTypeID(i));
            json_str += LString::format("\"npos\": \"%d\"", data.getAttrPos(i));
            // json_str += LString::format("\"size\": \"%d\"", data.getElemSize());
            json_str += "}";
        }
        json_str += "]";
        const size_t buffer_size = data.getDataSize();
        const size_t nelems = data.getSize();

        auto peer = pView->getPeerObj();
        auto env = peer.Env();

        auto method = peer.Get("createBuffer").As<Napi::Function>();
        auto rval = method.Call(
            peer, {Napi::Number::New(env, buffer_size), Napi::Number::New(env, nelems),
                   Napi::String::New(env, json_str)});

        int buffer_id = rval.As<Napi::Number>().Int32Value();
        printf("buffer ID: %d\n", buffer_id);
        m_nBufID = buffer_id;

        auto pbuf = const_cast<void *>(data.getData());

        Napi::Object array_buf = Napi::ArrayBuffer::New(
            env, pbuf, buffer_size, [](Napi::Env, void *finalizeData) {
                printf("finalizer called for %p\n", finalizeData);
                // delete [] static_cast<float*>(finalizeData);
            });
        m_arrayBufRef = Napi::Persistent(array_buf);
    }

    void drawBuffer(ElecView *pView, bool isUpdated)
    {
        auto peer = pView->getPeerObj();
        auto env = peer.Env();

        auto method = peer.Get("drawBuffer").As<Napi::Function>();
        method.Call(peer, {Napi::Number::New(env, m_nBufID),
                           Napi::Number::New(env, m_nDrawMode),
                           Napi::Number::New(env, m_nElems), m_arrayBufRef.Value(),
                           Napi::Boolean::New(env, isUpdated)});
    }

    virtual ~ElecVBOImpl()
    {
        qsys::ViewPtr pView = qsys::SceneManager::getViewS(m_nViewID);
        if (pView.isnull()) {
            // If any views aren't found, it is no problem,
            // because the parent context (and also all DLs) may be already destructed.
            return;
        }
        // auto env = getEnv(pView.get());
        // TODO: impl
    }
};

void ElecDisplayContext::drawElem(const gfx::AbstDrawElem &data)
{
    auto pda = static_cast<const gfx::AbstDrawAttrs *>(&data);
    auto pImpl = static_cast<ElecVBOImpl *>(data.getVBO());
    if (!pImpl) {
        pImpl = new ElecVBOImpl(m_pView, *pda);
        data.setVBO(pImpl);
    }
    pImpl->drawBuffer(m_pView, data.isUpdated());
    // printf("ElecDisplayContext::drawElem\n");
    data.setUpdated(false);
}

void ElecDisplayContext::startSection(const qlib::LString &section_name)
{
    if (!m_pDefPO) return;

    m_pDefPO->enable();
    // m_pDefPO->setUniformF("frag_alpha", getAlpha());
}

void ElecDisplayContext::endSection()
{
    if (!m_pDefPO) return;

    // m_pDefPO->setUniformF("frag_alpha", 1.0);
    m_pDefPO->disable();
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

gfx::DisplayContext *ElecDisplayContext::createDisplayList()
{
    ElecDisplayList *pdl = MB_NEW ElecDisplayList();

    // Targets the same view as this
    pdl->setTargetView(getTargetView());

    printf("createDisplayList OK\n");
    return pdl;
}

bool ElecDisplayContext::canCreateDL() const
{
    return true;
}

bool ElecDisplayContext::isCompatibleDL(DisplayContext *pdl) const
{
    ElecDisplayList *psrc = dynamic_cast<ElecDisplayList *>(pdl);
    if (psrc == nullptr) return false;
    return true;
}

void ElecDisplayContext::callDisplayList(DisplayContext *pdl)
{
    printf("callDisplayList called\n");
    ElecDisplayList *psrc = dynamic_cast<ElecDisplayList *>(pdl);
    if (psrc == nullptr || !psrc->isValid()) return;

    // Lines
    auto *pLines = psrc->getLineArray();
    if (pLines) drawElem(*pLines);

    printf("callDisplayList OK\n");
}

//////////

void ElecDisplayContext::vertex(const qlib::Vector4D &) {}
void ElecDisplayContext::normal(const qlib::Vector4D &) {}
void ElecDisplayContext::color(const gfx::ColorPtr &c) {}

void ElecDisplayContext::pushMatrix()
{
    if (m_matstack.size() <= 0) {
        Matrix4D m;
        m_matstack.push_front(m);
        return;
    }
    const Matrix4D &top = m_matstack.front();
    m_matstack.push_front(top);
}
void ElecDisplayContext::popMatrix()
{
    if (m_matstack.size() <= 1) {
        LString msg("POVWriter> FATAL ERROR: cannot popMatrix()!!");
        LOG_DPRINTLN(msg);
        MB_THROW(qlib::RuntimeException, msg);
        return;
    }
    m_matstack.pop_front();
}
void ElecDisplayContext::multMatrix(const qlib::Matrix4D &mat)
{
    Matrix4D top = m_matstack.front();
    top.matprod(mat);
    m_matstack.front() = top;

    // check unitarity
    // checkUnitary();
}
void ElecDisplayContext::loadMatrix(const qlib::Matrix4D &mat)
{
    m_matstack.front() = mat;

    // check unitarity
    // checkUnitary();
}

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
