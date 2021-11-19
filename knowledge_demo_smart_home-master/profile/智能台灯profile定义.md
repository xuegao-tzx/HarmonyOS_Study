# 数字管家台灯profile定义说明

##### 命令下发定义

###### 开关灯(SetLampStatus)

```
{ 
	"service_id": "SmartLamp",                  // 产品服务ID，固定为SmartLamp
	"command_name": "SetLampStatus",       		// 命令名称，固定为SetLampStatus
	"paras":
	{
   		"LampStatus": "ON"               		// 命令值（类型String）：ON/OFF
   		"DutyCycle":99							// 灯亮度(类型int):1-99
	}
}
```

###### 亮度设置(SetLampDutyCycle)

```
{ 
	"service_id": "SmartLamp",            // 产品服务ID，固定为SmartLamp
	"command_name": "SetLampDutyCycle",   // 命令名称，固定为SetLampDutyCycle
	"paras": 
	{
		"DutyCycle": 99               	  // 命令值：1-99
	} 
} 
```

###### 日程设置(SetSchedule)

```
{ 
	"service_id": "SmartLamp",            // 产品服务ID，固定为SmartLamp
	"command_name": "SetLampShedule",     // 命令名称，固定为SetLampShedule
	"paras":
 	{
 		"ScheduleID":0			// 日程ID号(类型int)： 0 - 65535
 		"option":"A"			// 日程操作(类型string)：长度为1，暂指定3个操作：A(新增日程)，U(更新日程)，D(删除日程)
		"Day": "1,3,5",   		// 开始亮灯的日期，以星期为单位 
								// 取值(类型string): "1,2,3,4,5,6,7", 1-7代表周一到周日，每天之间用逗号(,)隔开，可以只设1天，也可以设置多天
		"StartHour": 18,     	// 开始亮灯的小时时间(类型int) : 0 - 23
		"StartMinute": 12,   	// 开始亮灯的分钟时间(类型int) : 0-59
		"DurationMinutes": 1 	// 亮灯持续时间(类型int) : 0 - 1440
		"Command":"{"LampStatus":"ON","DutyCycle":99}"	// 日程到点后需要执行的命令。
														// (LampStatus：灯的开关状态，DutyCycle 灯的亮度,取值参照SetLampStatus)
	}
}
```

##### 属性获取定义

属性是指设备当前的一些状态，由设备端主动上报到云端

```
{
	serviceId: SmartLamp,      // 产品服务ID，固定为SmartLamp
	data: 
	{
		"LampStatus":"ON",   	// 台灯当前开关状态(类型String)：ON/OFF
		"DutyCycle":99,         // 台灯当前亮度(类型int)：0 - 65535
		"LightIntensity":341    // 台灯当前光强度(类型int)：0 - 65535
	}
}
```

