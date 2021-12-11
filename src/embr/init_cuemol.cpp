//
// Emscripten CueMol module initialization
//

#include <common.h>

#include <emscripten.h>
#include <emscripten/bind.h>

#include <gfx/gfx.hpp>
#include <qlib/ClassRegistry.hpp>
#include <qlib/LExceptions.hpp>
#include <qlib/LScriptable.hpp>
#include <qlib/qlib.hpp>
#include <qsys/qsys.hpp>

#include "EmView.hpp"
#include "embr.hpp"
#include "wrapper.hpp"

namespace embr {

using qlib::LString;

/// CueMol initialization routine
bool initCueMol(const std::string &config)
{
    // EM_ASM(var directory = '/mnt'; FS.mkdir(directory);
    //        FS.mount(NODEFS, {root : './'}, directory););

    printf("initCueMol(%s) called.\n", config.c_str());

    try {
        qlib::init();
        qsys::init(config.c_str());
        init();
        registerViewFactory();

        // setup timer
        // qlib::EventManager::getInstance()->initTimer(new ElecTimerImpl);

    } catch (const qlib::LException &e) {
        printf("XXXXX\n");
        // LOG_DPRINTLN("Init> Caught exception <%s>", typeid(e).name());
        // LOG_DPRINTLN("Init> Reason: %s", e.getMsg().c_str());
        return false;
    } catch (...) {
        printf("XXXXX\n");
        // LOG_DPRINTLN("Init> Caught unknown exception");
        return false;
    }

    // LOG_DPRINTLN("CueMol2 node_js module : INITIALIZED");
    return true;
}

// WrapperPtr getService(const std::string &clsname)
// {
//     printf("getService called\n");
//     qlib::LDynamic *pObj = NULL;
//     try {
//         qlib::ClassRegistry *pMgr = qlib::ClassRegistry::getInstance();
//         MB_ASSERT(pMgr != NULL);
//         pObj = pMgr->getSingletonObj(clsname);
//     } catch (...) {
//         LString msg = LString::format("getService(%s) failed", clsname.c_str());
//         LOG_DPRINTLN("Error: ", msg.c_str());
//         return nullptr;
//     }

//     if (pObj == nullptr) {
//         LString msg =
//             LString::format("getService(%s) returned nullptr", clsname.c_str());
//         LOG_DPRINTLN("Error: ", msg.c_str());
//         return nullptr;
//     }

//     printf("getService result %p\n", pObj);

//     return Wrapper::createWrapper(static_cast<qlib::LScriptable *>(pObj));
// }

// WrapperPtr createObj(const std::string &clsname)
// {
//     printf("createObj called\n");
//     auto pobj = Wrapper::createObj(clsname.c_str());
//     return Wrapper::createWrapper(pobj);
// }

}  // namespace embr

// namespace em = emscripten;

// EMSCRIPTEN_BINDINGS(embr)
// {
//     em::class_<embr::Wrapper>("Wrapper")
//         .smart_ptr_constructor("Wrapper", &std::make_shared<embr::Wrapper>)
//         .function("toString", &embr::Wrapper::toString)
//         .function("getProp", &embr::Wrapper::getProp)
//         .function("setProp", &embr::Wrapper::setProp)
//         .function("invokeMethod", &embr::Wrapper::invokeMethod);
//     em::function("initCueMol", &embr::initCueMol);
//     em::function("getService", &embr::getService);
//     em::function("createObj", &embr::createObj);
// }

// extern "C"
// {
//     EMSCRIPTEN_KEEPALIVE void test_func(const char *i)
//     {
//         printf("test func called %s\n", i);
//     }
// }
