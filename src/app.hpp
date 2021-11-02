#pragma once

#include <napi.h>

namespace qsys {

class AppMain
{
public:
    AppMain() {}
    ~AppMain() {}

    void createView();
    std::string toString() const { return std::string("AppMain object"); }

    static void init();
    static void fini();
    static AppMain *getInstance() { return s_pInst; }

private:
    static AppMain *s_pInst;
};

/////

class AppMainWrapper : public Napi::ObjectWrap<AppMainWrapper>
{
private:
    AppMain *m_pWrapped;

public:
    using super_t = Napi::ObjectWrap<AppMainWrapper>;

    AppMainWrapper(const Napi::CallbackInfo& info) : super_t(info) {}
    ~AppMainWrapper() = default;

    AppMain *getWrapped() { return m_pWrapped; }
    void setWrapped(AppMain *pval) { m_pWrapped = pval; }

    void createView();

    static void init(const Napi::CallbackInfo& info);
    static void fini(const Napi::CallbackInfo& info);
    static Napi::Value getInstance(const Napi::CallbackInfo& info);
    Napi::Value toString(const Napi::CallbackInfo& info);

    static Napi::Object Init(Napi::Env env, Napi::Object exports);

};


}
