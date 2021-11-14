#include "ElecDisplayList.hpp"

#include <qsys/View.hpp>

namespace node_jsbr {

ElecDisplayList::ElecDisplayList()
    : m_pLineArray(nullptr),
      m_fValid(false),
      m_nDrawMode(DRAWMODE_NONE),
      m_pColor(gfx::SolidColor::createRGB(0.5, 0.5, 0.5)),
      m_fPrevPosValid(false),
      m_prevCol(0)
{
    m_matstack.clear();
    pushMatrix();
    loadIdent();
}

ElecDisplayList::~ElecDisplayList()
{
    if (m_pLineArray) delete m_pLineArray;
}

qlib::uid_t ElecDisplayList::getSceneID() const
{
    printf("target view: %p\n", getTargetView());
    auto result = getTargetView()->getSceneID();
    printf("scene ID: %d\n", result);
    return result;
}

void ElecDisplayList::vertex(const Vector4D &aV)
{
    printf("vert (%f,%f,%f)\n", aV.x(), aV.y(), aV.z());
    Vector4D v(aV);
    xform_vec(v);
    printf("vert (%f,%f,%f)\n", v.x(), v.y(), v.z());

#ifdef MB_DEBUG
    if (!qlib::isFinite(v.x()) || !qlib::isFinite(v.y()) || !qlib::isFinite(v.z())) {
        LOG_DPRINTLN("ERROR: invalid vertex");
    }
#endif

    printf("draw mode %d\n", m_nDrawMode);
    switch (m_nDrawMode) {
        default:
        case DRAWMODE_NONE:
            MB_DPRINTLN("vertex command ignored.");
            break;

        case DRAWMODE_LINES:
            printf("lines prev pos %d\n", m_fPrevPosValid);
            if (!m_fPrevPosValid) {
                m_prevPos = v;
                printf("prev col scene id: %d\n", getSceneID());
                m_prevCol = m_pColor->getDevCode(getSceneID());
                printf("prevcol: %uX\n", m_prevCol);
                m_fPrevPosValid = true;
            } else {
                drawLine(v, m_pColor->getDevCode(getSceneID()), m_prevPos, m_prevCol);
                m_fPrevPosValid = false;
            }
            break;

            //////////////////////////////////////////////////////
        case DRAWMODE_LINESTRIP:
            if (!m_fPrevPosValid) {
                m_prevPos = v;
                m_prevCol = m_pColor->getDevCode(getSceneID());
                m_fPrevPosValid = true;
                break;
            } else {
                drawLine(v, m_pColor->getDevCode(getSceneID()), m_prevPos, m_prevCol);
                m_prevPos = v;
            }
            break;

            //////////////////////////////////////////////////////
        case DRAWMODE_TRIGS:
            // TODO: impl
            // m_pIntData->meshVertex(v, m_norm, m_pColor);
            break;

            //////////////////////////////////////////////////////
        case DRAWMODE_TRIGSTRIP:
            // TODO: impl
            // m_pIntData->meshVertex(v, m_norm, m_pColor, m_nCurAttrib);
            break;

            //////////////////////////////////////////////////////
        case DRAWMODE_TRIGFAN:
            // TODO: impl
            // m_pIntData->meshVertex(v, m_norm, m_pColor);
            break;
    }
}

void ElecDisplayList::normal(const Vector4D &av)
{
    Vector4D v(av);
    xform_norm(v);

#ifdef MB_DEBUG
    if (!qlib::isFinite(v.x()) || !qlib::isFinite(v.y()) || !qlib::isFinite(v.z())) {
        LOG_DPRINTLN("FileDC> ERROR: invalid mesh norm");
    }
#endif

    const double len = v.length();
    if (len < F_EPS4) {
        LOG_DPRINTLN("FileDisp> Normal vector <%f,%f,%f> is too small.", v.x(), v.y(),
                     v.z());
        m_norm = Vector4D(1.0, 0.0, 0.0);
        return;
    }
    m_norm = v.scale(1.0 / len);
}

void ElecDisplayList::color(const gfx::ColorPtr &c)
{
    m_pColor = c;
}

void ElecDisplayList::pushMatrix()
{
    if (m_matstack.size() <= 0) {
        Matrix4D m;
        m_matstack.push_front(m);
        return;
    }
    const Matrix4D &top = m_matstack.front();
    m_matstack.push_front(top);
}
void ElecDisplayList::popMatrix()
{
    if (m_matstack.size() <= 1) {
        LString msg("POVWriter> FATAL ERROR: cannot popMatrix()!!");
        LOG_DPRINTLN(msg);
        MB_THROW(qlib::RuntimeException, msg);
        return;
    }
    m_matstack.pop_front();
}
void ElecDisplayList::multMatrix(const qlib::Matrix4D &mat)
{
    Matrix4D top = m_matstack.front();
    top.matprod(mat);
    m_matstack.front() = top;

    // check unitarity
    // checkUnitary();
}
void ElecDisplayList::loadMatrix(const qlib::Matrix4D &mat)
{
    m_matstack.front() = mat;

    // check unitarity
    // checkUnitary();
}

void ElecDisplayList::setPolygonMode(int id) {}

void ElecDisplayList::startPoints()
{
    m_nDrawMode = DRAWMODE_POINTS;
}

void ElecDisplayList::startPolygon()
{
    LOG_DPRINTLN("polygon is not supported (vertex command ignored.)");
}

void ElecDisplayList::startLines()
{
    if (m_nDrawMode != DRAWMODE_NONE) {
        printf("ElecDisplayList::startLines ERR: %d\n", m_nDrawMode);
        MB_THROW(qlib::RuntimeException, "ElecDisplayList: Unexpected condition");
        return;
    }
    m_nDrawMode = DRAWMODE_LINES;
    printf("ElecDisplayList::startLines OK\n");
}

void ElecDisplayList::startLineStrip()
{
    if (m_nDrawMode != DRAWMODE_NONE) {
        MB_THROW(qlib::RuntimeException, "ElecDisplayList: Unexpected condition");
        return;
    }
    m_nDrawMode = DRAWMODE_LINESTRIP;
}

void ElecDisplayList::startTriangles()
{
    if (m_nDrawMode != DRAWMODE_NONE) {
        MB_THROW(qlib::RuntimeException, "ElecDisplayList: Unexpected condition");
        return;
    }
    m_nDrawMode = DRAWMODE_TRIGS;

    // if (m_nPolyMode == POLY_FILL || m_nPolyMode == POLY_FILL_NORGLN ||
    //     m_nPolyMode == POLY_FILL_XX)
    //     m_pIntData->meshStart(m_nDrawMode);
    // else if (m_nPolyMode == POLY_LINE) {
    //     m_nVertCnt = 0;
    //     m_vectmp.resize(3);
    // }
}

void ElecDisplayList::startTriangleStrip()
{
    if (m_nDrawMode != DRAWMODE_NONE) {
        MB_THROW(qlib::RuntimeException, "ElecDisplayList: Unexpected condition");
        return;
    }
    m_nDrawMode = DRAWMODE_TRIGSTRIP;
    // m_pIntData->meshStart(m_nDrawMode);
}

void ElecDisplayList::startTriangleFan()
{
    m_nDrawMode = DRAWMODE_TRIGFAN;
    // m_pIntData->meshStart(m_nDrawMode);
}

void ElecDisplayList::end()
{
    switch (m_nDrawMode) {
        case DRAWMODE_LINES:
        case DRAWMODE_LINESTRIP:
            m_fPrevPosValid = false;
            break;

        case DRAWMODE_TRIGS:
            // m_pIntData->meshEndTrigs();
            break;

        case DRAWMODE_TRIGFAN:
            // m_pIntData->meshEndFan();
            break;

        case DRAWMODE_TRIGSTRIP:
            // m_pIntData->meshEndTrigStrip();
            break;
    }
    m_nDrawMode = DRAWMODE_NONE;
}

void ElecDisplayList::drawLine(const Vector4D &v1, qlib::quint32 c1, const Vector4D &v2,
                               qlib::quint32 c2)
{
    m_lineBuf.push_back(LineDrawAttr{float(v1.x()), float(v1.y()), float(v1.z()),
                                     float(v1.w()), float(gfx::getFR(c1)),
                                     float(gfx::getFG(c1)), float(gfx::getFB(c1)),
                                     float(gfx::getFA(c1))});

    m_lineBuf.push_back(LineDrawAttr{float(v2.x()), float(v2.y()), float(v2.z()),
                                     float(v2.w()), float(gfx::getFR(c2)),
                                     float(gfx::getFG(c2)), float(gfx::getFB(c2)),
                                     float(gfx::getFA(c2))});
}

bool ElecDisplayList::recordStart()
{
    printf("ElecDisplayList::recordStart called %p\n", m_pLineArray);
    if (m_pLineArray) {
        printf("delete %p\n", m_pLineArray);
        delete m_pLineArray;
        m_pLineArray = nullptr;
    }

    m_fValid = false;

    m_matstack.clear();
    pushMatrix();
    loadIdent();

    return true;
}

void ElecDisplayList::recordEnd()
{
    printf("ElecDisplayList::recordEnd called %p\n", m_pLineArray);
    MB_ASSERT(m_pLineArray == nullptr);

    // Mark as valid
    m_fValid = true;

    // Create attr array
    const size_t nelems = m_lineBuf.size();
    printf("ElecDisplayList::recordEnd nelems %d\n", nelems);
    if (nelems == 0) return;

    m_pLineArray = new LineDrawArray();
    m_pLineArray->setDrawMode(gfx::AbstDrawElem::DRAW_LINES);
    m_pLineArray->setAttrSize(2);
    m_pLineArray->setAttrInfo(0, "vertexPosition", 4, qlib::type_consts::QTC_FLOAT32,
                              offsetof(LineDrawAttr, x));
    m_pLineArray->setAttrInfo(1, "color", 4, qlib::type_consts::QTC_FLOAT32,
                              offsetof(LineDrawAttr, r));
    m_pLineArray->alloc(nelems);

    size_t i = 0;
    for (const auto &elem : m_lineBuf) {
        MB_ASSERT(i < nelems);
        m_pLineArray->at(i) = elem;
        ++i;
    }
    m_pLineArray->setUpdated(true);
}

gfx::DisplayContext *ElecDisplayList::createDisplayList()
{
    return NULL;
}

bool ElecDisplayList::canCreateDL() const
{
    return false;
}

bool ElecDisplayList::isDisplayList() const
{
    return true;
}

}  // namespace node_jsbr
