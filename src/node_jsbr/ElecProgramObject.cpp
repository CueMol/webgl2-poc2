#include "ElecProgramObject.hpp"

#include <qlib/FileStream.hpp>

#include "ElecView.hpp"

namespace node_jsbr {

ElecProgramObject::~ElecProgramObject()
{
    // MB_DPRINTLN("OglProgramObj %d destroyed", m_progObjID);
    // glDeleteProgram(m_hPO);
}

bool ElecProgramObject::loadShaders(const qlib::MapTable<qlib::LString> &file_names)
{
    auto peer = m_pView->getPeerObj();
    auto env = peer.Env();
    auto args = Napi::Object::New(env);

    for (const auto &i : file_names) {
        printf("file: %s\n", i.second.c_str());
        qlib::LString src = loadFile(i.second);
        printf("src: %s\n", src.c_str());
        args.Set(i.first.c_str(), src.c_str());
    }

    auto method = peer.Get("createShader").As<Napi::Function>();
    auto rval = method.Call(peer, {args});

    int shader_id = rval.As<Napi::Number>().Int32Value();
    printf("shader ID: %d\n", shader_id);
    m_progObjID = shader_id;

    return true;
}

qlib::LString ElecProgramObject::loadFile(const qlib::LString &filename)
{
    // SysConfig *pconf = SysConfig::getInstance();
    // LString fnam = pconf->convPathName(filename);
    qlib::LString fnam = filename;

    // read source file
    qlib::FileInStream fis;
    fis.open(fnam);
    char sbuf[1024];
    qlib::LString source;
    while (fis.ready()) {
        int n = fis.read(sbuf, 0, sizeof sbuf - 1);
        sbuf[n] = '\0';
        source += sbuf;
    }
    return source;
}

}  // namespace node_jsbr
