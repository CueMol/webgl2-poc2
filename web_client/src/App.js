import React, { useEffect, useState, createRef } from 'react'

import Utils from './libs/cuemol';
import MolView from "./MolView";

function updateMolViewSize(canvas, vw, dpr) {
    let {width, height} = canvas.getBoundingClientRect();
    console.log(`canvas resize: ${width} x ${height}`);
    canvas.width = width * dpr;
    canvas.height = height * dpr;
    if (vw !== null) {
        vw.sizeChanged(width, height);
    }
}

export default function App() {
    let view = null;
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
        view = vw;

        const molview = document.getElementById('mol_view');
        console.log(`molview: ${molview}`);
        vw.bind("#mol_view");
        
        const canvas = document.getElementById('mol_view');
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
        if (view !== null) {
            // view.invalidate();
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
            updateMolViewSize(canvas, view, window.devicePixelRatio || 1.0);
        });
        resizeObserver.observe(canvas);

        updateMolViewSize(canvas, view, window.devicePixelRatio || 1.0);

    }, []);

    return (<div className="App">
                XXX
            <MolView id="mol_view"/>
            </div>
           );
}
