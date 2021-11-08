console.log("Initializing...");
// const WebGLRender = require('./webgl_native_render');
// let webgl = new WebGLRender();

const CueMolMgr = require("./cuemol_system");
let mgr = new CueMolMgr();
mgr.test();

let fpsElem, canvas;

window.addEventListener("load", () => {
    console.log("onLoad() called!!");

    // gfx_render.initApp();
    // gfx_render.AppMain.init();
    // app = gfx_render.AppMain.getInstance();
    // console.log("app: ", app.toString());
    
    let canvas = document.getElementById('canvas_area');
    mgr.bindCanvas(canvas);
    canvas.addEventListener("mousedown", (event) => {
        console.log("canvas mousedown");
    });
    canvas.addEventListener("mouseup", (event) => {
        console.log("canvas mouseup");
    });
    
    /////

    elem = document.getElementById("update_button");
    elem.addEventListener("click", (event) => {
	    console.log("Update button click() called!!");
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

    // webgl.render(canvas);
    mgr.updateDisplay();
    window.requestAnimationFrame(timer_update_canvas);
}

window.requestAnimationFrame(timer_update_canvas);
