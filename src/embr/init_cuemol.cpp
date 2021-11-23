//
// Emscripten CueMol module initialization
//

#include <common.h>

#include <emscripten/bind.h>
#include <emscripten.h>

#include <gfx/gfx.hpp>
#include <qlib/ClassRegistry.hpp>
#include <qlib/LExceptions.hpp>
#include <qlib/LScriptable.hpp>
#include <qlib/qlib.hpp>
#include <qsys/qsys.hpp>

namespace embr {

using qlib::LString;

/// CueMol initialization routine
bool initCueMol(const std::string &config)
{
	EM_ASM(
           var directory = '/mnt';
           FS.mkdir(directory);
           FS.mount(NODEFS, {root : '/'}, directory);
    );

    printf("initCueMol(%s) called.\n", config.c_str());

    try {
        qlib::init();
        qsys::init(config.c_str());
        // init();
        // registerViewFactory();

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

}  // namespace embr

namespace em = emscripten;

EMSCRIPTEN_BINDINGS(embr) {
    em::function("initCueMol", &embr::initCueMol);
}
