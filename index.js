
'use strict';

const cuemol = require("bindings")("node_jsbr");
cuemol.initCueMol();
let xx = cuemol.getAllClassNamesJSON();
console.log("cuemol.getAllClassNamesJSON()", xx);
xx = cuemol.getService("ProcessManager");
console.log("ProcessManager:", xx.toString());

xx = cuemol.createObj("Vector");
console.log("Vector:", xx.toString());
console.log("getClassName:", xx.getClassName());
console.log("getAbiClassName:", xx.getAbiClassName());
xx.setProp("x", 0.123);
console.log("getProp:", xx.getProp("x"));

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
