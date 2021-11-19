# TVOC甲醛检测profile定义说明

##### 下发命令定义


```
{ 
	"service_id": "TVOCDetection",                  // 产品服务ID，
	"command_name": "SetBeep",       			   // 命令名称，
	"paras":
	{
   		"BeepStatus": "OFF"               		   // 命令值（类型String）：ON --响铃 / OFF --关闭响铃
	}
}
```

#### 设置甲醛气体报警门限：

```
 {
	"service_id":"TVOCDetection",          // 产品服务ID，固定
	"command_name":"SetThreshold",        // 设置亮度命令名字，固定
	"paras":{
		"Threshold":100,                  // 可燃气体报警门限，类型(float),取值0-65535
	}
}
```

##### 属性获取定义

属性是指设备当前的一些状态，由设备端主动上报到云端

```
{ 
	"service_id": "TVOCDetection",                  // 产品服务ID，
	"data":{
	    "BeepStatus":"ON",   // 当前状态，类型（string）,取值为“ON”（打开）、“OFF”（关闭）
   		"TVOC": 20,          // 当前TVOC甲醛气体浓度值，类型（float）, 取值 0-65535
		"Threshold":100,     // 当前TVOC甲醛报警门限，类型（float）, 取值0-65535
	 }
}
```



##### 备注

告警消息见：[告警定义文件](./alam_info.md)

TVOC甲醛检测设备其他相关信息

1. 热点名字(AP)：teamX-TVOCDet01

2. ProductID:   61445cdadcbb87027db095d1
3. NodeID:  TVOCDet01
4. DeviceID: 61445cdadcbb87027db095d1_TVOCDet01
5. Device PWD: 12345678