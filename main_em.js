const Module = require("./build.em/src/embr/embr");

// console.log(Module);
// Module['preRun'] = () => {
//     console.log("preRun", Module.FS);
//     console.log("preRun", Module.FS().mkdir);
//     Module.FS.mkdir('/working');
//     FS.mount(NODEFS, { root: '.' }, '/working')
// }

Module['onRuntimeInitialized'] = () => {
    console.log("Module.initCueMol:", Module.initCueMol);
    let obj = new Module.initCueMol("/mnt/Users/user/proj64_cmake/cuemol2/src/xul_gui/sysconfig.xml");
    // console.log("obj:", obj);
    // obj.Initialize();
    // obj.SayHello();
};
