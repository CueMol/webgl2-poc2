let Utils = Object();

const createWrapper = Utils.createWrapper = (native_obj) => {
    if (typeof(native_obj) === 'undefined')
        return null;
    const clsname = native_obj.getClassName();
    const wrapper_name = "./build/javascript/wrappers/" + clsname + ".js";
    const wrapper_module = require(wrapper_name);
    const obj = new wrapper_module.constructor(native_obj, Utils);
    console.log("wrapper created:", typeof(obj));
    return obj;
};

module.exports.createWrapper = createWrapper;
