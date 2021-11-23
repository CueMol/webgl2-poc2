console.log("Initializing...");
// const WebGLRender = require('./webgl_native_render');
// let webgl = new WebGLRender();

const CueMolMgr = require("./cuemol_system");
let mgr = new CueMolMgr();
// mgr.test();

let fpsElem, canvas;

function adjustCanvasSize(placeholder, canvas, devicePixelRatio) {
    let rect = placeholder.getBoundingClientRect();
    let w = rect.width * devicePixelRatio;
    let h = rect.height * devicePixelRatio;
    canvas.style.width = rect.width + "px";
    canvas.style.height = rect.height + "px";
    canvas.width = w;
    canvas.height = h;
    return [w, h];
}

window.addEventListener("load", () => {
    console.log("onLoad() called!!");

    let canvas = document.getElementById('canvas_area');
    let placeholder = document.getElementById('placeholder');
    let devicePixelRatio = window.devicePixelRatio || 1;

    console.log("devicePixelRatio", devicePixelRatio);
    adjustCanvasSize(placeholder, canvas, devicePixelRatio);

    mgr.bindCanvas(canvas);
    const resizeObserver = new ResizeObserver(entries => {
        let [w, h] = adjustCanvasSize(placeholder, canvas, devicePixelRatio);
        rect = placeholder.getBoundingClientRect();
        w = rect.width;
        h = rect.height;
        mgr.resized(w, h);
        mgr.updateDisplay();
    });
    resizeObserver.observe(placeholder);
    
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
