console.log("Initializing...");
const CueMolMgr = require("./cuemol_system");
let mgr = new CueMolMgr();
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
