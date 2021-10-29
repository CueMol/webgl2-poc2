const Manager = require('./context_manager');
const gfx_render = require("bindings")("gfx_render");

module.exports = class WebGLRender {
    constructor() {
    }

    init(canvas) {
        this._mgr = new Manager();
        this._mgr.init(canvas);

        this._proxy = new gfx_render.Proxy();
        this._proxy.setManager(this._mgr);
        this._proxy.create();
    }

    render(canvas) {
        this._proxy.render();
        this._mgr.displayAll();
    }
}
