#pragma once

#include <gfx/DrawAttrArray.hpp>

namespace embr {

class EmVBORep : public gfx::VBORep
{
public:
    qlib::uid_t m_nSceneID;
    GLuint m_nBufID;

    virtual ~EmVBORep()
    {
        qsys::ScenePtr rsc = qsys::SceneManager::getSceneS(m_nSceneID);
        if (rsc.isnull()) {
            MB_DPRINTLN("OglVBO> unknown scene, VBO %d cannot be deleted", m_nBufID);
            return;
        }

        qsys::Scene::ViewIter viter = rsc->beginView();
        if (viter == rsc->endView()) {
            MB_DPRINTLN("OglVBO> no view, VBO %d cannot be deleted", m_nBufID);
            return;
        }

        qsys::ViewPtr rvw = viter->second;
        if (rvw.isnull()) {
            // If any views aren't found, it is no problem,
            // because the parent context (and also all DLs) may be already destructed.
            return;
        }
        gfx::DisplayContext *pctxt = rvw->getDisplayContext();
        pctxt->setCurrent();

        GLuint buffers[1];
        buffers[0] = m_nBufID;
        glDeleteBuffers(1, buffers);
    }
};

int convGLConsts(int id)
{
    switch (id) {
        case qlib::type_consts::QTC_BOOL:
            return GL_BOOL;
        case qlib::type_consts::QTC_UINT8:
            return GL_UNSIGNED_BYTE;
        case qlib::type_consts::QTC_UINT16:
            return GL_UNSIGNED_SHORT;
        case qlib::type_consts::QTC_UINT32:
            return GL_UNSIGNED_INT;
        case qlib::type_consts::QTC_INT8:
            return GL_BYTE;
        case qlib::type_consts::QTC_INT16:
            return GL_SHORT;
        case qlib::type_consts::QTC_INT32:
            return GL_INT;
        case qlib::type_consts::QTC_FLOAT32:
            return GL_FLOAT;
        case qlib::type_consts::QTC_FLOAT64:
            LOG_DPRINTLN("GL_DOUBLE not supported!!");
            // return GL_DOUBLE;
        default:
            return -1;
    }
}

int convGLNorm(int id)
{
    if (id == qlib::type_consts::QTC_FLOAT32 || id == qlib::type_consts::QTC_FLOAT64)
        return GL_FALSE;
    else
        return GL_TRUE;
}

using gfx::DrawElem;

GLenum convDrawMode(int nMode)
{
    GLenum mode;
    switch (nMode) {
        case DrawElem::DRAW_POINTS:
            mode = GL_POINTS;
            break;
        case DrawElem::DRAW_LINE_STRIP:
            mode = GL_LINE_STRIP;
            break;
        case DrawElem::DRAW_LINE_LOOP:
            mode = GL_LINE_LOOP;
            break;
        case DrawElem::DRAW_LINES:
            mode = GL_LINES;
            break;
        case DrawElem::DRAW_TRIANGLE_STRIP:
            mode = GL_TRIANGLE_STRIP;
            break;
        case DrawElem::DRAW_TRIANGLE_FAN:
            mode = GL_TRIANGLE_FAN;
            break;
        case DrawElem::DRAW_TRIANGLES:
            mode = GL_TRIANGLES;
            break;
        // case DrawElem::DRAW_QUAD_STRIP:
        //     mode = GL_QUAD_STRIP;
        //     break;
        // case DrawElem::DRAW_QUADS:
        //     mode = GL_QUADS;
        //     break;
        // case DrawElem::DRAW_POLYGON:
        //     mode = GL_POLYGON;
        //     break;
        default: {
            LString msg = "Ogl DrawElem: invalid draw mode";
            LOG_DPRINTLN(msg);
            MB_THROW(qlib::RuntimeException, msg);
        }
    }
    return mode;
}

}  // namespace embr
