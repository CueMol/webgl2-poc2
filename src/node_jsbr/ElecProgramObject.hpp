#pragma once

#include <qlib/MapTable.hpp>

namespace node_jsbr {

class ElecView;

class ElecProgramObject
{
private:
    ElecView *m_pView;

    // int m_progObjID;
    qlib::LString m_progObjName;

    qlib::LString loadFile(const qlib::LString &filename);

public:
    ElecProgramObject(ElecView *pView, const qlib::LString &name)
        : m_pView(pView), m_progObjName(name)
    {
    }
    virtual ~ElecProgramObject();

    bool loadShaders(const qlib::MapTable<qlib::LString> &name);

    // void clear();

    // void attach(const OglShaderObject *s);

    // bool link();

    // void use();

    // void validate();

    // inline GLuint getHandle() const
    // {
    //     return m_hPO;
    // }

    void enable();

    void disable();

    // inline GLint getUniformLocation(const char *name)
    // {
    //     GLint ul = glGetUniformLocation(m_hPO, name);
    //     if (ul == -1) {
    //         MB_DPRINTLN("Cannot find uniform location: %s", name);
    //     }
    //     return ul;
    // }

    // inline void bindAttribLocation(GLint index, const char *name)
    // {
    //     glBindAttribLocation(m_hPO, index, name);
    // }

    // inline GLint getAttribLocation(const char *name)
    // {
    //     GLint al = glGetAttribLocation(m_hPO, name);
    //     if (al == -1) {
    //         MB_DPRINTLN("Cannot find attribute location: %s", name);
    //     }
    //     return al;
    // }

    // // uniform variable

    // // int

    // inline void setUniform(const char *name, GLint v0)
    // {
    //     glUniform1i(getUniformLocation(name), v0);
    // }

    // inline void setUniform(const char *name, GLint v0, GLint v1)
    // {
    //     glUniform2i(getUniformLocation(name), v0, v1);
    // }

    // inline void setUniform(const char *name, GLint v0, GLint v1, GLint v2)
    // {
    //     glUniform3i(getUniformLocation(name), v0, v1, v2);
    // }

    // inline void setUniform(const char *name, GLint v0, GLint v1, GLint v2, GLint v3)
    // {
    //     glUniform4i(getUniformLocation(name), v0, v1, v2, v3);
    // }

    // // float

    // inline void setUniformF(const char *name, GLfloat v0)
    // {
    //     glUniform1f(getUniformLocation(name), v0);
    // }

    // inline void setUniformF(const char *name, GLfloat v0, GLfloat v1)
    // {
    //     glUniform2f(getUniformLocation(name), v0, v1);
    // }

    // inline void setUniformF(const char *name, GLfloat v0, GLfloat v1, GLfloat v2)
    // {
    //     glUniform3f(getUniformLocation(name), v0, v1, v2);
    // }

    // inline void setUniformF(const char *name, GLfloat v0, GLfloat v1, GLfloat v2,
    //                         GLfloat v3)
    // {
    //     glUniform4f(getUniformLocation(name), v0, v1, v2, v3);
    // }

    // // int array

    // inline void setUniform1iv(const char *name, GLuint count, const GLint *v)
    // {
    //     glUniform1iv(getUniformLocation(name), count, v);
    // }

    // inline void setUniform2iv(const char *name, GLuint count, const GLint *v)
    // {
    //     glUniform2iv(getUniformLocation(name), count, v);
    // }

    // inline void setUniform3iv(const char *name, GLuint count, const GLint *v)
    // {
    //     glUniform3iv(getUniformLocation(name), count, v);
    // }

    // inline void setUniform4iv(const char *name, GLuint count, const GLint *v)
    // {
    //     glUniform4iv(getUniformLocation(name), count, v);
    // }

    // // float array

    // inline void setUniform1fv(const char *name, GLuint count, const GLfloat *v)
    // {
    //     glUniform1fv(getUniformLocation(name), count, v);
    // }

    // inline void setUniform2fv(const char *name, GLuint count, const GLfloat *v)
    // {
    //     glUniform2fv(getUniformLocation(name), count, v);
    // }

    // inline void setUniform3fv(const char *name, GLuint count, const GLfloat *v)
    // {
    //     glUniform3fv(getUniformLocation(name), count, v);
    // }

    // inline void setUniform4fv(const char *name, GLuint count, const GLfloat *v)
    // {
    //     glUniform4fv(getUniformLocation(name), count, v);
    // }

    // // matrix

    // inline void setMatrix2fv(const char *name, GLuint count, GLboolean transpose,
    //                          const GLfloat *v)
    // {
    //     glUniformMatrix2fv(getUniformLocation(name), count, transpose, v);
    // }

    // inline void setMatrix3fv(const char *name, GLuint count, GLboolean transpose,
    //                          const GLfloat *v)
    // {
    //     glUniformMatrix3fv(getUniformLocation(name), count, transpose, v);
    // }

    // inline void setMatrix4fv(const char *name, GLuint count, GLboolean transpose,
    //                          const GLfloat *v)
    // {
    //     glUniformMatrix4fv(getUniformLocation(name), count, transpose, v);
    // }

    // void setMatrix(const char *name, const qlib::Matrix4D &mat);

    // // attribute variable

    // // float

    // inline void setAttrib1f(GLint al, GLfloat v0)
    // {
    //     glVertexAttrib1f(al, v0);
    // }

    // inline void setAttrib2f(GLint al, GLfloat v0, GLfloat v1)
    // {
    //     glVertexAttrib2f(al, v0, v1);
    // }

    // inline void setAttrib3f(GLint al, GLfloat v0, GLfloat v1, GLfloat v2)
    // {
    //     glVertexAttrib3f(al, v0, v1, v2);
    // }

    // inline void setAttrib4f(GLint al, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
    // {
    //     glVertexAttrib4f(al, v0, v1, v2, v3);
    // }

    // // float array

    // inline void setAttrib1fv(GLint al, const GLfloat *v)
    // {
    //     glVertexAttrib1fv(al, v);
    // }

    // inline void setAttrib2fv(GLint al, const GLfloat *v)
    // {
    //     glVertexAttrib2fv(al, v);
    // }

    // inline void setAttrib3fv(GLint al, const GLfloat *v)
    // {
    //     glVertexAttrib3fv(al, v);
    // }

    // inline void setAttrib4fv(GLint al, const GLfloat *v)
    // {
    //     glVertexAttrib4fv(al, v);
    // }

    // void setProgParam(GLenum pname, GLint param);
};
}  // namespace node_jsbr
