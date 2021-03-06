const _internal = require("bindings")("node_jsbr");
// const _internal = window.myAPI.node_jsbr;
const wrapper_utils = require("./wrapper_utils");
// const wrapper_utils = window.myAPI.wrapper_utils;

function openFile(cm, aSc, aPath, newobj_name, newobj_type, rdr_name, aOptions)
{
    var i, val;
    var StrMgr = cm.getService("StreamManager");
    var reader = StrMgr.createHandler(rdr_name, 0);
    reader.setPath(aPath);
    
    if (aOptions && "object"===typeof aOptions) {
        for (i in aOptions) {
	        val = aOptions[i];
	        console.log("set reader option: "+i+"="+val);
	        reader[i] = val;
        }
    }
    
    aSc.startUndoTxn("Open file");
    var newobj=null;
    try {
        // if (newobj_type)
	    //     newobj = newObj(newobj_type);
        // else
	    newobj = reader.createDefaultObj();
        reader.attach(newobj);
        reader.read();
        reader.detach();
        
        newobj.name = newobj_name;
        aSc.addObject(newobj);
    }
    catch (e) {
        console.log("File Open Error: "+e.message);
        aSc.rollbackUndoTxn();
        return;
    }
    aSc.commitUndoTxn();
    
    return newobj;
};

function makeSel(cm, aSelStr, aUID)
{
    var sel = cm.SelCommand();
    if (aUID) {
        if (!sel.compile(aSelStr, aUID))
	        return null;
    }
    else {
        if (!sel.compile(aSelStr, 0))
	        return null;
    }
    return sel;
}

function createRend(cm, aObj, aRendType, aRendName, aSelStr)
{
    var rend, sce = aObj.getScene();

    var sel;
    try {
        if (aSelStr) {
	        sel = makeSel(cm, aSelStr, sce.uid);
        }
    }
    catch (e) {
        console.log("createRend selstr: error="+e);
    }

    sce.startUndoTxn("Create new representation");
    try {
        rend = aObj.createRenderer(aRendType);
        console.log("*** end_type: "+rend.end_captype);
        rend.name = aRendName;
        if ("sel" in rend && sel)
	        rend.sel = sel;
    }
    catch (e) {
        sce.rollbackUndoTxn();
        throw e;
    }
    sce.commitUndoTxn();
    
    return rend;
}

class Manager {
    constructor() {
        // for program object
        this._prog_data = {};

        // common UBO info
        this._mvp_mat_loc = 0;
        this._mat_ubo = null;

        // for VBOs
        this._draw_data = {};

        _internal.initCueMol("./src/data/sysconfig.xml");
        let sceMgr = wrapper_utils.getService("SceneManager");
        let scene = sceMgr.createScene();
        scene.setName("Test Scene");
        console.log(`Scene created UID: ${scene.getUID()}, name: ${scene.name}`);

        let vw = scene.createView();
        // vw.name = "Primary View";
        this._view = vw;

        // this.loadTestRend(wrapper_utils, scene, vw);
        this.loadTestPDB(wrapper_utils, scene, vw);
    }

    loadTestRend(cuemol, scene, vw) {
        let obj = cuemol.createObj("Object");
        scene.addObject(obj);
        console.log(`Object created UID: ${obj.getUID()}, name: ${obj.name}`);

        // let rend = obj.createRenderer("test");
        let rend = obj.createRenderer("dltest");
        console.log(`Renderer created UID: ${rend.getUID()}, name: ${rend.name}`);
        this.rend = rend;
    }
    
    loadTestPDB(cuemol, scene, vw) {
        let path = "./src/data/1CRN.pdb"

        let cmdMgr = cuemol.getService("CmdMgr");
        
        let load_object = cmdMgr.getCmd("load_object");
        load_object.target_scene = scene;
        load_object.file_path = path;
        // load_object.object_name ="1CRN.pdb";
        load_object.run();
        let mol = load_object.result_object;        
        // let mol = openFile(cuemol, scene, path, "1CRN.pdb", null, "pdb", null);

        let new_rend = cmdMgr.getCmd("new_renderer");
        new_rend.target_object = mol;
        new_rend.renderer_type = "simple";
        new_rend.renderer_name = "simple1";
        new_rend.recenter_view = true;
        new_rend.default_style_name = "DefaultCPKColoring";
        new_rend.run();
        
        // let rend = createRend(cuemol, mol, "simple", "simple1", "*");
        // rend.name = "my renderer";
        // rend.applyStyles("DefaultCPKColoring");
        // let pos = rend.getCenter();
        // console.log("view center:"+pos.toString());
        // vw.setViewCenter(pos);
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
    
    makeModif(event) {
        let modif = event.buttons;
        if (event.ctrlKey) {
            modif += 32;
        }
        if (event.shiftKey) {
            modif += 64;
        }
        return modif;
    }

    bindCanvas(canvas) {
        this._canvas = canvas;
        this._context = canvas.getContext('webgl2');
        let gl = this._context;
        gl.enable(gl.DEPTH_TEST);
        gl.depthFunc(gl.LEQUAL);
        gl.disable(gl.CULL_FACE);
        gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);
        gl.enable(gl.BLEND);

        if (window.devicePixelRatio) {
            this._view.setSclFac(window.devicePixelRatio, window.devicePixelRatio);
        }
        _internal.bindPeer(this._view._wrapped, this);

        canvas.addEventListener("mousedown", (event) => {
            let modif = this.makeModif(event);
            // console.log("canvas mousedown");
            this._view.onMouseDown(event.clientX, event.clientY,
                                   event.screenX, event.screenY,
                                   modif);
        });
        canvas.addEventListener("mouseup", (event) => {
            let modif = this.makeModif(event);
            // console.log("canvas mouseup");
            this._view.onMouseUp(event.clientX, event.clientY,
                                   event.screenX, event.screenY,
                                   modif);
        });
        canvas.addEventListener("mousemove", (event) => {
            let modif = this.makeModif(event);
            // console.log("canvas mousemove");
            this._view.onMouseMove(event.clientX, event.clientY,
                                   event.screenX, event.screenY,
                                   modif);
        });
    }

    updateDisplay() {
        // this._view.rotateView(1.0 * Math.PI / 180, 0, 0);
        this._view.invalidate();
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
            throw `unknown shader type: ${name}`;
        }
    }

    createShader(name, data) {
        const gl = this._context;
        if (name in this._prog_data) {
            // console.log(`name ${name} already exists`);
            // return false;
            return true;
        }
        const program = gl.createProgram();

        for (const [key, value] of Object.entries(data)) {
            // console.log(key, value);
            let shader_type = this.toShaderTypeID(key);
            const shader = gl.createShader(shader_type);
            gl.shaderSource(shader, value);
            gl.compileShader(shader);

            const status = gl.getShaderParameter(shader, gl.COMPILE_STATUS);
            if(!status) {
                const info = gl.getShaderInfoLog(shader);
                console.log(info);
                return false;
            }

            gl.attachShader(program, shader);
        }

        gl.linkProgram(program);
    
        const status = gl.getProgramParameter(program, gl.LINK_STATUS);
        if(!status) {
            const info = gl.getProgramInfoLog(program);
            console.log(info);
            return false;
        }

        // setup common UBO entries
        let mvp_mat_index = gl.getUniformBlockIndex(program, 'mvp_matrix');
        gl.uniformBlockBinding(program, mvp_mat_index, this._mvp_mat_loc);

        this._prog_data[name] = program;
        return true;
    }

    deleteShader(shader_name) {
        const gl = this._context;
        if (!name in this._prog_data) {
            console.log(`name ${name} not defined`);
            return false;
        }
        gl.deleteProgram(this._prog_data[shader_name]);
    }

    enableShader(shader_name) {
        const gl = this._context;
        gl.useProgram(this._prog_data[shader_name]);
    }
    disableShader() {
        const gl = this._context;
        gl.useProgram(null);
    }

    clear(r,g,b) {
        const gl = this._context;
        gl.clearColor(r,g,b,1);
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    }

    checkMvpMatUBO() {
        if (this._mat_ubo===null) {
            // Create UBO
            const gl = this._context;
            let matrix_ubo = gl.createBuffer();
            gl.bindBuffer(gl.UNIFORM_BUFFER, matrix_ubo);
            gl.bufferData(gl.UNIFORM_BUFFER, 4*4*4*2, gl.DYNAMIC_DRAW);
            gl.bindBufferBase(gl.UNIFORM_BUFFER, this._mvp_mat_loc, matrix_ubo);
            gl.bindBuffer(gl.UNIFORM_BUFFER, null);
            this._mat_ubo = matrix_ubo;
            console.log("mvp UBO created");
        }
    }

    setUpModelMat(array_buf) {
        this.checkMvpMatUBO();
        // transfer UBO
        const gl = this._context;
        gl.bindBuffer(gl.UNIFORM_BUFFER, this._mat_ubo);
        gl.bufferSubData(gl.UNIFORM_BUFFER, 0, array_buf);
        gl.bindBuffer(gl.UNIFORM_BUFFER, null);
    }

    setUpProjMat(cx, cy, array_buf) {
        this.checkMvpMatUBO();
        // transfer UBO
        const gl = this._context;
        gl.viewport(0, 0, cx, cy);
        gl.bindBuffer(gl.UNIFORM_BUFFER, this._mat_ubo);
        gl.bufferSubData(gl.UNIFORM_BUFFER, 4*4*4, array_buf);
        gl.bindBuffer(gl.UNIFORM_BUFFER, null);
    }

    resized(width, height) {
        console.log("resized:", width, height);
        this._view.sizeChanged(width, height);
    }

    createBuffer(name, nsize, num_elems, nsize_index, elem_info_str) {
        if (name in this._draw_data) {
            console.log(`name ${name} already exists`);
            return false;
        }

        const gl = this._context;
        let elem_info = JSON.parse(elem_info_str);

        // VAO
        let vao = gl.createVertexArray();
        gl.bindVertexArray(vao);

        let vertexBuffer = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, vertexBuffer);

        const stride = nsize / num_elems;
        elem_info.forEach((value) => {
            let aloc = value["nloc"];
            gl.enableVertexAttribArray(aloc);
            gl.vertexAttribPointer(aloc, value["nelems"], gl.FLOAT, false, stride, value["npos"]);
        });
        gl.bufferData(gl.ARRAY_BUFFER, nsize, gl.STATIC_DRAW);
        console.log("vbo nsize=", nsize);

        // index buffer
        let indexBuffer = null;
        if (nsize_index > 0) {
            indexBuffer = gl.createBuffer();
            gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, indexBuffer);
            gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, nsize_index, gl.STATIC_DRAW);
            // gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, null);
            console.log("ibo nsize=", nsize_index);
        }

        // gl.bindBuffer(gl.ARRAY_BUFFER, null);
        gl.bindVertexArray(null);

        this._draw_data[name] = [vao, vertexBuffer, indexBuffer];

        // const new_id = this._new_draw_id
        // this._draw_data[new_id] = [vao, vertexBuffer];
        // this._new_draw_id ++;

        console.log("create buffer OK, new_id=", name);
        return true;
    }

    drawBuffer(id, nmode, nelems, array_buf, index_buf, isUpdated) {
        const gl = this._context;
        const obj = this._draw_data[id];
        if (isUpdated) {
            // Transfer VBO to GPU
            const vbo = obj[1];
            const ibo = obj[2];
            gl.bindBuffer(gl.ARRAY_BUFFER, vbo);
            gl.bufferSubData(gl.ARRAY_BUFFER, 0, array_buf);
            gl.bindBuffer(gl.ARRAY_BUFFER, null);
            if (index_buf !== null && ibo !== null) {
                gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, ibo);
                gl.bufferSubData(gl.ELEMENT_ARRAY_BUFFER, 0, index_buf);
                gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, null);
            }
        }

        let nglmode = gl.TRIANGLES;
        if (nmode == 4) {
            nglmode = gl.LINES;
            gl.lineWidth(5);
        }
        // else if (nmode == 7) {
        //     nglmode
        // }

        gl.bindVertexArray(obj[0]);
        if (index_buf === null) {
            gl.drawArrays(nglmode, 0, nelems);
        }
        else {
            // console.log("drawelem nelems=", nelems);
            gl.drawElements(nglmode, nelems, gl.UNSIGNED_INT, 0);
        }
        gl.bindVertexArray(null);
    }

    deleteBuffer(id) {
        const gl = this._context;

        if (!id in this._draw_data)
            return false;
        const obj = this._draw_data[id];
        if (obj === null)
            return false;

        delete this._draw_data[id];
        // delete VBO
        gl.deleteBuffer(obj[1]);
        // delete index VBO
        if (obj[2] !== null) {
            gl.deleteBuffer(obj[2]);
        }
        // delete VAO
        gl.deleteVertexArray(obj[0]);

        return true;
    }
}

module.exports.create = function () {
    return new Manager();
}
