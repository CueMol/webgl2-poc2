#include <common.h>

#include <emscripten.h>

#include <qlib/ClassRegistry.hpp>
#include <qlib/LScriptable.hpp>
#include <qlib/LString.hpp>
#include <qlib/LVarArgs.hpp>
#include <qlib/LVariant.hpp>

#include "wrapper.hpp"

namespace embr {
bool initCueMol(const std::string &config);
}

extern "C"
{
    using EMBR_HANDLE = void *;

    EMSCRIPTEN_KEEPALIVE bool initCueMol(const char *config) {
        return embr::initCueMol(config);
    }

    EMSCRIPTEN_KEEPALIVE EMBR_HANDLE createObject(const char *name)
    {
        auto pobj = embr::Wrapper::createObj(name);
        return static_cast<EMBR_HANDLE>(pobj);
    }

    EMSCRIPTEN_KEEPALIVE void getProp(EMBR_HANDLE pobj, const char *propname,
                                      EMBR_HANDLE presult)
    {
        auto pScObj = static_cast<qlib::LScriptable *>(pobj);
        if (!pScObj) {
            // TODO: throw js excpt
            LOG_DPRINTLN("Wrapped obj is null");
            return;
        }

        if (!pScObj->hasNestedProperty(propname)) {
            auto msg =
                qlib::LString::format("GetProp: property \"%s\") not found.", propname);
            LOG_DPRINTLN(msg);
            return;
        }

        auto pVarArgs = static_cast<qlib::LVarArgs *>(presult);

        auto &lvar = pVarArgs->retval();

        if (!pScObj->getNestedProperty(propname, lvar)) {
            auto msg = qlib::LString::format(
                "GetProp: getProperty(\"%s\") call failed.", propname);
            LOG_DPRINTLN(msg);
            return;
        }

        // printf("getProp(%s) OK\n", propname);
        // if (lvar.isReal()) {
        //     printf("result=%f\n", lvar.getRealValue());
        // }
    }

    EMSCRIPTEN_KEEPALIVE void setProp(EMBR_HANDLE pobj, const char *propname,
                                      EMBR_HANDLE pargs)
    {
        auto pScObj = static_cast<qlib::LScriptable *>(pobj);
        if (!pScObj) {
            // TODO: throw js excpt
            LOG_DPRINTLN("Wrapped obj is null");
            return;
        }

        if (!pScObj->hasNestedProperty(propname)) {
            auto msg =
                qlib::LString::format("SetProp: property \"%s\") not found.", propname);
            LOG_DPRINTLN(msg);
            // TODO: throw js excpt
            return;
        }

        auto pVarArgs = static_cast<qlib::LVarArgs *>(pargs);
        // variant (lvar) doesn't have ownership of its content
        auto &lvar = pVarArgs->at(0);

        if (!pScObj->setNestedProperty(propname, lvar)) {
            auto msg = qlib::LString::format("SetProp(%s) failed.", propname);
            LOG_DPRINTLN(msg);
            // TODO: throw js excpt
            return;
        }
    }

    EMSCRIPTEN_KEEPALIVE void invokeMethod(EMBR_HANDLE pobj, const char *methodname,
                                           EMBR_HANDLE pargs)
    {
        auto pScObj = static_cast<qlib::LScriptable *>(pobj);
        if (!pScObj) {
            LOG_DPRINTLN("Wrapped obj is null");
            // TODO: throw js excpt
            return;
        }

        if (!pScObj->hasMethod(methodname)) {
            auto msg = qlib::LString::format("InvokeMethod: method \"%s\" not found.",
                                             methodname);
            LOG_DPRINTLN(msg);
            // TODO: throw js excpt
            return;
        }

        auto pVarArgs = static_cast<qlib::LVarArgs *>(pargs);

        // Invoke method
        bool ok = false;
        qlib::LString errmsg;

        try {
            ok = pScObj->invokeMethod(methodname, *pVarArgs);
            if (!ok) {
                errmsg = qlib::LString::format("call method %s: failed", methodname);
            }
        } catch (qlib::LException &e) {
            errmsg = qlib::LString::format("Exception occured in native method %s: %s",
                                           methodname, e.getMsg().c_str());
        } catch (std::exception &e) {
            errmsg = qlib::LString::format(
                "Std::exception occured in native method %s: %s", methodname, e.what());
        } catch (...) {
            errmsg = qlib::LString::format(
                "Unknown Exception occured in native method %s", methodname);
        }

        if (!ok) {
            LOG_DPRINTLN(errmsg);
            // TODO: throw js excpt
            return;
        }
    }

    EMSCRIPTEN_KEEPALIVE EMBR_HANDLE allocVarArgs(size_t nargs)
    {
        printf("alloc_varargs called %zu\n", nargs);
        auto result = new qlib::LVarArgs(nargs);
        printf("alloc_varargs result %p\n", result);
        return static_cast<EMBR_HANDLE>(result);
    }

    EMSCRIPTEN_KEEPALIVE void freeVarArgs(EMBR_HANDLE ptr)
    {
        printf("free_varargs called %p\n", ptr);
        auto pVarArgs = static_cast<qlib::LVarArgs *>(ptr);
        delete pVarArgs;
    }

    EMSCRIPTEN_KEEPALIVE int getVarArgsTypeID(EMBR_HANDLE ptr, int ind)
    {
        printf("getVarArgsTypeID called %p [%d] \n", ptr, ind);
        auto pVarArgs = static_cast<qlib::LVarArgs *>(ptr);
        if (ind >= 0) {
            return pVarArgs->at(ind).getTypeID();
        } else {
            return pVarArgs->retval().getTypeID();
        }
    }

    //

    EMSCRIPTEN_KEEPALIVE void setBoolVarArgs(EMBR_HANDLE ptr, int ind, bool value)
    {
        printf("setBoolVarargs called %p [%d] <- %d\n", ptr, ind, value);
        auto pVarArgs = static_cast<qlib::LVarArgs *>(ptr);
        if (ind >= 0) {
            pVarArgs->at(ind).setBoolValue(value);
        } else {
            pVarArgs->retval().setBoolValue(value);
        }
    }

    EMSCRIPTEN_KEEPALIVE bool getBoolVarArgs(EMBR_HANDLE ptr, int ind)
    {
        printf("getBoolVarargs called %p [%d] \n", ptr, ind);
        auto pVarArgs = static_cast<qlib::LVarArgs *>(ptr);
        if (ind >= 0) {
            return pVarArgs->at(ind).getBoolValue();
        } else {
            return pVarArgs->retval().getBoolValue();
        }
    }

    //

    EMSCRIPTEN_KEEPALIVE void setIntVarArgs(EMBR_HANDLE ptr, int ind, int value)
    {
        printf("setIntVarargs called %p [%d] <- %d\n", ptr, ind, value);
        auto pVarArgs = static_cast<qlib::LVarArgs *>(ptr);
        if (ind >= 0) {
            pVarArgs->at(ind).setIntValue(value);
        } else {
            pVarArgs->retval().setIntValue(value);
        }
    }

    EMSCRIPTEN_KEEPALIVE int getIntVarArgs(EMBR_HANDLE ptr, int ind)
    {
        printf("getIntVarargs called %p [%d] \n", ptr, ind);
        auto pVarArgs = static_cast<qlib::LVarArgs *>(ptr);
        if (ind >= 0) {
            return pVarArgs->at(ind).getIntValue();
        } else {
            return pVarArgs->retval().getIntValue();
        }
    }

    //

    EMSCRIPTEN_KEEPALIVE void setRealVarArgs(EMBR_HANDLE ptr, int ind, double value)
    {
        printf("setRealVarargs called %p [%d] <- %f\n", ptr, ind, value);
        auto pVarArgs = static_cast<qlib::LVarArgs *>(ptr);
        if (ind >= 0) {
            pVarArgs->at(ind).setRealValue(value);
        } else {
            pVarArgs->retval().setRealValue(value);
        }
    }

    EMSCRIPTEN_KEEPALIVE int getRealVarArgs(EMBR_HANDLE ptr, int ind)
    {
        printf("getRealVarargs called %p [%d] \n", ptr, ind);
        auto pVarArgs = static_cast<qlib::LVarArgs *>(ptr);
        if (ind >= 0) {
            return pVarArgs->at(ind).getRealValue();
        } else {
            return pVarArgs->retval().getRealValue();
        }
    }

    //

    EMSCRIPTEN_KEEPALIVE void setStrVarArgs(EMBR_HANDLE ptr, int ind, const char *value)
    {
        printf("setStrVarArgs called %p [%d] <- %s\n", ptr, ind, value);
        auto pVarArgs = static_cast<qlib::LVarArgs *>(ptr);
        if (ind >= 0) {
            pVarArgs->at(ind).setStringValue(value);
        } else {
            pVarArgs->retval().setStringValue(value);
        }
    }

    EMSCRIPTEN_KEEPALIVE const char *getStrVarArgs(EMBR_HANDLE ptr, int ind)
    {
        printf("getIntVarargs called %p [%d] \n", ptr, ind);
        auto pVarArgs = static_cast<qlib::LVarArgs *>(ptr);
        if (ind >= 0) {
            return pVarArgs->at(ind).getStringValue().c_str();
        } else {
            return pVarArgs->retval().getStringValue().c_str();
        }
    }
}
