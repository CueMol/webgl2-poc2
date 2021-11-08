#include "ElecProgramObject.hpp"

namespace node_jsbr {

ElecProgramObject::~ElecProgramObject()
{
    // MB_DPRINTLN("OglProgramObj %d destroyed", m_progObjID);
    // glDeleteProgram(m_hPO);
}

bool ElecProgramObject::loadShaders(const qlib::MapTable<qlib::LString> &file_names)
{
    for (const auto &i: file_names) {
        printf("file: %s\n", i.second.c_str());
    }
    return true;
}

}  // namespace node_jsbr
