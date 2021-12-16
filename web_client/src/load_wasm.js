// import utils_module from './libs/utils_module.js';
import embr from './embr.js';
const Utils = window.Utils;

console.log("Utils:", Utils);
let promise = embr();

promise.then(
    (m) => {
        let utils = new Utils(m);
        utils.initCueMol("/sysconfig.xml");
        window.onCueMolLoaded(utils);
    }
);
