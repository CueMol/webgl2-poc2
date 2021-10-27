#include "proxy.hpp"

namespace gfx_render {

Napi::Object Proxy::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func =
        DefineClass(env,
                    "Proxy",
                    { InstanceMethod("setManager", &Proxy::SetManager),
                      InstanceMethod("create", &Proxy::Create),
                      InstanceMethod("render", &Proxy::Render) });
    
    Napi::FunctionReference* constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.SetInstanceData(constructor);

    exports.Set("Proxy", func);
    return exports;
}

Proxy::Proxy(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<Proxy>(info) {
    // Napi::Env env = info.Env();
    // int length = info.Length();
    // if (length <= 0 || !info[0].IsNumber()) {
    //     Napi::TypeError::New(env, "Number expected").ThrowAsJavaScriptException();
    //     return;
    // }
    // Napi::Number value = info[0].As<Napi::Number>();
    // value_ = value.DoubleValue();
}

//////////

Napi::Value Proxy::SetManager(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    // printf("Proxy::SetManager called\n");

    if (info.Length() != 1) {
        Napi::TypeError::New(env, "Wrong number of arguments")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsObject()) {
        Napi::TypeError::New(env, "Wrong type of argument 0")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    disp_mgr_ = Napi::Persistent(info[0].As<Napi::Object>());

    return env.Undefined();
}

constexpr float tri_size = 10.0;
constexpr float vertices_orig[] = {
    -tri_size, tri_size,  0.0, 1.0, 0.0, 0.0, 1.0,
    -tri_size, -tri_size, 0.0, 0.0, 1.0, 0.0, 1.0,
    tri_size,  tri_size,  0.0, 0.0, 0.0, 1.0, 1.0,
    -tri_size, -tri_size, 0.0, 0.0, 1.0, 0.0, 1.0,
    tri_size,  -tri_size, 0.0, 0.0, 0.0, 0.0, 1.0,
    tri_size,  tri_size,  0.0, 0.0, 0.0, 1.0, 1.0,
};

Napi::Value Proxy::Create(const Napi::CallbackInfo& info)
{
    // printf("Proxy::Create called\n");
    return info.Env().Undefined();
}
Napi::Value Proxy::Render(const Napi::CallbackInfo& info)
{
    // printf("Proxy::Render called\n");
    return info.Env().Undefined();
}

//////////

float* Proxy::getBuffer(const Napi::CallbackInfo& info, int buffer_id)
{
    // Call the JS-side display manager's getBuffer() method
    // to get the GL-bound buffer array

    Napi::Env env = info.Env();

    auto method = disp_mgr_.Get("getBuffer").As<Napi::Function>();
    auto rval = method.Call({Napi::Number::New(env, buffer_id)});

    return nullptr;

    // auto xxx = Nan::Get(Nan::New(mgr_), Nan::New("getBuffer").ToLocalChecked())
    //                .ToLocalChecked();
    // auto callback = v8::Local<v8::Function>::Cast(xxx);
    // Nan::Callback cb(callback);
    // const int argc = 1;
    // v8::Local<v8::Value> argv[1] = {Nan::New(bufid)};
    // Nan::AsyncResource resource("demo:get_buffer");
    // auto rval = cb(&resource, Nan::New(mgr_), argc, argv);

    // auto array = v8::Local<v8::ArrayBufferView>::Cast(rval.ToLocalChecked());
    // v8::Local<v8::ArrayBuffer> buffer = array->Buffer();
    // float* data = static_cast<float*>(buffer->GetContents().Data());

    // size_t len = array->ByteLength();
    // printf("buffer ptr: %p, size %d\n", data, len / sizeof(float));

    // return data;
    
}


// Napi::Value Proxy::SetManager(const Napi::CallbackInfo& info) {
//     double num = this->value_;

//     return Napi::Number::New(info.Env(), num);
// }

// Napi::Value Proxy::PlusOne(const Napi::CallbackInfo& info) {
//     this->value_ = this->value_ + 1;

//     return Proxy::GetValue(info);
// }

// Napi::Value Proxy::Multiply(const Napi::CallbackInfo& info) {
//     Napi::Number multiple;
//     if (info.Length() <= 0 || !info[0].IsNumber()) {
//         multiple = Napi::Number::New(info.Env(), 1);
//     } else {
//         multiple = info[0].As<Napi::Number>();
//     }

//     Napi::Object obj = info.Env().GetInstanceData<Napi::FunctionReference>()->New(
//                                                                                   {Napi::Number::New(info.Env(), this->value_ * multiple.DoubleValue())});

//     return obj;
// }

}
