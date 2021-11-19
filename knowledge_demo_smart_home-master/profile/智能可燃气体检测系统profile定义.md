# 可燃气体检测profile定义说明

##### 下发命令定义


```
{ 
	"service_id": "GasDetection",                  // 产品服务ID，
	"command_name": "SetBeep",       			   // 命令名称，
	"paras":
	{
   		"BeepStatus": "OFF"               		   // 命令值（类型String）：ON --响铃 / OFF --关闭响铃
	}
}
```

##### 属性获取定义

属性是指设备当前的一些状态，由设备端主动上报到云端

```
{ 
	"service_id": "GasDetection",                  // 产品服务ID，
	"data":{
	    "BeepStatus":"ON",   // 当前状态，类型（string）,取值为“ON”（打开）、“OFF”（关闭）
   		"MQ5": 20,           // 当前可燃气体浓度值，类型（int）, 取值 0-65535
   		"Threshold":100,     // 当前TVOC甲醛报警门限，类型（float）, 取值0-65535
	 }
}
```



##### 备注

告警消息见：[告警定义文件](./alam_info.md)

可燃气体检测设备其他相关信息

1. 热点名字(AP)：teamX-GasDet01

2. ProductID:  61445c41f74139027d2233e5 
3. NodeID: GasDet01
4. DeviceID: 61445c41f74139027d2233e5_GasDet01
5. Device PWD:12345678

