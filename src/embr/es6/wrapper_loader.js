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
import EmView from './wrappers/EmView.js';

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
    "ViewInputConfig": ViewInputConfig,
    "EmView": EmView,
};

export { wrapper_map as default};
