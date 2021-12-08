const path = require("path");
const CopyPlugin = require('copy-webpack-plugin');

module.exports = function override(config, env) {
    config.plugins.push(
        new CopyPlugin({
            patterns: [
                {
                    from: path.resolve(__dirname, "src/load_wasm.js"),
                },
                {
                    from: path.resolve(__dirname, "src/embr.js"),
                },
                {
                    from: path.resolve(__dirname, "src/embr.wasm"),
                },
                {
                    from: path.resolve(__dirname, "src/embr.data"),
                },
                {
                    from: path.resolve(__dirname, "src/libs/"),
                    to: "libs/",
                },
            ],
        })
    );
    
    return config;
};
