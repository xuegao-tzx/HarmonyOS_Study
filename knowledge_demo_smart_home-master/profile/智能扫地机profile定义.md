# 数字管家智能扫地机设备profile定义说明

##### 下发命令定义


```
{ 
	"service_id": "CleanData",                  // 产品服务ID，
	"command_name": "CleanCmd",       			// 命令名称，
	"paras":
	{
   		"GoTOClean": "ON"               			// 命令值（类型String）：ON --清扫 / OFF --暂停
	}
}
```

###### 日程设置(SetSchedule)

```
{
	"service_id": "CleanData",      // 产品服务ID，固定为CleanData
	"command_name": "SetSchedule",     	// 命令名称，固定为SetSchedule
	"paras":
 	{
 		"ScheduleID":0			// 日程ID号(类型int)： 0 - 65535
 		"Option":"A"			// 日程操作(类型string)：长度为1，暂指定3个操作：A(新增日程)，U(更新日程)，D(删除日程)
		"Day": "1,3,5",   		// 日程操作扫地机的日期，以星期为单位 
								// 取值(类型string): "1,2,3,4,5,6,7", 1-7代表周一到周日，每天之间用逗号(,)隔开，可以只设1天，也可以设置多天
		"StartHour": 18,     	// 操作扫地机的小时时间(类型int) : 0 - 23
		"StartMinute": 12,   	// 操作扫地机的分钟时间(类型int) : 0-59
		"DurationMinute": 1 	// 持续清扫时间(类型int) : 0 - 1440
		"Command": {"GoTOClean":"ON"} 		// 命令(类型string) : ON -- 启动清扫  OFF --停止清扫
	}
}
```

##### 属性获取定义

属性是指设备当前的一些状态，由设备端主动上报到云端

```
{
	"service_id":"CleanData",   // 产品服务ID，固定
	"data":{
		"CleanerStatus":"ON",         // 当前状态，类型（string）,取值为“ON”（清扫）、“OFF”（停止）
	}

}
```



##### 备注

智能扫地机设备其他相关信息

1. 热点名字(AP)：teamX-CleanRobot

2. ProductID: 61371e952cce4f0286262229
3. NodeID: CleanRobot
4. DeviceID:61371e952cce4f0286262229_CleanRobot
5. Device PWD:12345678