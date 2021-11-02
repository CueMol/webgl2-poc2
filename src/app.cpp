#include "app.hpp"
#include <cassert>

namespace qsys {

AppMain *AppMain::s_pInst;

void AppMain::init()
{
    printf("=== AppMain::init called ===\n");
    assert(!s_pInst);
    s_pInst = new AppMain();
}

void AppMain::fini()
{
    printf("=== AppMain::fini called ===\n");
    assert(s_pInst);
    delete s_pInst;
    s_pInst = nullptr;
}

void AppMain::createView()
{
}

//////////

void AppMainWrapper::init(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    if (AppMain::getInstance()) {
        Napi::TypeError::New(env, "App is already initialized")
            .ThrowAsJavaScriptException();
        return;
    }

    AppMain::init();
}

void AppMainWrapper::fini(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    if (!AppMain::getInstance()) {
        Napi::TypeError::New(env, "App is not initialized")
            .ThrowAsJavaScriptException();
        return;
    }

    AppMain::fini();
}

Napi::Value AppMainWrapper::getInstance(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();

    auto pApp = AppMain::getInstance();
    if (!pApp) {
        Napi::TypeError::New(env, "App is not initialized")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    auto ctor = env.GetInstanceData<Napi::FunctionReference>();
    auto obj = ctor->New({});

    AppMainWrapper *pobj = Unwrap(obj);
    pobj->setWrapped(pApp);

    return obj;
}

Napi::Value AppMainWrapper::toString(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    auto pobj = getWrapped();
    if (!pobj) {
        Napi::TypeError::New(env, "App is not initialized")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    return Napi::String::New(env, pobj->toString());
}

Napi::Object AppMainWrapper::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func =
        DefineClass(env, "AppMain",
                    { InstanceMethod<&AppMainWrapper::toString>("toString"),
                      StaticMethod<&AppMainWrapper::init>("init"),
                      StaticMethod<&AppMainWrapper::fini>("fini"),
                      StaticMethod<&AppMainWrapper::getInstance>("getInstance")});
    
    Napi::FunctionReference* constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.SetInstanceData(constructor);

    exports.Set("AppMain", func);
    return exports;
}

}
