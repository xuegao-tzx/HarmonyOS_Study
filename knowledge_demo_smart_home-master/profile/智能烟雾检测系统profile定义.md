### 烟雾传感器Profile定义

#### 手动开关烟雾报警：

```
 {
	"service_id":"SmartSmokeSensation",       // 产品服务ID，固定
	"command_name":"SetBeep", // 开关命令名字，固定
	"paras":{
		"BeepStatus":"ON",    // 命令参数,控制开关,类型(string)，选项为ON（打开）/OFF(关闭)
	}
}
```
#### 设置烟雾报警门限：

```
 {
	"service_id":"SmartSmokeSensation",          // 产品服务ID，固定
	"command_name":"SetThreshold", // 设置亮度命令名字，固定
	"paras":{
		"Threshold":100,                  // 烟雾报警门限，类型(float),取值0-65535
	}
}
```
####  上报数据：

```
{
	"service_id":"SmartSmokeSensation",   // 产品服务ID，固定
	"data":{
		"BeepStatus":"ON",   // 当前状态，类型（string）,取值为“ON”（打开）、“OFF”（关闭）
		"MQ2": 20,           // 当前烟雾检测数值，类型（float）, 取值0-65535
		"Threshold":100,     // 当前烟雾报警门限，类型（float）, 取值0-65535
	}
}
```

####  产品的其他信息定义

告警消息见：[告警定义文件](./alam_info.md)

+ 热点名字(AP)：teamX-Smoke01
+ ProductID: 6128bdd12cce4f02861e6d98
+ NodeID: smartsmokesensation01
+ DeviceID:6128bdd12cce4f02861e6d98_smartsmokesensation01
+ Device PWD:12345678