const _internal = require("bindings")("node_jsbr");
const wrapper_utils = require("./wrapper_utils");

module.exports = class Manager {
    constructor() {
        // for program object
        this._new_prog_id = 0;
        this._prog_data = [];

        _internal.initCueMol("xxx");
        let sceMgr = wrapper_utils.getService("SceneManager");
        let scene = sceMgr.createScene();
        scene.setName("Test Scene");
        console.log(`Scene created UID: ${scene.getUID()}, name: ${scene.name}`);

        let vw = scene.createView();
        // vw.name = "Primary View";
        this._view = vw;

        let obj = wrapper_utils.createObj("Object");
        scene.addObject(obj);
        console.log(`Object created UID: ${obj.getUID()}, name: ${obj.name}`);

        let rend = obj.createRenderer("test");
        console.log(`Renderer created UID: ${rend.getUID()}, name: ${rend.name}`);
    }

    test() {
        let xx = _internal.getAllClassNamesJSON();
        console.log("_internal.classes:", xx);
        
        let yy;
        xx = wrapper_utils.createObj("Vector");
        console.log("xx._utils:", xx._utils);
        console.log("Vector:", xx.toString());
        xx.x = 0.123
        xx.y = 1.23
        xx.z = 12.3
        console.log("xx._utils:", xx._utils);
        console.log("length:", xx.length());
        console.log("xx._utils:", xx._utils);
        yy = xx.scale(10);
        console.log(`scaled: ${yy}`);
        
        //////////
        
        xx = wrapper_utils.createObj("Color");
        xx.setHSB(1, 1, 120);
        console.log(`color: ${xx}`);
    }
    
    bindCanvas(canvas) {
        this._canvas = canvas;
        this._context = canvas.getContext('webgl2');

        _internal.bindPeer(this._view._wrapped, this);
    }

    updateDisplay() {
        this._view.rotateView(1.0 * Math.PI / 180, 0, 0);
        this._view.checkAndUpdate();
    }

    toShaderTypeID(name) {
        const gl = this._context;
        if (name === "vertex") {
            return gl.VERTEX_SHADER;
        }
        else if (name === "fragment") {
            return gl.FRAGMENT_SHADER;
        }
        else {
            throw "xxx";
        }
    }

    createShader(data) {
        const gl = this._context;

        const program = gl.createProgram();

        for (const [key, value] of Object.entries(data)) {
            console.log(key, value);
            let shader_type = this.toShaderTypeID(key);
            const shader = gl.createShader(shader_type);
            gl.shaderSource(shader, value);
            gl.compileShader(shader);

            const status = gl.getShaderParameter(shader, gl.COMPILE_STATUS);
            if(!status) {
                const info = gl.getShaderInfoLog(shader);
                console.log(info);
                return -1;
            }

            gl.attachShader(program, shader);
        }

        gl.linkProgram(program);
    
        const status = gl.getProgramParameter(program, gl.LINK_STATUS);
        if(!status) {
            const info = gl.getProgramInfoLog(program);
            console.log(info);
            return -1;
        }

        const new_id = this._new_prog_id
        this._prog_data[new_id] = program;
        this._new_prog_id ++;

        return new_id;
    }

}
