#pragma once

#include <napi.h>

namespace qlib {
class LScriptable;
class LVariant;
}

namespace node_jsbr {

class Wrapper : public Napi::ObjectWrap<Wrapper>
{
private:
    /// wrapped object
    qlib::LScriptable* m_pWrapped;

public:
    using super_t = Napi::ObjectWrap<Wrapper>;

    Wrapper(const Napi::CallbackInfo& info) : super_t(info) {}
    ~Wrapper() = default;

    qlib::LScriptable* getWrapped()
    {
        return m_pWrapped;
    }
    void setWrapped(qlib::LScriptable* pval)
    {
        m_pWrapped = pval;
    }

    Napi::Value toString(const Napi::CallbackInfo& info);
    Napi::Value getClassName(const Napi::CallbackInfo &info);
    Napi::Value getAbiClassName(const Napi::CallbackInfo &info);
    Napi::Value getProp(const Napi::CallbackInfo &info);
    Napi::Value setProp(const Napi::CallbackInfo &info);
    Napi::Value invokeMethod(const Napi::CallbackInfo &info);

    Napi::Value lvarToNapiValue(Napi::Env env, qlib::LVariant &variant);
    bool napiValueToLVar(Napi::Env env, Napi::Value napi_val, qlib::LVariant &rvar);

    static Napi::Object Init(Napi::Env env, Napi::Object exports);

    static Napi::Object createWrapper(Napi::Env env, qlib::LScriptable *pObj);
};

}  // namespace node_jsbr
