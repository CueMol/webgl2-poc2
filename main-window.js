console.log("Initializing...");
const gfx_render = require("bindings")("gfx_render");
const WebGLRender = require('./webgl_native_render');
let webgl = new WebGLRender();

const cuemol = require("bindings")("node_jsbr");
console.log("cuemol.hello():", cuemol.hello());

const drawCanvas = () => {
    let placeholder = document.getElementById('placeholder');
    let rect = placeholder.getBoundingClientRect();
    let w = rect.width;
    let h = rect.height
    // console.log("PLH width", w, "height", h);
    // console.log("canvas rect", rect);

    let canvas = document.getElementById('canvas_area');
    canvas.width = w;
    canvas.height = h;
    
    webgl.render(canvas);
}

const resizeObserver = new ResizeObserver(entries => {
    drawCanvas();
    for (const entry of entries) {
        const rect = entry.contentRect;
        // console.log("resize called", rect);
    }
});

let fpsElem, canvas;

window.addEventListener("load", () => {
    console.log("onLoad() called!!");

    // gfx_render.initApp();
    gfx_render.AppMain.init();
    app = gfx_render.AppMain.getInstance();
    console.log("app: ", app.toString());
    
    let canvas = document.getElementById('canvas_area');
    // let id = mgr.registerCanvas(canvas);
    // console.log("register canvas id:", canvas.getAttribute("mgr_idx"));
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
        drawCanvas();
    });

    resizeObserver.observe(document.getElementById("placeholder"));

    webgl.init(canvas);
    drawCanvas();

    fpsElem = document.getElementById('fps');
});

let then = 0;

let timer_update_canvas = (timestamp) => {
    timestamp *= 0.001;
    const deltaTime = timestamp - then;
    then = timestamp;
    const fps = 1 / deltaTime;
    fpsElem.textContent = fps.toFixed(1);

    webgl.render(canvas);
    window.requestAnimationFrame(timer_update_canvas);
}

window.requestAnimationFrame(timer_update_canvas);

// // const gfx_render = require("bindings")("gfx_render");

// window.addEventListener("load", () => {
//     console.log("onLoad() called!!");
//     console.log("Addon: " + gfx_render)
//     result = gfx_render.hello();
//     console.log("result: " + result)
// });
