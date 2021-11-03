#include "wrapper.hpp"

#include <qlib/LScriptable.hpp>
#include <qlib/LVariant.hpp>
#include <qlib/qlib.hpp>

namespace node_jsbr {

using qlib::LString;

Napi::Value Wrapper::toString(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    auto pobj = getWrapped();
    if (!pobj) {
        Napi::Error::New(env, "Wrapper is not initialized")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    auto msg =
        qlib::LString::format("wrapper (%p) <%s>", pobj, pobj->toString().c_str());
    return Napi::String::New(env, msg.c_str());
}

Napi::Value Wrapper::getAbiClassName(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    auto pScObj = getWrapped();

    qlib::LString str;
    if (pScObj) {
        qlib::LClass *pCls = pScObj->getClassObj();
        if (pCls) {
            str = pCls->getAbiClassName();
        } else {
            str = "(unknown)";
        }

    } else {
        str = "(null)";
    }

    return Napi::String::New(env, str.c_str());
}

Napi::Value Wrapper::getClassName(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    auto pScObj = getWrapped();

    qlib::LString str;
    if (pScObj) {
        qlib::LClass *pCls = pScObj->getClassObj();
        if (pCls) {
            str = pCls->getClassName();
        } else {
            str = "(unknown)";
        }

    } else {
        str = "(null)";
    }

    return Napi::String::New(env, str.c_str());
}

Napi::Value Wrapper::getProp(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() != 1) {
        Napi::TypeError::New(env, "Wrong number of arguments")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString()) {
        Napi::TypeError::New(env, "Wrong type of argument 0")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    auto propname = info[0].As<Napi::String>().Utf8Value();
    auto pScObj = getWrapped();
    if (!pScObj) {
        Napi::Error::New(env, "Wrapped obj is null").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!pScObj->hasNestedProperty(propname)) {
        auto msg =
            LString::format("GetProp: property \"%s\") not found.", propname.c_str());
        Napi::Error::New(env, msg.c_str()).ThrowAsJavaScriptException();
        return env.Null();
    }

    qlib::LVariant lvar;
    if (!pScObj->getNestedProperty(propname, lvar)) {
        LString msg = LString::format("GetProp: getProperty(\"%s\") call failed.",
                                      propname.c_str());
        Napi::Error::New(env, msg.c_str()).ThrowAsJavaScriptException();
        return env.Null();
    }

    return lvarToNapiValue(env, lvar);
}

Napi::Value Wrapper::setProp(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() != 2) {
        Napi::TypeError::New(env, "Wrong number of arguments")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString()) {
        Napi::TypeError::New(env, "Wrong type of argument 0")
            .ThrowAsJavaScriptException();
        return env.Null();
    }
    auto propname = info[0].As<Napi::String>().Utf8Value();

    auto value = info[1];

    auto pScObj = getWrapped();
    if (!pScObj) {
        Napi::Error::New(env, "Wrapped obj is null").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!pScObj->hasNestedProperty(propname)) {
        auto msg =
            LString::format("SetProp: property \"%s\") not found.", propname.c_str());
        Napi::Error::New(env, msg.c_str()).ThrowAsJavaScriptException();
        return env.Null();
    }

    // variant (lvar) doesn't have ownership of its content
    qlib::LVariant lvar;
    if (!Wrapper::napiValueToLVar(env, value, lvar)) {
        auto msg = LString::format(
            "SetProp(%s): Cannot converting NapiValue to LVariant.", propname.c_str());
        Napi::Error::New(env, msg.c_str()).ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!pScObj->setNestedProperty(propname, lvar)) {
        auto msg = LString::format("SetProp(%s) failed.", propname.c_str());
        Napi::Error::New(env, msg.c_str()).ThrowAsJavaScriptException();
        return env.Null();
    }

    return env.Null();
}

/// convert LVariant to Napi::Value
Napi::Value Wrapper::lvarToNapiValue(Napi::Env env, qlib::LVariant &variant)
{
    switch (variant.getTypeID()) {
        case qlib::LVariant::LT_NULL:
            // MB_DPRINTLN("LVariant: null");
            return env.Null();

        case qlib::LVariant::LT_BOOLEAN:
            return Napi::Boolean::New(env, variant.getBoolValue());

        case qlib::LVariant::LT_INTEGER:
            // MB_DPRINTLN("LVar: integer(%d)", variant.getIntValue());
            return Napi::Number::New(env, variant.getIntValue());

        case qlib::LVariant::LT_REAL:
            // MB_DPRINTLN("LVar: real(%f)", variant.getRealValue());
            return Napi::Number::New(env, variant.getRealValue());

        case qlib::LVariant::LT_STRING: {
            // MB_DPRINTLN("LVar: string(%s)", str.c_str());
            return Napi::String::New(env, variant.getStringValue().c_str());
        }
        default:
            break;
    }

    auto msg = LString::format("Unable to convert LVariant(%d) to Napi::Value",
                               variant.getTypeID());
    Napi::Error::New(env, msg.c_str()).ThrowAsJavaScriptException();
    return env.Null();
}

/// convert PyObject to LVariant
bool Wrapper::napiValueToLVar(Napi::Env env, Napi::Value value, qlib::LVariant &rvar)
{
    try {
        // boolean
        if (value.IsBoolean()) {
            rvar.setBoolValue(bool(value.ToBoolean()));
            return true;
        }
        // number
        if (value.IsNumber()) {
            rvar.setRealValue(double(value.ToNumber()));
            return true;
        }
        // string
        if (value.IsString()) {
            rvar.setStringValue(value.ToString().Utf8Value().c_str());
            return true;
        }
    } catch (...) {
        return false;
    }

    return false;
}

Napi::Object Wrapper::Init(Napi::Env env, Napi::Object exports)
{
    Napi::Function func =
        DefineClass(env, "Wrapper",
                    {InstanceMethod<&Wrapper::toString>("toString"),
                     InstanceMethod<&Wrapper::getClassName>("getClassName"),
                     InstanceMethod<&Wrapper::getAbiClassName>("getAbiClassName"),
                     InstanceMethod<&Wrapper::getProp>("getProp"),
                     InstanceMethod<&Wrapper::setProp>("setProp")});

    Napi::FunctionReference *ctor = new Napi::FunctionReference();
    *ctor = Napi::Persistent(func);
    env.SetInstanceData(ctor);

    exports.Set("Wrapper", func);
    return exports;
}

Napi::Object Wrapper::createWrapper(Napi::Env env, qlib::LScriptable *pObj)
{
    auto ctor = env.GetInstanceData<Napi::FunctionReference>();
    auto wrapper = ctor->New({});

    Wrapper *pWrapper = Wrapper::Unwrap(wrapper);
    pWrapper->setWrapped(pObj);

    return wrapper;
}

}  // namespace node_jsbr
