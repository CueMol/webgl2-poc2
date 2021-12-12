#include "EmProgramObject.hpp"

#include <qlib/FileStream.hpp>

#include "EmView.hpp"

namespace embr {

EmProgramObject::~EmProgramObject()
{
    // MB_DPRINTLN("OglProgramObj %d destroyed", m_progObjID);
    // glDeleteProgram(m_hPO);
}

bool EmProgramObject::loadShaders(const qlib::MapTable<qlib::LString> &file_names)
{
    return true;
}

qlib::LString EmProgramObject::loadFile(const qlib::LString &filename)
{
    return "";
}

void EmProgramObject::enable()
{
}

void EmProgramObject::disable()
{
}

// void EmProgramObject::destroy()
// {
// }

}  // namespace embr
