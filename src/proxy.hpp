#pragma once

#include <napi.h>

#include <random>
#include <vector>
#include "random_generator.hpp"

namespace gfx_render {

class Proxy : public Napi::ObjectWrap<Proxy>
{
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    Proxy(const Napi::CallbackInfo& info);
    
private:
    Napi::Value SetManager(const Napi::CallbackInfo& info);
    Napi::Value Create(const Napi::CallbackInfo& info);
    Napi::Value Render(const Napi::CallbackInfo& info);

    /// Random number generator
    RandomGenerator rng_;
    // Nan::Persistent<v8::Object> mgr_;

    /// JS-side WebGL display manager
    Napi::ObjectReference disp_mgr_;

    static inline float clamp(float min, float max, float val)
    {
        return std::min<float>(std::max<float>(min, val), max);
    }

    inline double randomUniform()
    {
        return rng_.randomUniform();
    }        

    constexpr static int VERTEX_SIZE = 3;  // vec3
    constexpr static int COLOR_SIZE = 4;   // vec4
    constexpr static int STRIDE_SIZE = VERTEX_SIZE + COLOR_SIZE;
    
    constexpr static int VERTEX_NUMS = 6 * 1000; // * 1000;
    constexpr static int TRIANGLE_NUMS = VERTEX_NUMS / 3;
    constexpr static float scl = 1.0f;

    inline void updateData(float *pbuf)
    {
        for (size_t i = 0; i < VERTEX_NUMS; ++i) {
        // for (size_t i = 0; i < VERTEX_NUMS/2; ++i) {
            const int bias = STRIDE_SIZE * i;
            pbuf[bias + 0] += randomUniform() * scl;
            pbuf[bias + 1] += randomUniform() * scl;
            pbuf[bias + 2] += randomUniform() * scl;

            pbuf[bias + 3] = clamp(0, 1, randomUniform() * 0.1 + pbuf[bias + 3]);
            pbuf[bias + 4] = clamp(0, 1, randomUniform() * 0.1 + pbuf[bias + 4]);
            pbuf[bias + 5] = clamp(0, 1, randomUniform() * 0.1 + pbuf[bias + 5]);
        }
    }

    float *getBuffer(const Napi::CallbackInfo& info, int bufid);

};

}
