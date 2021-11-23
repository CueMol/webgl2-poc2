
'use strict';

// const cuemol = require("bindings")("node_jsbr");
// const wrapper_utils = require("./wrapper_utils");
// const Vector = require("./build/javascript/wrappers/Vector.js");

// cuemol.initCueMol("xxx");
// let xx = cuemol.getAllClassNamesJSON();
// console.log("cuemol.getAllClassNamesJSON()", xx);
// xx = cuemol.getService("ProcessManager");
// let wxx = wrapper_utils.createWrapper(xx);
// // console.log("ProcessManager:", xx.toString());
//  console.log("ProcessManager:", wxx.toString());

// xx = cuemol.createObj("Vector");
// console.log("Vector:", xx.toString());
// console.log("getClassName:", xx.getClassName());
// console.log("getAbiClassName:", xx.getAbiClassName());
// xx.setProp("x", 0.123);
// xx.setProp("y", 1.23);
// xx.setProp("z", 12.3);
// // xx.setProp("x", () => { return 1.234});
// console.log("getProp:", xx.getProp("x"));
// console.log("length:", xx.invokeMethod("length"));
// let yy = xx.invokeMethod("scale", 10);
// console.log("scaled:", yy.getProp("strvalue"));

// //////////

// xx = wrapper_utils.createObj("Vector");
// console.log("xx._utils:", xx._utils);
// console.log("Vector:", xx.toString());
// xx.x = 0.123
// xx.y = 1.23
// xx.z = 12.3
// console.log("xx._utils:", xx._utils);
// console.log("length:", xx.length());
// console.log("xx._utils:", xx._utils);
// yy = xx.scale(10);
// console.log(`scaled: ${yy}`);

// //////////

// xx = wrapper_utils.createObj("Color");
// xx.setHSB(1, 1, 120);
// console.log(`color: ${xx}`);


const electron = require("electron");

const app = electron.app;
const BrowserWindow = electron.BrowserWindow;
// app.allowRendererProcessReuse = false;
// app.disableHardwareAcceleration();
// console.log(app.getGPUInfo("basic"));


let mainWindow;
let subWindow;

app.on('window-all-closed', function() {
    if (process.platform != 'darwin') {
	    app.quit();
    }
});

const createWindow = () => {
    mainWindow = new BrowserWindow({
        webPreferences: {
            nodeIntegration: true,
            contextIsolation: false
        },
    });
    mainWindow.loadFile('./index.html');

    mainWindow.on('ready-to-show', function () {
        mainWindow.show();
        mainWindow.focus();
    });


    mainWindow.on('closed', function() {
	    mainWindow = null;
    });

    // mainWindow.webContents.openDevTools();
}

app.whenReady().then(createWindow);

// const {ipcMain} = require('electron');

// ipcMain.on('asynchronous-message', (event, arg) => {
//     console.log(arg);
//     event.sender.send('asynchronous-reply', 'pong');
// });

// ipcMain.on('resize', (event, x, y, w, h) => {
// 	console.log("resize called", x, y, w, h);
// });
