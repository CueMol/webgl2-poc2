import utils_module from "./libs/utils_module.js";
let prom = utils_module("../embr.js");
console.log("Promise:", prom)
prom.then((utils) => {
    utils.initCueMol("/sysconfig.xml");
});

// import x from './embr.js';
// const Module = x();
// console.log("Module:", Module);

// Module.then(
//     (m) => {
//         console.log("Module._em_test_cxx:", m._em_test_cxx);
//         console.log("result:", m._em_test_cxx(123));
//     }
// );
