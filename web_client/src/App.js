import { useEffect } from 'react';
import Utils from './libs/cuemol';

export default function App() {
    window.Utils = Utils;
    window.onCueMolLoaded = (utils) => {
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

    };

    useEffect(()=>{
        const head = document.getElementsByTagName('head')[0];

        const scriptUrl = document.createElement('script');
        scriptUrl.type = 'module';
        scriptUrl.src = 'load_wasm.js';

        head.appendChild(scriptUrl);
    });
    return (<div className="App">
            XXX
            </div>
           );
}
