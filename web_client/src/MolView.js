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

export default function MolView({id}) {
    let canvasRef = useRef(0);
    window.Utils = Utils;
    window.onCueMolLoaded = (utils) => {
        // let vecobj = utils.createObject("Vector");
        // console.log(`Vector: ${vecobj}`);
        // vecobj.strvalue = "(1,2,3)"
        // console.log("Vector.strvalue:", vecobj.strvalue);

        // console.log("==========");
        // let vecobj2 = utils.createObject("Vector");
        // // vecobj2.strvalue = "(3,2,1)"
        // vecobj2.set3(3,2,1);
        // console.log("Vec.dot:", vecobj.dot(vecobj2));
        // console.log("Vec.cross:", vecobj.cross(vecobj2).toString());
        
        // vecobj.destroy();
        // vecobj2.destroy();
        
        // console.log("==========");

        // let mgr = utils.getService("SceneManager");
        // console.log(`mgr: ${mgr}`);
        // console.log(`mgr.toString(): ${mgr.toString()}`);
        // mgr.destroy();
        // console.log(`mgr.toString(): ${mgr.toString()}`);

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

        let obj = utils.createObject("Object");
        scene.addObject(obj);
        console.log(`Object created UID: ${obj.getUID()}, name: ${obj.name}`);

        // let rend = obj.createRenderer("test");
        let rend = obj.createRenderer("dltest");
        console.log(`Renderer created UID: ${rend.getUID()}, name: ${rend.name}`);
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
            view.rotateView(0, 1, 0);
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
