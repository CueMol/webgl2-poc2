import { useEffect } from 'react';

export default function App() {
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
