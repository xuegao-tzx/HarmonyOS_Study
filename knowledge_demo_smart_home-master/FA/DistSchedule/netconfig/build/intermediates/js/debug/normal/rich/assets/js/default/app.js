/******/ (function(modules) { // webpackBootstrap
/******/ 	// The module cache
/******/ 	var installedModules = {};
/******/
/******/ 	// The require function
/******/ 	function __webpack_require__(moduleId) {
/******/
/******/ 		// Check if module is in cache
/******/ 		if(installedModules[moduleId]) {
/******/ 			return installedModules[moduleId].exports;
/******/ 		}
/******/ 		// Create a new module (and put it into the cache)
/******/ 		var module = installedModules[moduleId] = {
/******/ 			i: moduleId,
/******/ 			l: false,
/******/ 			exports: {}
/******/ 		};
/******/
/******/ 		// Execute the module function
/******/ 		modules[moduleId].call(module.exports, module, module.exports, __webpack_require__);
/******/
/******/ 		// Flag the module as loaded
/******/ 		module.l = true;
/******/
/******/ 		// Return the exports of the module
/******/ 		return module.exports;
/******/ 	}
/******/
/******/
/******/ 	// expose the modules object (__webpack_modules__)
/******/ 	__webpack_require__.m = modules;
/******/
/******/ 	// expose the module cache
/******/ 	__webpack_require__.c = installedModules;
/******/
/******/ 	// define getter function for harmony exports
/******/ 	__webpack_require__.d = function(exports, name, getter) {
/******/ 		if(!__webpack_require__.o(exports, name)) {
/******/ 			Object.defineProperty(exports, name, { enumerable: true, get: getter });
/******/ 		}
/******/ 	};
/******/
/******/ 	// define __esModule on exports
/******/ 	__webpack_require__.r = function(exports) {
/******/ 		if(typeof Symbol !== 'undefined' && Symbol.toStringTag) {
/******/ 			Object.defineProperty(exports, Symbol.toStringTag, { value: 'Module' });
/******/ 		}
/******/ 		Object.defineProperty(exports, '__esModule', { value: true });
/******/ 	};
/******/
/******/ 	// create a fake namespace object
/******/ 	// mode & 1: value is a module id, require it
/******/ 	// mode & 2: merge all properties of value into the ns
/******/ 	// mode & 4: return value when already ns object
/******/ 	// mode & 8|1: behave like require
/******/ 	__webpack_require__.t = function(value, mode) {
/******/ 		if(mode & 1) value = __webpack_require__(value);
/******/ 		if(mode & 8) return value;
/******/ 		if((mode & 4) && typeof value === 'object' && value && value.__esModule) return value;
/******/ 		var ns = Object.create(null);
/******/ 		__webpack_require__.r(ns);
/******/ 		Object.defineProperty(ns, 'default', { enumerable: true, value: value });
/******/ 		if(mode & 2 && typeof value != 'string') for(var key in value) __webpack_require__.d(ns, key, function(key) { return value[key]; }.bind(null, key));
/******/ 		return ns;
/******/ 	};
/******/
/******/ 	// getDefaultExport function for compatibility with non-harmony modules
/******/ 	__webpack_require__.n = function(module) {
/******/ 		var getter = module && module.__esModule ?
/******/ 			function getDefault() { return module['default']; } :
/******/ 			function getModuleExports() { return module; };
/******/ 		__webpack_require__.d(getter, 'a', getter);
/******/ 		return getter;
/******/ 	};
/******/
/******/ 	// Object.prototype.hasOwnProperty.call
/******/ 	__webpack_require__.o = function(object, property) { return Object.prototype.hasOwnProperty.call(object, property); };
/******/
/******/ 	// __webpack_public_path__
/******/ 	__webpack_require__.p = "";
/******/
/******/
/******/ 	// Load entry module and return exports
/******/ 	return __webpack_require__(__webpack_require__.s = "d:\\Github\\HarmonyOS_Study\\knowledge_demo_smart_home-master\\FA\\DistSchedule\\netconfig\\src\\main\\js\\default\\app.js?entry");
/******/ })
/************************************************************************/
/******/ ({

/***/ "./lib/manifest-plugin.js!d:\\Github\\HarmonyOS_Study\\knowledge_demo_smart_home-master\\FA\\DistSchedule\\netconfig\\build\\intermediates\\js\\debug\\jsManifest\\default\\manifest.json":
/*!***********************************************************************************************************************************************************************************!*\
  !*** ./lib/manifest-plugin.js!d:/Github/HarmonyOS_Study/knowledge_demo_smart_home-master/FA/DistSchedule/netconfig/build/intermediates/js/debug/jsManifest/default/manifest.json ***!
  \***********************************************************************************************************************************************************************************/
/*! exports provided: appID, appName, versionName, versionCode, minPlatformVersion, pages, deviceType, window, default */
/***/ (function(module) {

module.exports = JSON.parse("{\"appID\":\"com.example.distschedule\",\"appName\":\"$string:netconfig_MainAbility\",\"versionName\":\"1.0.0\",\"versionCode\":1000000,\"minPlatformVersion\":5,\"pages\":[\"pages/index/index\"],\"deviceType\":[\"phone\"],\"window\":{\"autoDesignWidth\":true,\"designWidth\":720}}");

/***/ }),

/***/ "./lib/script.js!./node_modules/babel-loader/lib/index.js?presets[]=C:\\Users\\lenovo\\AppData\\Local\\Huawei\\Sdk\\js\\2.2.0.3\\build-tools\\ace-loader\\node_modules\\@babel\\preset-env&plugins[]=C:\\Users\\lenovo\\AppData\\Local\\Huawei\\Sdk\\js\\2.2.0.3\\build-tools\\ace-loader\\node_modules\\@babel\\plugin-transform-modules-commonjs&comments=false!./lib/resource-reference-script.js!./lib/manifest-loader.js?path=d:\\Github\\HarmonyOS_Study\\knowledge_demo_smart_home-master\\FA\\DistSchedule\\netconfig\\src\\main\\js\\default\\app.js!d:\\Github\\HarmonyOS_Study\\knowledge_demo_smart_home-master\\FA\\DistSchedule\\netconfig\\src\\main\\js\\default\\app.js":
/*!**************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************!*\
  !*** ./lib/script.js!./node_modules/babel-loader/lib?presets[]=C:/Users/lenovo/AppData/Local/Huawei/Sdk/js/2.2.0.3/build-tools/ace-loader/node_modules/@babel/preset-env&plugins[]=C:/Users/lenovo/AppData/Local/Huawei/Sdk/js/2.2.0.3/build-tools/ace-loader/node_modules/@babel/plugin-transform-modules-commonjs&comments=false!./lib/resource-reference-script.js!./lib/manifest-loader.js?path=d:/Github/HarmonyOS_Study/knowledge_demo_smart_home-master/FA/DistSchedule/netconfig/src/main/js/default/app.js!d:/Github/HarmonyOS_Study/knowledge_demo_smart_home-master/FA/DistSchedule/netconfig/src/main/js/default/app.js ***!
  \**************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

module.exports = function(module, exports, $app_require$){"use strict";

var _interopRequireDefault = __webpack_require__(/*! @babel/runtime/helpers/interopRequireDefault */ "./node_modules/@babel/runtime/helpers/interopRequireDefault.js");

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports["default"] = void 0;

var _faNetconfig = _interopRequireDefault(__webpack_require__(/*! ./common/fa-netconfig.js */ "d:\\Github\\HarmonyOS_Study\\knowledge_demo_smart_home-master\\FA\\DistSchedule\\netconfig\\src\\main\\js\\default\\common\\fa-netconfig.js"));

var _default = {
  NetConfig: _faNetconfig["default"],
  Product: {
    productId: '',
    productName: ''
  },
  ConfigParams: {
    deviceInfo: {},
    sessionId: ''
  },
  onCreate: function onCreate() {
    console.info('AceApplication onCreate');
  },
  onDestroy: function onDestroy() {
    console.info('AceApplication onDestroy');
  }
};
exports["default"] = _default;
;
(exports["default"] || module.exports).manifest = __webpack_require__(/*! !./lib/manifest-plugin.js!../../../../build/intermediates/js/debug/jsManifest/default/manifest.json */ "./lib/manifest-plugin.js!d:\\Github\\HarmonyOS_Study\\knowledge_demo_smart_home-master\\FA\\DistSchedule\\netconfig\\build\\intermediates\\js\\debug\\jsManifest\\default\\manifest.json");

function requireModule(moduleName) {
  const systemList = ['system.router', 'system.app', 'system.prompt', 'system.configuration',
  'system.image', 'system.device', 'system.mediaquery', 'ohos.animator', 'system.grid', 'system.resource']
  var target = ''
  if (systemList.includes(moduleName.replace('@', ''))) {
    target = $app_require$('@app-module/' + moduleName.substring(1));
    return target;
  }
  var shortName = moduleName.replace(/@[^.]+.([^.]+)/, '$1');
  if (typeof ohosplugin !== 'undefined' && /@ohos/.test(moduleName)) {
    target = ohosplugin;
    for (let key of shortName.split('.')) {
      target = target[key];
      if(!target) {
        break;
      }
    }
    if (typeof target !== 'undefined') {
      return target;
    }
  }
  if (typeof systemplugin !== 'undefined') {
    target = systemplugin;
    for (let key of shortName.split('.')) {
      target = target[key];
      if(!target) {
        break;
      }
    }
    if (typeof target !== 'undefined') {
      return target;
    }
  }
  target = requireNapi(shortName);
  return target;
}
}
/* generated by ace-loader */


/***/ }),

/***/ "./node_modules/@babel/runtime/helpers/interopRequireDefault.js":
/*!**********************************************************************!*\
  !*** ./node_modules/@babel/runtime/helpers/interopRequireDefault.js ***!
  \**********************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";


function _interopRequireDefault(obj) {
  return obj && obj.__esModule ? obj : {
    "default": obj
  };
}

module.exports = _interopRequireDefault;

function requireModule(moduleName) {
  const systemList = ['system.router', 'system.app', 'system.prompt', 'system.configuration',
  'system.image', 'system.device', 'system.mediaquery', 'ohos.animator', 'system.grid', 'system.resource']
  var target = ''
  if (systemList.includes(moduleName.replace('@', ''))) {
    target = $app_require$('@app-module/' + moduleName.substring(1));
    return target;
  }
  var shortName = moduleName.replace(/@[^.]+.([^.]+)/, '$1');
  if (typeof ohosplugin !== 'undefined' && /@ohos/.test(moduleName)) {
    target = ohosplugin;
    for (let key of shortName.split('.')) {
      target = target[key];
      if(!target) {
        break;
      }
    }
    if (typeof target !== 'undefined') {
      return target;
    }
  }
  if (typeof systemplugin !== 'undefined') {
    target = systemplugin;
    for (let key of shortName.split('.')) {
      target = target[key];
      if(!target) {
        break;
      }
    }
    if (typeof target !== 'undefined') {
      return target;
    }
  }
  target = requireNapi(shortName);
  return target;
}


/***/ }),

/***/ "./node_modules/@babel/runtime/helpers/newArrowCheck.js":
/*!**************************************************************!*\
  !*** ./node_modules/@babel/runtime/helpers/newArrowCheck.js ***!
  \**************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";


function _newArrowCheck(innerThis, boundThis) {
  if (innerThis !== boundThis) {
    throw new TypeError("Cannot instantiate an arrow function");
  }
}

module.exports = _newArrowCheck;

function requireModule(moduleName) {
  const systemList = ['system.router', 'system.app', 'system.prompt', 'system.configuration',
  'system.image', 'system.device', 'system.mediaquery', 'ohos.animator', 'system.grid', 'system.resource']
  var target = ''
  if (systemList.includes(moduleName.replace('@', ''))) {
    target = $app_require$('@app-module/' + moduleName.substring(1));
    return target;
  }
  var shortName = moduleName.replace(/@[^.]+.([^.]+)/, '$1');
  if (typeof ohosplugin !== 'undefined' && /@ohos/.test(moduleName)) {
    target = ohosplugin;
    for (let key of shortName.split('.')) {
      target = target[key];
      if(!target) {
        break;
      }
    }
    if (typeof target !== 'undefined') {
      return target;
    }
  }
  if (typeof systemplugin !== 'undefined') {
    target = systemplugin;
    for (let key of shortName.split('.')) {
      target = target[key];
      if(!target) {
        break;
      }
    }
    if (typeof target !== 'undefined') {
      return target;
    }
  }
  target = requireNapi(shortName);
  return target;
}


/***/ }),

/***/ "d:\\Github\\HarmonyOS_Study\\knowledge_demo_smart_home-master\\FA\\DistSchedule\\netconfig\\src\\main\\js\\default\\app.js?entry":
/*!*****************************************************************************************************************************!*\
  !*** d:/Github/HarmonyOS_Study/knowledge_demo_smart_home-master/FA/DistSchedule/netconfig/src/main/js/default/app.js?entry ***!
  \*****************************************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

var $app_script$ = __webpack_require__(/*! !./lib/script.js!./node_modules/babel-loader?presets[]=C:/Users/lenovo/AppData/Local/Huawei/Sdk/js/2.2.0.3/build-tools/ace-loader/node_modules/@babel/preset-env&plugins[]=C:/Users/lenovo/AppData/Local/Huawei/Sdk/js/2.2.0.3/build-tools/ace-loader/node_modules/@babel/plugin-transform-modules-commonjs&comments=false!./lib/resource-reference-script.js!./lib/manifest-loader.js?path=d:/Github/HarmonyOS_Study/knowledge_demo_smart_home-master/FA/DistSchedule/netconfig/src/main/js/default/app.js!./app.js */ "./lib/script.js!./node_modules/babel-loader/lib/index.js?presets[]=C:\\Users\\lenovo\\AppData\\Local\\Huawei\\Sdk\\js\\2.2.0.3\\build-tools\\ace-loader\\node_modules\\@babel\\preset-env&plugins[]=C:\\Users\\lenovo\\AppData\\Local\\Huawei\\Sdk\\js\\2.2.0.3\\build-tools\\ace-loader\\node_modules\\@babel\\plugin-transform-modules-commonjs&comments=false!./lib/resource-reference-script.js!./lib/manifest-loader.js?path=d:\\Github\\HarmonyOS_Study\\knowledge_demo_smart_home-master\\FA\\DistSchedule\\netconfig\\src\\main\\js\\default\\app.js!d:\\Github\\HarmonyOS_Study\\knowledge_demo_smart_home-master\\FA\\DistSchedule\\netconfig\\src\\main\\js\\default\\app.js")

      $app_define$('@app-application/app', [], function($app_require$, $app_exports$, $app_module$) {
      
      $app_script$($app_module$, $app_exports$, $app_require$)
      if ($app_exports$.__esModule && $app_exports$.default) {
        $app_module$.exports = $app_exports$.default
      }
      
      })
      $app_bootstrap$('@app-application/app',undefined,undefined)

/***/ }),

/***/ "d:\\Github\\HarmonyOS_Study\\knowledge_demo_smart_home-master\\FA\\DistSchedule\\netconfig\\src\\main\\js\\default\\common\\fa-netconfig.js":
/*!***************************************************************************************************************************************!*\
  !*** d:/Github/HarmonyOS_Study/knowledge_demo_smart_home-master/FA/DistSchedule/netconfig/src/main/js/default/common/fa-netconfig.js ***!
  \***************************************************************************************************************************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

"use strict";


var _interopRequireDefault = __webpack_require__(/*! @babel/runtime/helpers/interopRequireDefault */ "./node_modules/@babel/runtime/helpers/interopRequireDefault.js");

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports["default"] = void 0;

var _newArrowCheck2 = _interopRequireDefault(__webpack_require__(/*! @babel/runtime/helpers/newArrowCheck */ "./node_modules/@babel/runtime/helpers/newArrowCheck.js"));

/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License,Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * NET_CONFIG JS API
 */
var CONSTANT = {
  BUNDLE_NAME: "com.huawei.ailifeability",
  ABILITY_NAME: "com.huawei.ailifeability.NetConfigAbility",
  IS_SUPPORT_WIFI_AWARE: 1000,
  DISCOVERY_BY_NAN_CODE: 1001,
  DISCOVERY_BY_SOFT_AP_CODE: 1002,
  CONNECT_DEVICE_CODE: 1003,
  GET_WIFI_LIST_CODE: 1004,
  SEND_MESSAGE_CODE: 1005,
  CONFIG_DEVICE_NET_CODE: 1006,
  DISCONNECT_DEVICE_CODE: 1007,
  SUBSCRIBE_EVENT_CODE: 1008,
  UNSUBSCRIBE_EVENT_CODE: 1009,
  REGISTER_MSG_RECEIVE: 1010,
  REGISTER_DISCONNECT_CALLBACK: 1011,
  SET_DEVICE_BIND_INFO: 1012
};
var EventObserver = {
  isInited: false,
  callbackMap: new Map(),
  register: function register(item) {
    EventObserver.callbackMap.set(item.messageCode, item.callback);
  },
  notify: function notify(result) {
    if (result.messageCode && EventObserver.callbackMap.get(result.messageCode) != undefined) {
      EventObserver.callbackMap.get(result.messageCode)(result);
    } else {
      console.error("NetConfig# notify failed, current messageCode not exist, messageCode=" + result.messageCode);
    }
  }
};

function getSubscribeParam() {
  var action = {};
  action.bundleName = CONSTANT.BUNDLE_NAME;
  action.abilityName = CONSTANT.ABILITY_NAME;
  action.messageCode = CONSTANT.SUBSCRIBE_EVENT_CODE;
  action.abilityType = 1;
  action.syncOption = 0;
  return action;
}

function unsubscribe() {
  var action = {};
  action.bundleName = CONSTANT.BUNDLE_NAME;
  action.abilityName = CONSTANT.ABILITY_NAME;
  action.messageCode = CONSTANT.UNSUBSCRIBE_EVENT_CODE;
  action.abilityType = 1;
  FeatureAbility.unsubscribeAbilityEvent(action).then(function (result) {
    var ret = JSON.parse(result);

    if (ret.code !== 0) {
      console.error("NetConfig# unsubscribe abilityEvent failed, error message: " + ret.data);
    }
  });
}

function callAbilityFunc(callCode, argsObj, callbackFunc) {
  console.info("NetConfig# callAbilityFunc arguments" + JSON.stringify(arguments));
  var action = {};
  action.bundleName = CONSTANT.BUNDLE_NAME;
  action.abilityName = CONSTANT.ABILITY_NAME;
  action.messageCode = callCode;
  action.abilityType = 1;
  action.data = argsObj;

  if (callbackFunc != undefined) {
    EventObserver.register({
      messageCode: callCode,
      callback: callbackFunc
    });
  }

  console.info("NetConfig# callAbilityFunc start.");
  return FeatureAbility.callAbility(action);
}

function callAbility(callCode, argsObj, callbackFunc) {
  var _this = this;

  console.info("NetConfig# callAbility arguments" + JSON.stringify(arguments));

  if (EventObserver.isInited) {
    return callAbilityFunc(callCode, argsObj, callbackFunc);
  } else {
    console.info("NetConfig# subscribe start.");
    return FeatureAbility.subscribeAbilityEvent(getSubscribeParam(), function (result) {
      (0, _newArrowCheck2["default"])(this, _this);
      var ret = JSON.parse(result);

      if (ret.data) {
        EventObserver.notify(ret.data);
      } else {
        console.error("NetConfig# subscribe callback JSON parse failed");
      }
    }.bind(this)).then(function (result) {
      (0, _newArrowCheck2["default"])(this, _this);
      console.info("NetConfigTime#FA subscribe end.");
      var ret = JSON.parse(result);

      if (ret.code !== 0) {
        console.error("NetConfig# subscribe abilityEvent failed, error message: " + ret.data);
      }

      EventObserver.isInited = true;
      return callAbilityFunc(callCode, argsObj, callbackFunc);
    }.bind(this));
  }
}

var FANetConfig = {
  isSupportWifiAware: function isSupportWifiAware() {
    return callAbility(CONSTANT.IS_SUPPORT_WIFI_AWARE, {});
  },
  discoveryByNAN: function discoveryByNAN(scanInfo, callbackFunc) {
    if (!(typeof scanInfo.duration === 'number' && scanInfo.duration % 1 === 0)) {
      console.error("NetConfig# call discoveryByNAN failed, parameter 'duration' type should be integer.");
      return;
    }

    if (!(typeof scanInfo.lockTime === 'number' && scanInfo.lockTime % 1 === 0)) {
      console.error("NetConfig# call discoveryByNAN failed, parameter 'lockTime' type should be integer.");
      return;
    }

    if (typeof callbackFunc !== 'function') {
      console.error("NetConfig# call discoveryByNAN failed, parameter 'callbackFunc' type should be function.");
      return;
    }

    return callAbility(CONSTANT.DISCOVERY_BY_NAN_CODE, {
      scanNanInfo: scanInfo
    }, callbackFunc);
  },
  discoveryBySoftAp: function discoveryBySoftAp(callbackFunc) {
    if (typeof callbackFunc !== 'function') {
      console.error("NetConfig# call discoveryBySoftAp failed, parameter 'callbackFunc' type should be function.");
      return;
    }

    return callAbility(CONSTANT.DISCOVERY_BY_SOFT_AP_CODE, {}, callbackFunc);
  },
  connectDevice: function connectDevice(connectInfo, callbackFunc) {
    var isConnectInfoNull = connectInfo == null || connectInfo == undefined;

    if (isConnectInfoNull) {
      console.error("NetConfig# call connectDevice failed, parameter 'connectInfo' should not be undefined.");
      return;
    }

    if (typeof connectInfo.targetDeviceId !== 'string') {
      console.error("NetConfig# call connectDevice failed, parameter 'connectInfo.targetDeviceId' type should be string.");
      return;
    }

    if (!(typeof connectInfo.type === 'number' && connectInfo.type % 1 === 0)) {
      console.error("NetConfig# call connectDevice failed, parameter 'connectInfo.type' type should be integer.");
      return;
    }

    if (typeof connectInfo.pin !== 'string') {
      console.error("NetConfig# call connectDevice failed, parameter 'connectInfo.pin' type should be string.");
      return;
    }

    if (typeof connectInfo.password !== 'string') {
      console.error("NetConfig# call connectDevice failed, parameter 'connectInfo.password' type should be string.");
      return;
    }

    if (typeof connectInfo.sessionId !== 'string') {
      console.error("NetConfig# call connectDevice failed, parameter 'connectInfo.sessionId' type should be string.");
      return;
    }

    if (typeof callbackFunc !== 'function') {
      console.error("NetConfig# call connectDevice failed, parameter 'callbackFunc' type should be function.");
      return;
    }

    return callAbility(CONSTANT.CONNECT_DEVICE_CODE, {
      connectInfo: connectInfo
    }, callbackFunc);
  },
  getWifiList: function getWifiList(callbackFunc) {
    if (typeof callbackFunc !== 'function') {
      console.error("NetConfig# call getWifiList failed, parameter 'callbackFunc' type should be function.");
      return;
    }

    return callAbility(CONSTANT.GET_WIFI_LIST_CODE, {}, callbackFunc);
  },
  sendMessage: function sendMessage(commonInfo, message, callbackFunc) {
    if (commonInfo == null || commonInfo == undefined) {
      console.error("NetConfig# call sendMessage failed, parameter 'commonInfo' type should be string.");
      return;
    }

    if (typeof commonInfo.sessionId !== 'string') {
      console.error("NetConfig# call sendMessage failed, parameter 'sessionId' type should be string.");
      return;
    }

    if (typeof message !== 'string') {
      console.error("NetConfig# call sendMessage failed, parameter 'message' type should be string.");
      return;
    }

    if (typeof callbackFunc !== 'function') {
      console.error("NetConfig# call sendMessage failed, parameter 'callbackFunc' type should be function.");
      return;
    }

    return callAbility(CONSTANT.SEND_MESSAGE_CODE, {
      commonInfo: commonInfo,
      message: message
    }, callbackFunc);
  },
  configDeviceNet: function configDeviceNet(deviceInfo, accountInfo, netConfigInfo, callbackFunc) {
    if (typeof deviceInfo !== 'string') {
      console.error("NetConfig# call configDeviceNet failed, parameter 'deviceInfo' type should be string.");
      return;
    }

    if (typeof accountInfo !== 'string') {
      console.error("NetConfig# call configDeviceNet failed, parameter 'accountInfo' type should be string.");
      return;
    }

    var isNetConfigInfoNull = netConfigInfo == null || netConfigInfo == undefined;

    if (isNetConfigInfoNull) {
      console.error("NetConfig# call configDeviceNet failed, parameter 'netConfigInfo' should not be undefined.");
      return;
    }

    if (typeof netConfigInfo.ssid !== 'string') {
      console.error("NetConfig# call configDeviceNet failed, parameter 'netConfigInfo.ssid' type should be string.");
      return;
    }

    if (typeof netConfigInfo.isDefaultPassword !== 'boolean') {
      console.error("NetConfig# call configDeviceNet failed, parameter 'netConfigInfo.isDefaultPassword' type should be boolean.");
      return;
    }

    if (!(typeof netConfigInfo.channel === 'number' && netConfigInfo.channel % 1 === 0)) {
      console.error("NetConfig# call configDeviceNet failed, parameter 'netConfigInfo.channel' type should be integer.");
      return;
    }

    if (typeof netConfigInfo.sessionId !== 'string') {
      console.error("NetConfig# call configDeviceNet failed, parameter 'netConfigInfo.sessionId' type should be string.");
    }

    if (!(typeof netConfigInfo.type === 'number' && netConfigInfo.type % 1 === 0)) {
      console.error("NetConfig# call configDeviceNet failed, parameter 'netConfigInfo.type' type should be integer.");
      return;
    }

    if (typeof callbackFunc !== 'function') {
      console.error("NetConfig# call configDeviceNet failed, parameter 'callbackFunc' type should be function.");
      return;
    }

    return callAbility(CONSTANT.CONFIG_DEVICE_NET_CODE, {
      deviceInfo: deviceInfo,
      accountInfo: accountInfo,
      netConfigInfo: netConfigInfo
    }, callbackFunc);
  },
  disconnectDevice: function disconnectDevice(commonInfo, callbackFunc) {
    if (commonInfo == null || commonInfo == undefined) {
      console.error("NetConfig# call disconnectDevice failed, parameter 'commonInfo' type should be string.");
      return;
    }

    if (typeof commonInfo.sessionId !== 'string') {
      console.error("NetConfig# call disconnectDevice failed, parameter 'sessionId' type should be string.");
      return;
    }

    if (typeof callbackFunc !== 'function') {
      console.error("NetConfig# call disconnectDevice failed, parameter 'callbackFunc' type should be function.");
      return;
    }

    return callAbility(CONSTANT.DISCONNECT_DEVICE_CODE, {
      commonInfo: commonInfo
    }, callbackFunc);
  },
  registerMsgReceive: function registerMsgReceive(commonInfo, callbackFunc) {
    if (commonInfo == null || commonInfo == undefined) {
      console.error("NetConfig# call registerMsgReceive failed, parameter 'commonInfo' type should be string.");
      return;
    }

    if (typeof commonInfo.sessionId !== 'string') {
      console.error("NetConfig# call registerMsgReceive failed, parameter 'sessionId' type should be string.");
      return;
    }

    if (typeof callbackFunc !== 'function') {
      console.error("NetConfig# call registerMsgReceive failed, parameter 'callbackFunc' type should be function.");
      return;
    }

    return callAbility(CONSTANT.REGISTER_MSG_RECEIVE, {
      commonInfo: commonInfo
    }, callbackFunc);
  },
  registerDisconnectCallback: function registerDisconnectCallback(commonInfo, callbackFunc) {
    if (commonInfo == null || commonInfo == undefined) {
      console.error("NetConfig# call registerDisconnectCallback failed, parameter 'commonInfo' type should be string.");
      return;
    }

    if (typeof commonInfo.sessionId !== 'string') {
      console.error("NetConfig# call registerDisconnectCallback failed, parameter 'sessionId' type should be string.");
      return;
    }

    if (typeof callbackFunc !== 'function') {
      console.error("NetConfig# call registerDisconnectCallback failed, parameter 'callbackFunc' type should be function.");
      return;
    }

    return callAbility(CONSTANT.REGISTER_DISCONNECT_CALLBACK, {
      commonInfo: commonInfo
    }, callbackFunc);
  },
  setDeviceBindInfo: function setDeviceBindInfo(bindInfo, callbackFunc) {
    if (bindInfo == null || bindInfo == undefined) {
      console.error("NetConfig# call setDeviceBinfInfo failed, parameter 'bindInfo' is null.");
      return;
    }

    if (typeof callbackFunc !== 'function') {
      console.error("NetConfig# call setDeviceBinfInfo failed, parameter 'callbackFunc' type should be function.");
      return;
    }

    return callAbility(CONSTANT.SET_DEVICE_BIND_INFO, {
      bindInfo: bindInfo
    }, callbackFunc);
  }
};
var _default = FANetConfig;
exports["default"] = _default;

function requireModule(moduleName) {
  const systemList = ['system.router', 'system.app', 'system.prompt', 'system.configuration',
  'system.image', 'system.device', 'system.mediaquery', 'ohos.animator', 'system.grid', 'system.resource']
  var target = ''
  if (systemList.includes(moduleName.replace('@', ''))) {
    target = $app_require$('@app-module/' + moduleName.substring(1));
    return target;
  }
  var shortName = moduleName.replace(/@[^.]+.([^.]+)/, '$1');
  if (typeof ohosplugin !== 'undefined' && /@ohos/.test(moduleName)) {
    target = ohosplugin;
    for (let key of shortName.split('.')) {
      target = target[key];
      if(!target) {
        break;
      }
    }
    if (typeof target !== 'undefined') {
      return target;
    }
  }
  if (typeof systemplugin !== 'undefined') {
    target = systemplugin;
    for (let key of shortName.split('.')) {
      target = target[key];
      if(!target) {
        break;
      }
    }
    if (typeof target !== 'undefined') {
      return target;
    }
  }
  target = requireNapi(shortName);
  return target;
}


/***/ })

/******/ });
//# sourceMappingURL=app.js.map