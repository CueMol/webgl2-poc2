import React, { useEffect, useRef } from "react";
import "./MolView.css";
import useMouseEvent from "./MouseEvents";

import Utils from './libs/cuemol';

function updateMolViewSize(canvas, vw, dpr) {
    let {width, height} = canvas.getBoundingClientRect();
    console.log(`canvas resize: ${width} x ${height}`);
    canvas.width = width * dpr;
    canvas.height = height * dpr;
    if (vw) {
        vw.sizeChanged(width, height);
    }
}

function combRefs(ref1, ref2) {
    return (node) => {
        ref1(node);
        ref2.current = node;
    };
}

function test_func1(utils) {
    let vecobj = utils.createObject("Vector");
    console.log(`Vector: ${vecobj}`);
    vecobj.strvalue = "(1,2,3)"
    console.log("Vector.strvalue:", vecobj.strvalue);

    console.log("==========");
    let vecobj2 = utils.createObject("Vector");
    // vecobj2.strvalue = "(3,2,1)"
    vecobj2.set3(3,2,1);
    console.log("Vec.dot:", vecobj.dot(vecobj2));
    console.log("Vec.cross:", vecobj.cross(vecobj2).toString());
    
    vecobj.destroy();
    vecobj2.destroy();
    
    console.log("==========");

    let mgr = utils.getService("SceneManager");
    console.log(`mgr: ${mgr}`);
    console.log(`mgr.toString(): ${mgr.toString()}`);
    mgr.destroy();
    console.log(`mgr.toString(): ${mgr.toString()}`);
}

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

function test_load_func1(utils) {
    let sceMgr = utils.getService("SceneManager");
    let scene = sceMgr.createScene();
    scene.setName("Test Scene");
    console.log(`Scene created UID: ${scene.getUID()}, name: ${scene.name}`);

    let vw = scene.createView();
    vw.name = "Primary View";

    const molview = document.getElementById('mol_view');
    console.log(`molview: ${molview}`);
    vw.bind("#mol_view");
    
    let canvas = document.getElementById('mol_view');
    canvas.view = vw;
    updateMolViewSize(canvas, vw, window.devicePixelRatio || 1.0);
    if (window.devicePixelRatio) {
        vw.setSclFac(window.devicePixelRatio, window.devicePixelRatio);
        // vw.resetSclFac();
    }

    //////////

    let path = "/1CRN.pdb"
    let mol = openFile(utils, scene, path, "1CRN.pdb", null, "pdb", null);
    let rend = createRend(utils, mol, "simple", "simple1", "*");
    rend.name = "my renderer";
    rend.applyStyles("DefaultCPKColoring");
    let pos = rend.getCenter();
    console.log("view center:"+pos.toString());
    vw.setViewCenter(pos);
    

    // let obj = utils.createObject("Object");
    // scene.addObject(obj);
    // console.log(`Object created UID: ${obj.getUID()}, name: ${obj.name}`);

    // // let rend = obj.createRenderer("test");
    // let rend = obj.createRenderer("dltest");
    // console.log(`Renderer created UID: ${rend.getUID()}, name: ${rend.name}`);
}

export default function MolView({id}) {
    let canvasRef = useRef(0);
    window.Utils = Utils;
    window.onCueMolLoaded = (utils) => {
        test_load_func1(utils);
    };

    useEffect(()=>{
        const head = document.getElementsByTagName('head')[0];

        const scriptUrl = document.createElement('script');
        scriptUrl.type = 'module';
        scriptUrl.src = 'load_wasm.js';

        head.appendChild(scriptUrl);

    }, []);

    let then = 0;

    let timer_update_canvas = (timestamp) => {
        timestamp *= 0.001;
        const deltaTime = timestamp - then;
        then = timestamp;
        const fps = 1 / deltaTime;
        // fpsElem.textContent = fps.toFixed(1);
        // console.log(`fps: ${fps}`);
        if (canvasRef.current && canvasRef.current.view) {
            let view = canvasRef.current.view;
            // view.invalidate();
            // view.rotateView(0, 1, 0);
            view.checkAndUpdate();
        }

        requestAnimationFrame(timer_update_canvas);
    }
    
    useEffect( () => {
        requestAnimationFrame(timer_update_canvas);
    }, []);

    useEffect(()=>{
        const canvas = document.getElementById('mol_view');
        const placeholder = document.getElementById('placeholder');

        const resizeObserver = new ResizeObserver(entries => {
            updateMolViewSize(canvas, canvas.view, window.devicePixelRatio || 1.0);
        });
        resizeObserver.observe(canvas);

        updateMolViewSize(canvas, canvas.view, window.devicePixelRatio || 1.0);

    }, []);

    const [ref, mouseEvent] = useMouseEvent();

    useEffect(() => {
        // console.log(mouseEvent, canvasRef.current.view);
        let view = canvasRef.current.view;
        if (view) {
            if (mouseEvent.event === "down") {
                view.onMouseDown(mouseEvent.clientX, mouseEvent.clientY,
                                 mouseEvent.screenX, mouseEvent.screenY,
                                 mouseEvent.modif);
            } else if (mouseEvent.event === "up") {
                view.onMouseUp(mouseEvent.clientX, mouseEvent.clientY,
                               mouseEvent.screenX, mouseEvent.screenY,
                               mouseEvent.modif);
            } else if (mouseEvent.event === "move") {
                view.onMouseMove(mouseEvent.clientX, mouseEvent.clientY,
                                 mouseEvent.screenX, mouseEvent.screenY,
                                 mouseEvent.modif);
            }
        }
    }, [mouseEvent]);

    return (
        <div id="placeholder">
            <canvas id={id} ref={combRefs(ref, canvasRef)}></canvas>
        </div>
    );
}
