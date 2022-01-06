// const { contextBridge, ipcRenderer } = require('electron');
const { ipcRenderer } = require('electron');
const _internal = require('bindings')('node_jsbr');

console.log('ipcRenderer:', ipcRenderer);

window.myAPI = {
  ipcRenderer: ipcRenderer,
  internal: _internal,
  getClassName: function (self) {
    return _internal.getClassName(self);
  },
};

// contextBridge.exposeInMainWorld('myAPI', {
//   internal: _internal,
//   ipcRenderer: ipcRenderer,
//   getClassName: function (self) {
//     return _internal.getClassName(self);
//   },
// });
