//

const convStr = (Module, value) => {
    let arr = Module.intArrayFromString(value);
    let ptr = Module.allocate(arr, Module.ALLOC_NORMAL);
    return ptr;
}

module.exports.initCueMol = (Module, config) => {
    let pstr = convStr(Module, config);
    let pobj = Module["_initCueMol"](pstr);
    Module._free(pstr);
    return pobj
}

module.exports.createObject = (Module, className) => {
    let pstr = convStr(Module, className);
    let pobj = Module["_createObject"](pstr);
    Module._free(pstr);
    return pobj
}

const varToVal = (Module, args, ind) => {
    let type_id = Module["_getVarArgsTypeID"](args, ind);
    console.log(`varToVal ${ind} -> type ID ${type_id}`);
    if (type_id == 0) {
        // LT_NULL
        return null;
    }
    else if (type_id == 1) {
        // LT_BOOLEAN
        return Module["_getBoolVarArgs"](args, ind);
    }
    else if (type_id == 2) {
        // LT_INTEGER
        return Module["_getIntVarArgs"](args, ind);
    }
    else if (type_id == 3) {
        // LT_REAL
        return Module["_getRealVarArgs"](args, ind);
    }
    else if (type_id == 4) {
        // LT_STRING
        let pstr = Module["_getStrVarArgs"](args, ind);
        return Module.UTF8ToString(pstr);
    }
    else if (type_id == 5) {
        // LT_OBJECT
    }
    else if (type_id == 6) {
        // LT_SMARTPTR
    }
    else if (type_id == 7) {
        // LT_ENUM
    }
    else if (type_id == 8) {
        // LT_ARRAY
    }
    else if (type_id == 9) {
        // LT_LIST
    }
    else if (type_id == 10) {
        // LT_DICT
    }

    // error
    throw `unsupported type ID: ${type_id}`;
}

const valToVar = (Module, args, ind, value) => {
    if (value === null || value === undefined) {
        // LT_NULL
    }
    else if (typeof value === "boolean") {
        // LT_BOOLEAN
        Module["_setBoolVarArgs"](args, ind, value);
    }
    else if (typeof value === "number") {
        // LT_REAL / LT_INT
        Module["_setRealVarArgs"](args, ind, value);
    }
    else if (typeof value === "string") {
        // LT_STRING
        let pstr = convStr(Module, value);
        try {
            Module["_setStrVarArgs"](args, ind, pstr);
        }
        finally {
            Module._free(pstr);
        }
    }
    else {
        console.log(`unsupported type: ${typeof value}`);
    }
}

module.exports.getPropObj = (Module, obj, propName) => {
    let args = Module["_allocVarArgs"](0);
    let pPropName = convStr(Module, propName);
    try {
        Module["_getProp"](obj, pPropName, args);
        return varToVal(Module, args, -1);
    }
    finally {
        Module._free(pPropName);
        Module["_freeVarArgs"](args);
    }
}

module.exports.setPropObj = (Module, obj, propName, value) => {
    let args = Module["_allocVarArgs"](1);
    let pPropName = convStr(Module, propName);
    try {
        valToVar(Module, args, 0, value);
        Module["_setProp"](obj, pPropName, args);
    }
    finally {
        Module._free(pPropName);
        Module["_freeVarArgs"](args);
    }
}

module.exports.invokeMethod = (Module, obj, methodName, ...args) => {
    let pMethodName = convStr(Module, methodName);
    let num_args = args.length;
    let varargs = Module["_allocVarArgs"](num_args);
    try {
        args.forEach((elem, ind) => {
            valToVar(Module, varargs, ind, elem);
        });
        Module["_invokeMethod"](obj, pMethodName, varargs);
        return varToVal(Module, varargs, -1);
    }
    finally {
        Module._free(pMethodName);
        Module["_freeVarArgs"](args);
    }
}
