console.log("Initializing...");
const CueMolMgr = require("./cuemol_system");
// const CueMolMgr = window.myAPI.cuemol_system;
const cuemol = require("./wrapper_utils");
// const cuemol = window.myAPI.wrapper_utils;

console.log("CueMolMgr: ", CueMolMgr);

// let mgr = new CueMolMgr();
let mgr = CueMolMgr.create();
console.log("mgr: ", mgr);
// mgr.test();

let fpsElem, canvas;

function adjustCanvasSize(placeholder, canvas, devicePixelRatio) {
    let rect = placeholder.getBoundingClientRect();
    let w = rect.width * devicePixelRatio;
    let h = rect.height * devicePixelRatio;
    // canvas.style.width = rect.width + "px";
    // canvas.style.height = rect.height + "px";
    canvas.width = w;
    canvas.height = h;
    return [w, h];
}

function adjustCanvasSize2(mgr, canvas, dpr) {
    let {width, height} = canvas.getBoundingClientRect();
    console.log(`canvas resize: ${width} x ${height}`);
    canvas.width = width * dpr;
    canvas.height = height * dpr;
    mgr.resized(width, height);
}

window.addEventListener("load", () => {
    console.log("onLoad() called!!");

    let canvas = document.getElementById('canvas_area');
    let placeholder = document.getElementById('placeholder');
    let devicePixelRatio = window.devicePixelRatio || 1;

    console.log("devicePixelRatio", devicePixelRatio);
    // adjustCanvasSize(placeholder, canvas, devicePixelRatio);
    adjustCanvasSize2(mgr, canvas, devicePixelRatio);

    mgr.bindCanvas(canvas);
    const resizeObserver = new ResizeObserver(entries => {
        // let [w, h] = adjustCanvasSize(placeholder, canvas, devicePixelRatio);
        // rect = placeholder.getBoundingClientRect();
        // let {width, height}  = canvas.getBoundingClientRect();
        // mgr.resized(width, height);
        adjustCanvasSize2(mgr, canvas, devicePixelRatio);
        mgr.updateDisplay();
    });
    // resizeObserver.observe(placeholder);
    resizeObserver.observe(canvas);
    
    /////

    elem = document.getElementById("update_button");
    elem.addEventListener("click", (event) => {
	    console.log("Update button click() called!!");
        mgr.rend.update = !mgr.rend.update;
    });

    // webgl.init(canvas);

    fpsElem = document.getElementById('fps');
});

let then = 0;

let timer_update_canvas = (timestamp) => {
    timestamp *= 0.001;
    const deltaTime = timestamp - then;
    then = timestamp;
    const fps = 1 / deltaTime;
    fpsElem.textContent = fps.toFixed(1);

    mgr.updateDisplay();
    window.requestAnimationFrame(timer_update_canvas);
}

window.requestAnimationFrame(timer_update_canvas);

require('electron').ipcRenderer.on('open-file', (event, message) => {
    console.log(message);
    let file_path = message[0];
    let scene = mgr._view.getScene();
    let cmdMgr = cuemol.getService("CmdMgr");

    let load_object = cmdMgr.getCmd("load_object");
    load_object.target_scene = scene;
    load_object.file_path = file_path;
    load_object.run();
    let mol = load_object.result_object;        

    let new_rend = cmdMgr.getCmd("new_renderer");
    new_rend.target_object = mol;
    new_rend.renderer_type = "simple";
    new_rend.renderer_name = "simple1";
    new_rend.recenter_view = true;
    new_rend.default_style_name = "DefaultCPKColoring";
    new_rend.run();
})
