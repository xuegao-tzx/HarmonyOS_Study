### 智能风扇  Profile定义

#### 手动开关风扇：

```
 {
	"service_id":"SmartFan",       // 产品服务ID，固定
	"command_name":"SetFanStatus", // 开关命令名字，固定
	"paras":{
		"FanStatus":"ON",           // 命令参数,控制风扇开关,类型(string)，选项为ON（打开）/OFF(关闭)
		"SpeedLevel":1,              // 命令参数,风扇挡位(int 类型)， 1 - 4   1：风速最小
	}
}
```
#### 手动调整风扇风速：

```
 {
	"service_id":"SmartFan",         // 产品服务ID，固定
	"command_name":"SetSpeedLevel",  // 设置风扇风速命令名字，固定
	"paras":{
		"SpeedLevel":1,              // 命令参数,风扇挡位(int 类型)，1 - 4   1：风速最小
	}
}
```
####  日程预约模式：

```
{
	"service_id":"SmartFan",    // 产品服务ID，固定
	"command_name":"SetSchedule",   // 日程预约命令名字，固定
    "paras":{
  	"sheduleID":0,    // 日程ID号（int）,取值为0-65535， 用于服务端对设备日程的增、删、改
  	"option":"A",     // 日程操作类型（string）,取值为“A”（增加）、“D”（删除）、“U”（更新）
  	“Day”:"1,3,5",    // 日程的天（星期几）类型（string）,已逗号区分的数字，“1,2,3,4,5,6,7”,或者下划线连接的数字，比方“1-7”标识周一到周日
  	“StartHour”:18,   // 日程的起始时辰，类型（int）,取值为0-23
  	“StartMinute”:30, // 日程的起始分，类型（int), 取值为0-59
  	"DurationMinutes":1， 	// 亮灯持续时间(类型int) : 0 - 1440
  	"Command":{"FanStatus":"ON","SpeedLevel":1}, // 日程的预约动作，类型（string）,取值为“ON”（打开）、“OFF”(关闭),如果打开风速，需要携带风扇的风速
  }
}
```

####  上报状态：

```
{
	"service_id":"SmartFan",   // 产品服务ID，固定
	"data":{
		"FanStatus":"ON",        // 当前状态，类型string, 取值为“ON”(打开)、“OFF”(关闭)
		"SpeedLevel": 1,          // 当前设置风速风速(int 类型), 取值1-4
	}
}
```

####  产品的其他信息定义

+ 当前产品的ProductID固定为： 6150601d88056b027dd2ca47 
+ 产品进入配网状态的AP热点名字定义：teamX+"-"+"NodeID"模式，比方有个设备的NodeID为Fan01,则对应的配网（softap）模式下的ssid为teamX-Fan01
+ 设备ID的构成方式为，ProductID+"_"+"NodeID"模式，比方有个设备的NodeID为Fan01,则对应的deviceID为6150601d88056b027dd2ca47_Fan01
+ 和设备关联的ProductID/NodeID/SSID/SSID_PWD/配网方式等信息可以从NFC卡片中获取