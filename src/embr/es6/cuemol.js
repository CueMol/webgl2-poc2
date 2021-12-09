import BaseWrapper from './base_wrapper.js';

import AbstractColor from './wrappers/AbstractColor.js';
import AnimMgr from './wrappers/AnimMgr.js';
import AnimObj from  './wrappers/AnimObj.js';
import ByteArray from  './wrappers/ByteArray.js';
import Camera from  './wrappers/Camera.js';
import CmdMgr from  './wrappers/CmdMgr.js';
import Color from  './wrappers/Color.js';
import Command from  './wrappers/Command.js';
import DrawObj from  './wrappers/DrawObj.js';
import GradientColor from  './wrappers/GradientColor.js';
import InOutHandler from  './wrappers/InOutHandler.js';
import LScrCallBack from  './wrappers/LScrCallBack.js';
import LScrObject from  './wrappers/LScrObject.js';
import LoadObjectCommand from  './wrappers/LoadObjectCommand.js';
import LoadSceneCommand from  './wrappers/LoadSceneCommand.js';
import Matrix from  './wrappers/Matrix.js';
import MolColorRef from './wrappers/MolColorRef.js';
import MsgLog from './wrappers/MsgLog.js';
import MultiGradient from './wrappers/MultiGradient.js';
import NamedColor from './wrappers/NamedColor.js';
import NewRendererCommand from './wrappers/NewRendererCommand.js';
import NewSceneCommand from './wrappers/NewSceneCommand.js';
import ObjReader from './wrappers/ObjReader.js';
import ObjWriter from './wrappers/ObjWriter.js';
import Object from './wrappers/Object.js';
import ProcessManager from './wrappers/ProcessManager.js';
import Quat from './wrappers/Quat.js';
import RangeSet from './wrappers/RangeSet.js';
import RegExpr from './wrappers/RegExpr.js';
import RendGroup from './wrappers/RendGroup.js';
import Renderer from './wrappers/Renderer.js';
import ScalarObject from './wrappers/ScalarObject.js';
import Scene from './wrappers/Scene.js';
import SceneExporter from './wrappers/SceneExporter.js';
import SceneManager from './wrappers/SceneManager.js';
import SceneXMLReader from './wrappers/SceneXMLReader.js';
import SceneXMLWriter from './wrappers/SceneXMLWriter.js';
import ScrEventManager from './wrappers/ScrEventManager.js';
import StreamManager from './wrappers/StreamManager.js';
import StyleManager from './wrappers/StyleManager.js';
import StyleSet from './wrappers/StyleSet.js';
import TestDLRenderer from './wrappers/TestDLRenderer.js';
import TestRenderer from './wrappers/TestRenderer.js';
import TextImgBuf from './wrappers/TextImgBuf.js';
import TimeValue from './wrappers/TimeValue.js';
import Vector from './wrappers/Vector.js';
import View from './wrappers/View.js';
import ViewInputConfig from './wrappers/ViewInputConfig.js';

let wrapper_map = {
    "AbstractColor": AbstractColor,
    "AnimMgr": AnimMgr,
    "AnimObj": AnimObj,
    "ByteArray": ByteArray,
    "Camera": Camera,
    "CmdMgr": CmdMgr,
    "Color": Color,
    "Command": Command,
    "DrawObj": DrawObj,
    "GradientColor": GradientColor,
    "InOutHandler": InOutHandler,
    "LScrCallBack": LScrCallBack,
    "LScrObject": LScrObject,
    "LoadObjectCommand": LoadObjectCommand,
    "LoadSceneCommand": LoadSceneCommand,
    "Matrix": Matrix,
    "MolColorRef": MolColorRef,
    "MsgLog": MsgLog,
    "MultiGradient": MultiGradient,
    "NamedColor": NamedColor,
    "NewRendererCommand": NewRendererCommand,
    "NewSceneCommand": NewSceneCommand,
    "ObjReader": ObjReader,
    "ObjWriter": ObjWriter,
    "Object": Object,
    "ProcessManager": ProcessManager,
    "Quat": Quat,
    "RangeSet": RangeSet,
    "RegExpr": RegExpr,
    "RendGroup": RendGroup,
    "Renderer": Renderer,
    "ScalarObject": ScalarObject,
    "Scene": Scene,
    "SceneExporter": SceneExporter,
    "SceneManager": SceneManager,
    "SceneXMLReader": SceneXMLReader,
    "SceneXMLWriter": SceneXMLWriter,
    "ScrEventManager": ScrEventManager,
    "StreamManager": StreamManager,
    "StyleManager": StyleManager,
    "StyleSet": StyleSet,
    "TestDLRenderer": TestDLRenderer,
    "TestRenderer": TestRenderer,
    "TextImgBuf": TextImgBuf,
    "TimeValue": TimeValue,
    "Vector": Vector,
    "View": View,
    "ViewInputConfig": ViewInputConfig
};

export default class CueMol {
    constructor(module) {
        this.module = module;
    }

    convStr(value) {
        let arr = this.module.intArrayFromString(value);
        let ptr = this.module.allocate(arr, this.module.ALLOC_NORMAL);
        return ptr;
    }

    initCueMol(config) {
        let pstr = this.convStr(config);
        let pobj = this.module["_initCueMol"](pstr);
        this.module._free(pstr);
        return pobj
    }

    createWrapper(pobj) {
        let pstr = this.module["_getClassName"](pobj);
        let className = this.module.UTF8ToString(pstr);
        console.log(`createWrapper: className=${className}`);
        // let moduleFile = path.join(WRAPPERS_DIR, className);
        // let moduleFile = path.join(__dirname, "wrappers", className);
        // console.log(`module file: ${moduleFile}`);
        // const Klass = require(moduleFile);
        const Klass = wrapper_map[className];
        return new Klass(pobj, this);
    }

    createObject(className) {
        let pstr = this.convStr(className);
        let pobj = this.module["_createObject"](pstr);
        this.module._free(pstr);
        return this.createWrapper(pobj);
    }
    
    getService(className) {
        let pstr = this.convStr(className);
        let pobj = this.module["_getService"](pstr);
        this.module._free(pstr);
        return this.createWrapper(pobj);
    }

    varToVal(args, ind) {
        let type_id = this.module["_getVarArgsTypeID"](args, ind);
        console.log(`varToVal ${ind} -> type ID ${type_id}`);
        if (type_id === 0) {
            // LT_NULL
            return null;
        }
        else if (type_id === 1) {
            // LT_BOOLEAN
            return this.module["_getBoolVarArgs"](args, ind);
        }
        else if (type_id === 2) {
            // LT_INTEGER
            return this.module["_getIntVarArgs"](args, ind);
        }
        else if (type_id === 3) {
            // LT_REAL
            return this.module["_getRealVarArgs"](args, ind);
        }
        else if (type_id === 4) {
            // LT_STRING
            let pstr = this.module["_getStrVarArgs"](args, ind);
            return this.module.UTF8ToString(pstr);
        }
        else if (type_id === 5) {
            // LT_OBJECT
            let pobj = this.module["_getObjectVarArgs"](args, ind);
            return this.createWrapper(pobj);
        }
        // else if (type_id === 6) {
        //     // LT_SMARTPTR
        // }
        else if (type_id === 7) {
            // LT_ENUM
        }
        else if (type_id === 8) {
            // LT_ARRAY
        }
        else if (type_id === 9) {
            // LT_LIST
        }
        else if (type_id === 10) {
            // LT_DICT
        }

        // error
        throw `unsupported type ID: ${type_id}`;
    }

    valToVar(args, ind, value) {
        if (value === null || value === undefined) {
            // LT_NULL
        }
        else if (typeof value === "boolean") {
            // LT_BOOLEAN
            this.module["_setBoolVarArgs"](args, ind, value);
        }
        else if (typeof value === "number") {
            // LT_REAL / LT_INT
            this.module["_setRealVarArgs"](args, ind, value);
        }
        else if (typeof value === "string") {
            // LT_STRING
            let pstr = this.convStr(value);
            try {
                this.module["_setStrVarArgs"](args, ind, pstr);
            }
            finally {
                this.module._free(pstr);
            }
        }
        else if (value instanceof BaseWrapper) {
            // LT_OBJECT
            this.module["_setObjectVarArgs"](args, ind, value.wrapped);
        }
        else {
            console.log(`unsupported type: ${typeof value}`);
        }
    }

}
