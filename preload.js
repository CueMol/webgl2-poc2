const { contextBridge, ipcRenderer} = require("electron");

contextBridge.exposeInMainWorld(
  'myAPI', {
    fs : require('fs'),
      node_jsbr: require("bindings")("node_jsbr"),
      cuemol_system: require("./cuemol_system"),
      wrapper_utils: require("./wrapper_utils"),
    ipcRenderer : ipcRenderer,
  }
);
