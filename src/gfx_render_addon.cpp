#include <napi.h>

#include <stdio.h>
#include <stdlib.h>
#include "proxy.hpp"

Napi::String Method(const Napi::CallbackInfo& info) {
    printf("=== hello called ===\n");
    Napi::Env env = info.Env();
    return Napi::String::New(env, "world");
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "hello"),
                Napi::Function::New(env, Method));
    return gfx_render::Proxy::Init(env, exports);
    // return exports;
}

NODE_API_MODULE(gfx_render, Init)
