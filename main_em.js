const Module = require("./build.em/src/embr/embr");
const utils = require("./src/embr/utils");

Module['onRuntimeInitialized'] = () => {
    console.log("Module.initCueMol:", Module.initCueMol);
    utils.initCueMol(Module, "/mnt/src/data/sysconfig.xml");

    let pvec = utils.createObject(Module, "Vector");
    console.log("createObj Vec:", pvec);
    utils.setPropObj(Module, pvec, "x", 1.234);
    console.log("getProp Vec.x:", utils.getPropObj(Module, pvec, "x"));
    console.log("getProp Vec.strvalue:", utils.getPropObj(Module, pvec, "strvalue"));

    utils.invokeMethod(Module, pvec, "set3", 1.234, 4.453, 2.341);
    console.log("getProp Vec.toString:", utils.invokeMethod(Module, pvec, "toString"));

    return;
    
    // let args = Module["_allocVarArgs"](3);
    // Module["_setIntVarArgs"](args, 0, 123);
    // console.log("getIntVarArgs:", Module["_getIntVarArgs"](args, 0));
    // Module["_freeVarArgs"](args);


    let mgr = Module.getService("SceneManager");
    console.log("mgr:", mgr);
    console.log("mgr.toString():", mgr.toString());

    let vec = Module.createObj("Vector");
    let vec2 = Module.createObj("Vector");
    // vec.setProp("x", () => {});

    vec.setProp("x", 1);
    vec.setProp("y", 2);
    vec.setProp("z", 3);
    console.log("vec:", vec);
    console.log("vec.toString():", vec.toString());

    console.log("vec.x:", vec.getProp("x"));

    console.log("vec.len():", vec.invokeMethod("length", []));

    vec2.invokeMethod("set3", [3,2,1])
    console.log("vec2.toString():", vec2.toString());
    console.log("vec.dot():", vec.invokeMethod("dot", [vec2]));

};
