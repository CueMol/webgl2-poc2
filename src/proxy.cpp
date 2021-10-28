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
    Napi::Env env = info.Env();

    if (info.Length() != 0) {
        Napi::TypeError::New(env, "Wrong number of arguments")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    // Call: CreateBuffer(bufsize, nelems) -> buf_id
    const auto buffer_size = VERTEX_NUMS * (VERTEX_SIZE + COLOR_SIZE);
    const auto nelems = VERTEX_NUMS;
    buffer_id_ = createBuffer(info, buffer_size, nelems);

    // Call: GetBuffer(buf_id) -> buf_ptr
    float* data = getBuffer(info, buffer_id_);

    // Initialize vertex buffer
    {
        for (int i = 0; i < VERTEX_NUMS / 6; ++i) {
            // for (int i = 0; i < 10; ++i) {
            const int bias = i * 6 * STRIDE_SIZE;
            for (int j = 0; j < 6 * STRIDE_SIZE; ++j) {
                data[bias + j] = vertices_orig[j];
                if (buffer_size <= bias + j) {
                    printf("XXXXXXXXXXXXXXXXXXX buffer overrun\n");
                }
            }
        }
    }

    // printf("Proxy::Create called\n");
    return env.Undefined();
}
Napi::Value Proxy::Render(const Napi::CallbackInfo& info)
{
    // printf("Proxy::Render called\n");

    Napi::Env env = info.Env();

    float* data = getBuffer(info, buffer_id_);
    updateData(data);

    // sendBuffer(bufid) -> void
    sendBuffer(info, buffer_id_);

    return info.Env().Undefined();
}

//////////

int Proxy::createBuffer(const Napi::CallbackInfo& info, int buffer_size, int nelems)
{
    // Call the JS-side display manager's getBuffer() method
    // to get the GL-bound buffer array

    Napi::Env env = info.Env();

    auto method = disp_mgr_.Get("createBuffer").As<Napi::Function>();
    auto rval = method.Call({Napi::Number::New(env, buffer_size),
            Napi::Number::New(env, nelems)});

    int buffer_id = rval.As<Napi::Number>().Int32Value();
    printf("buffer ID: %d\n", buffer_id);

    return buffer_id;
}


float* Proxy::getBuffer(const Napi::CallbackInfo& info, int buffer_id)
{
    // Call the JS-side display manager's getBuffer() method
    // to get the GL-bound buffer array

    Napi::Env env = info.Env();

    auto method = disp_mgr_.Get("getBuffer").As<Napi::Function>();
    auto rval = method.Call({Napi::Number::New(env, buffer_id)});

    auto array_buf = rval.As<Napi::ArrayBuffer>();
    float *pbuf = static_cast<float*>(array_buf.Data());
    size_t len = array_buf.ByteLength();
    printf("buffer ptr: %p, size %lu\n", pbuf, len / sizeof(float));

    return pbuf;
}

void Proxy::sendBuffer(const Napi::CallbackInfo& info, int buffer_id)
{
    Napi::Env env = info.Env();

    auto method = disp_mgr_.Get("sendBuffer").As<Napi::Function>();
    method.Call({Napi::Number::New(env, buffer_id)});
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
