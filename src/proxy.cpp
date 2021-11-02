#include "proxy.hpp"

#include <fstream>
#include <cstdio>

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

//////////

Proxy::Proxy(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<Proxy>(info)
{
    vert_buf_.setAttrSize(2);
    vert_buf_.setAttrInfo(0, "vertexPosition", 3, gfx::FLOAT32, offsetof(DrawElem, x));
    vert_buf_.setAttrInfo(1, "color", 4, gfx::FLOAT32, offsetof(DrawElem, r));
}

Napi::Value Proxy::SetManager(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();

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

    printf("Proxy::SetManager OK\n");
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

    // Setup shader
    int shader_id = createShader(info);
    if (shader_id < 0) {
        Napi::TypeError::New(env, "Create shader failed")
            .ThrowAsJavaScriptException();
        return env.Null();
    }
    shader_id_ = shader_id;

    // Call: CreateBuffer(bufsize, nelems) -> buf_id
    const auto buffer_size = VERTEX_NUMS * (VERTEX_SIZE + COLOR_SIZE);
    const auto nelems = VERTEX_NUMS;
    buffer_id_ = createBuffer(info, buffer_size, nelems);
    printf("Proxy::Create create buffer OK (%d)\n", buffer_id_);

    // Call: GetBuffer(buf_id) -> buf_ptr
    float* data = getBuffer(info, buffer_id_);
    if (data==nullptr)
        return env.Null();

    printf("Proxy::Create get buffer OK (%p)\n", data);

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

    printf("Proxy::Create OK\n");
    return env.Undefined();
}
Napi::Value Proxy::Render(const Napi::CallbackInfo& info)
{
    // printf("Proxy::Render called\n");

    Napi::Env env = info.Env();

    float* data = getBuffer(info, buffer_id_);
    if (data==nullptr)
        return env.Null();
        
    updateData(data);

    // sendBuffer(bufid) -> void
    sendBuffer(info, buffer_id_);

    return env.Undefined();
}

//////////

int Proxy::createShader(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();

    std::string vert_shader_fname = "shaders/vertex_shader.glsl";
    std::string vert_src;
    {
        std::ifstream ifs(vert_shader_fname);
        if (!ifs) {
            printf("file: %s not found\n", vert_shader_fname.c_str());
            return -1;
        }
        vert_src = std::string(std::istreambuf_iterator<char>(ifs),
                               std::istreambuf_iterator<char>());
    }
    printf("file: %s OK\n", vert_shader_fname.c_str());
    // printf("vert src:\n%s\n", vert_src.c_str());

    std::string frag_shader_fname = "shaders/fragment_shader.glsl";
    std::string frag_src;
    {
        std::ifstream ifs(frag_shader_fname);
        if (!ifs) {
            printf("file: %s not found\n", frag_shader_fname.c_str());
            return -1;
        }
        frag_src = std::string(std::istreambuf_iterator<char>(ifs),
                               std::istreambuf_iterator<char>());
    }
    printf("file: %s OK\n", frag_shader_fname.c_str());
    // printf("frag src:\n%s\n", frag_src.c_str());

    auto method = disp_mgr_.Get("createShader").As<Napi::Function>();
    auto rval = method.Call(disp_mgr_.Value(),
                            {Napi::String::New(env, vert_src),
                             Napi::String::New(env, frag_src)});

    int buffer_id = rval.As<Napi::Number>().Int32Value();
    printf("shader ID: %d\n", buffer_id);

    return buffer_id;
}

template <typename ... Args>
std::string format(const std::string& fmt, Args ... args )
{
    size_t len = std::snprintf( nullptr, 0, fmt.c_str(), args ... );
    std::vector<char> buf(len + 1);
    std::snprintf(&buf[0], len + 1, fmt.c_str(), args ... );
    return std::string(&buf[0], &buf[0] + len);
}

int Proxy::createBuffer(const Napi::CallbackInfo& info, int buffer_size, int nelems)
{
    std::string json_str;
    json_str += "[";
    for (size_t i=0; i<vert_buf_.getAttrSize(); ++i) {
        if (i>0) json_str += ",";
        json_str += "{";
        json_str += format("\"name\": \"%s\",", vert_buf_.getAttrName(i).c_str());
        json_str += format("\"nelems\": \"%d\",", vert_buf_.getAttrElemSize(i));
        json_str += format("\"itype\": \"%d\",", vert_buf_.getAttrTypeID(i));
        json_str += format("\"npos\": \"%d\"", vert_buf_.getAttrPos(i));
        json_str += "}";
    }
    json_str += "]";
    // printf("json_str:\n%s\n", json_str.c_str());

    // Call the JS-side display manager's getBuffer() method
    // to get the GL-bound buffer array

    Napi::Env env = info.Env();

    auto method = disp_mgr_.Get("createBuffer").As<Napi::Function>();
    auto rval = method.Call(disp_mgr_.Value(),
                            { Napi::Number::New(env, buffer_size),
                              Napi::Number::New(env, nelems),
                              Napi::String::New(env, json_str) });

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
    auto rval = method.Call(disp_mgr_.Value(), {Napi::Number::New(env, buffer_id)});
    if (!rval.IsTypedArray()) {
        Napi::TypeError::New(env, "getBuffer returned unexpected type")
            .ThrowAsJavaScriptException();
        return nullptr;
    }
    
    auto array_buf = rval.As<Napi::Float32Array>();
    float *pbuf = static_cast<float*>(array_buf.Data());
    size_t len = array_buf.ByteLength();
    // printf("buffer ptr: %p, size %lu\n", pbuf, len / sizeof(float));

    return pbuf;
}

void Proxy::sendBuffer(const Napi::CallbackInfo& info, int buffer_id)
{
    Napi::Env env = info.Env();

    auto method = disp_mgr_.Get("sendBuffer").As<Napi::Function>();
    method.Call(disp_mgr_.Value(), {Napi::Number::New(env, buffer_id)});
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
