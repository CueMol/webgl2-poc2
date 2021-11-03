//
// Node.js CueMol module initialization
//
#include <common.h>
#include <napi.h>
#include <qlib/qlib.hpp>
#include <qlib/LExceptions.hpp>

namespace node_jsbr {

bool g_bInitOK = false;

// Napi::Value isInitialized(const Napi::CallbackInfo& info)
// {
//     if (g_bInitOK)
//         Napi::Boolean::New(true);
//     else
//         Napi::Boolean::New(false);
// }

/// CueMol initialization routine
Napi::Value initCueMol(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    if (g_bInitOK)
        return env.Null();

    std::string config = "xxx";
    printf("initCueMol(%s) called.\n", config.c_str());

    try {
        qlib::init();
        MB_DPRINTLN("CueMol2 pymodule : INITIALIZED");
    } catch (const qlib::LException &e) {
        LOG_DPRINTLN("Init> Caught exception <%s>", typeid(e).name());
        LOG_DPRINTLN("Init> Reason: %s", e.getMsg().c_str());
    } catch (...) {
        LOG_DPRINTLN("Init> Caught unknown exception");
    }
}

// for test
Napi::String Method(const Napi::CallbackInfo& info) {
    printf("=== hello called ===\n");
    Napi::Env env = info.Env();
    return Napi::String::New(env, "world");
}

}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    // for test
    exports.Set(Napi::String::New(env, "hello"),
                Napi::Function::New(env, node_jsbr::Method));

    // exports.Set(Napi::String::New(env, "initApp"),
    //             Napi::Function::New(env, initApp));

    // exports.Set(Napi::String::New(env, "finiApp"),
    //             Napi::Function::New(env, finiApp));

    // exports =  gfx_render::Proxy::Init(env, exports);
    // exports =  qsys::AppMainWrapper::Init(env, exports);

    return exports;
}

NODE_API_MODULE(cuemol, Init)
