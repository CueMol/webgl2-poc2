const fs = require('fs');
const path = require('path');
const glmat = require('gl-matrix');
const Buffer = require('buffer').Buffer;

const VERTEX_SIZE = 3; // vec3
const COLOR_SIZE  = 4; // vec4
const STRIDE = (VERTEX_SIZE + COLOR_SIZE) * Float32Array.BYTES_PER_ELEMENT;
const POSITION_OFFSET = 0;
const COLOR_OFFSET = VERTEX_SIZE * Float32Array.BYTES_PER_ELEMENT;

const radius = 100;

function loadShader(gl) {
    let vertexShaderSource = fs.readFileSync(path.resolve(__dirname, 'shaders/vertex_shader.glsl'));

    // console.log("rawdata", vertexShaderSource);
    
    const vertexShader = gl.createShader(gl.VERTEX_SHADER);
    gl.shaderSource(vertexShader, vertexShaderSource);
    gl.compileShader(vertexShader);
    
    const vShaderCompileStatus = gl.getShaderParameter(vertexShader, gl.COMPILE_STATUS);
    if(!vShaderCompileStatus) {
        const info = gl.getShaderInfoLog(vertexShader);
        console.log(info);
    }

    let fragmentShaderSource = fs.readFileSync(path.resolve(__dirname, 'shaders/fragment_shader.glsl'));
    const fragmentShader = gl.createShader(gl.FRAGMENT_SHADER);
    gl.shaderSource(fragmentShader, fragmentShaderSource);
    gl.compileShader(fragmentShader);

    const fShaderCompileStatus = gl.getShaderParameter(fragmentShader, gl.COMPILE_STATUS);
    if(!fShaderCompileStatus) {
        const info = gl.getShaderInfoLog(fragmentShader);
        console.log(info);
    }

    const program = gl.createProgram();
    gl.attachShader(program, vertexShader);
    gl.attachShader(program, fragmentShader);
    gl.linkProgram(program);

    const linkStatus = gl.getProgramParameter(program, gl.LINK_STATUS);
    if(!linkStatus) {
        const info = gl.getProgramInfoLog(program);
        console.log(info);
    }

    return program;
}


class AttrInfo {
    constructor(iloc, nelems, itype, ipos) {
        this.iloc = iloc;
        this.nelems = nelems;
        this.itype = itype;
        this.ipos = ipos;
    }
}

class DrawEntry {
    constructor(gl, val, cal, nsize, nelems) {
        this._nsize = nsize;
        this._nelems = nelems;

        this._buf = new Float32Array(nsize);
        console.log("alloc Float32Array nsize=", nsize);

        // VAO
        this._vao = gl.createVertexArray();
        gl.bindVertexArray(this._vao);

        // Prepare VBO
        this._vertexBuffer = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, this._vertexBuffer);
        gl.enableVertexAttribArray(val);
        gl.vertexAttribPointer(val, VERTEX_SIZE, gl.FLOAT, false, STRIDE, POSITION_OFFSET);
        gl.enableVertexAttribArray(cal);
        gl.vertexAttribPointer(cal, COLOR_SIZE, gl.FLOAT, false, STRIDE, COLOR_OFFSET);
        gl.bufferData(gl.ARRAY_BUFFER, this._buf, gl.STATIC_DRAW);

        gl.bindVertexArray(null);
        gl.bindBuffer(gl.ARRAY_BUFFER, null);
    }

    draw(gl) {
        gl.bindVertexArray(this._vao);
        gl.drawArrays(gl.TRIANGLES, 0, this._nelems);
        gl.bindVertexArray(null);
    }
}

module.exports = class Manager {
    constructor() {
        this._radian = 0;
        this._dist = 100;
        this._new_draw_id = 0;
        this._draw_data = [];

        this._new_prog_id = 0;
        this._prog_data = [];
    }
    
    // Bind this manager to specific canvas
    // Called from JS side
    init(canvas) {
        this._canvas = canvas;
        this._context = canvas.getContext('webgl2');
    }

    // Create shader program
    // Called from native side
    createShader(vert_source, frag_source) {
        console.log("vert_source:", vert_source);
        console.log("frag_source:", frag_source);
        const gl = this._context;

        const vertexShader = gl.createShader(gl.VERTEX_SHADER);
        gl.shaderSource(vertexShader, vert_source);
        gl.compileShader(vertexShader);
    
        const vShaderCompileStatus = gl.getShaderParameter(vertexShader, gl.COMPILE_STATUS);
        if(!vShaderCompileStatus) {
            const info = gl.getShaderInfoLog(vertexShader);
            console.log(info);
            return -1;
        }

        const fragmentShader = gl.createShader(gl.FRAGMENT_SHADER);
        gl.shaderSource(fragmentShader, frag_source);
        gl.compileShader(fragmentShader);

        const fShaderCompileStatus = gl.getShaderParameter(fragmentShader, gl.COMPILE_STATUS);
        if(!fShaderCompileStatus) {
            const info = gl.getShaderInfoLog(fragmentShader);
            console.log(info);
            return -1;
        }

        const program = gl.createProgram();
        gl.attachShader(program, vertexShader);
        gl.attachShader(program, fragmentShader);
        gl.linkProgram(program);

        const linkStatus = gl.getProgramParameter(program, gl.LINK_STATUS);
        if(!linkStatus) {
            const info = gl.getProgramInfoLog(program);
            console.log(info);
            return -1;
        }

        const new_id = this._new_prog_id
        this._prog_data[new_id] = program;
        this._new_prog_id ++;

        this._vertexAttribLocation = gl.getAttribLocation(program, 'vertexPosition');
        this._colorAttribLocation  = gl.getAttribLocation(program, 'color');
        this._modelLocation = gl.getUniformLocation(program, 'model');
        this._viewLocation = gl.getUniformLocation(program, 'view');
        this._projectionLocation = gl.getUniformLocation(program, 'projection');

        return new_id;
    }

    // Create new WebGL buffer
    // Called from native side
    createBuffer(nsize, num_elems, elem_info_str) {
        const gl = this._context;
        // console.log("elem info:", elem_info_str);
        let elem_info = JSON.parse(elem_info_str);
        console.log("elem info:", elem_info);

        let prog_id = 0;
        let program = this._prog_data[prog_id];
        elem_info.forEach((value) => {
            let aloc = gl.getAttribLocation(program, value["name"]);
        });

        let obj = new DrawEntry(gl,
                                this._vertexAttribLocation,
                                this._colorAttribLocation,
                                nsize,
                                num_elems);
        const new_id = this._new_draw_id
        this._draw_data[new_id] = obj;
        this._new_draw_id ++;

        // console.log("new_id=", new_id);
        // console.log("buf=", this._draw_data[id]._buf);
        return new_id;
    }

    getBuffer(id) {
        return this._draw_data[id]._buf;
    }

    // Send WebGL buffer to GPU
    // Called from native side
    sendBuffer(id) {
        const gl = this._context;
        const obj = this._draw_data[id];
        // Transfer VBO to GPU
        gl.bindBuffer(gl.ARRAY_BUFFER, obj._vertexBuffer);
        gl.bufferSubData(gl.ARRAY_BUFFER, 0, obj._buf);
        gl.bindBuffer(gl.ARRAY_BUFFER, null);
    }

    freeBuffer(id) {
        if (id >= this._draw_data.length)
            return false;
        const obj = this._draw_data[id];
        if (obj === null)
            return false;
        const gl = this._context;
        this._draw_data[id] = null;
        gl.deleteBuffer(obj._vertexBuffer);
        return true;
    }

    // Draw the scene
    displayAll() {
        const canvas = this._canvas;
        const gl = this._context;

        this._radian += 1.0 * Math.PI / 180;

        gl.clearColor(0.0, 0.0, 0.0, 1.0);
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        // gl.useProgram(this._program);
        gl.useProgram(this._prog_data[0]);

        this.setUpModelMat(this._radian);
        let cx = canvas.width;
        let cy = canvas.height;
        gl.viewport(0, 0, cx, cy);
        this.setUpProjMat(cx, cy);

        this._draw_data.forEach((value) => {
            if (value !==null) {
                value.draw(gl);
            }
        });

        // reset
        gl.useProgram(null);
        gl.bindBuffer(gl.ARRAY_BUFFER, null);

        gl.flush();
    }
    

    setUpModelMat(radian) {
        const gl = this._context;
        const model = glmat.mat4.create();
        glmat.mat4.identity(model);
        glmat.mat4.translate(model, model, [0, 0, -this._dist]);
        glmat.mat4.rotateY(model, model, radian);

        // glmat.mat4.translate(model, model, [0, 0, 0]);

        gl.uniformMatrix4fv(this._modelLocation, false, model);
    }

    setUpProjMat(cx, cy) {
        const gl = this._context;
        const projection = glmat.mat4.create();
        const slabdepth = 100.0;
        const slabnear = this._dist-slabdepth/2.0;
        const slabfar  = this._dist+slabdepth;
        const fasp = cx / cy;
        const vw = 100.0;
        // gl.viewport(0, 0, cx, cy);
        glmat.mat4.ortho(projection, -vw*fasp, vw*fasp,
                         -vw, vw, slabnear, slabfar);        
        gl.uniformMatrix4fv(this._projectionLocation, false, projection);
    }

}
