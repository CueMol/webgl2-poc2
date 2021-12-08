var Module = {};
Module['locateFile'] = (path, prefix) => {
    console.log("locate file called", path, prefix);
    return prefix + path;
};

Module = require("./build.em/src/embr/embr");
// import Module from "./build.em/src/embr/embr";

Module['locateFile'] = (path, prefix) => {
    console.log("locate file called", path, prefix);
    return prefix + path;
};

const utils = require("./build.em/javascript/utils");
// const Vector = require("./src/embr/Vector");

Module['onRuntimeInitialized'] = () => {
    console.log("Module.initCueMol:", Module.initCueMol);
    // utils.initCueMol("/mnt/src/data/sysconfig.xml");
    utils.initCueMol("/sysconfig.xml");

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
    // vecobj2.strvalue = "(3,2,1)"
    vecobj2.set3(3,2,1);
    console.log("Vec.cross:", vecobj.cross(vecobj2).toString());

    vecobj.destroy();
    vecobj2.destroy();

    console.log("==========");

    let mgr = utils.getService("SceneManager");
    console.log("mgr:", mgr);
    console.log("mgr.toString():", mgr.toString());

    return;
};
