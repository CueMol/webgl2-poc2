#include <napi.h>

#include <stdio.h>
#include <stdlib.h>
#include "proxy.hpp"
#include "app.hpp"

Napi::String Method(const Napi::CallbackInfo& info) {
    printf("=== hello called ===\n");
    Napi::Env env = info.Env();
    return Napi::String::New(env, "world");
}

Napi::Value initApp(const Napi::CallbackInfo& info) {
    printf("=== initApp called ===\n");
    Napi::Env env = info.Env();

    if (qsys::AppMain::getInstance()) {
        Napi::TypeError::New(env, "App is already initialized")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    qsys::AppMain::init();
    return env.Null();
}

Napi::Value finiApp(const Napi::CallbackInfo& info) {
    printf("=== finiApp called ===\n");
    Napi::Env env = info.Env();

    if (!qsys::AppMain::getInstance()) {
        Napi::TypeError::New(env, "App is not initialized")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    qsys::AppMain::fini();
    return env.Null();
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    // for test
    exports.Set(Napi::String::New(env, "hello"),
                Napi::Function::New(env, Method));

    exports.Set(Napi::String::New(env, "initApp"),
                Napi::Function::New(env, initApp));

    exports.Set(Napi::String::New(env, "finiApp"),
                Napi::Function::New(env, finiApp));

    exports =  gfx_render::Proxy::Init(env, exports);
    exports =  qsys::AppMainWrapper::Init(env, exports);

    return exports;
}

NODE_API_MODULE(gfx_render, Init)
