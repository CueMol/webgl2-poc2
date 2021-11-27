const Module = require("./build.em/src/embr/embr");
// import Module from "./build.em/src/embr/embr";

const utils = require("./build.em/javascript/utils");
// const Vector = require("./src/embr/Vector");

Module['onRuntimeInitialized'] = () => {
    console.log("Module.initCueMol:", Module.initCueMol);
    utils.initCueMol("/mnt/src/data/sysconfig.xml");

    let vecobj = utils.createObject("Vector");
    console.log("createObj Vec:", vecobj["@implements_Vector"]);
    // utils.setPropObj(pvec, "x", 1.234);
    // console.log("getProp Vec.x:", utils.getPropObj(pvec, "x"));
    // console.log("getProp Vec.strvalue:", utils.getPropObj(pvec, "strvalue"));
    console.log("Vector:", vecobj);
    console.log("Vector.strvalue:", vecobj.strvalue);
    vecobj.strvalue = "(1,2,3)"
    console.log("Vector.strvalue:", vecobj.strvalue);

    // utils.invokeMethod(pvec, "set3", 1.234, 4.453, 2.341);
    // console.log("getProp Vec.toString:", utils.invokeMethod(pvec, "toString"));

    console.log("==========");
    let vecobj2 = utils.createObject("Vector");
    vecobj2.strvalue = "(3,2,1)"
    console.log("Vec.cross:", vecobj.cross(vecobj2).toString());

    vecobj.destroy();
    vecobj2.destroy();

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
