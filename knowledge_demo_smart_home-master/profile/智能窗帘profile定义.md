# 数字管家自动窗帘设备profile定义说明

##### 下发命令定义

###### 窗帘开关(SetCurtainStatus)

```
{ 
	"service_id": "SmartCurtain",                  // 产品服务ID，固定为SmartCurtain
	"command_name": "SetCurtainStatus",       				// 命令名称，SetCurtainStatus
	"paras":
	{
   		"CurtainStatus": "OPEN"               			// 命令值（类型String）：OPEN --打开窗帘 / CLOSE --关闭窗帘
	}
}
```

###### 日程设置(SetSchedule)

```
{
	"service_id": "SmartCurtain",      // 产品服务ID，固定为SmartCurtain
	"command_name": "SetSchedule",     	// 命令名称，固定为SetSchedule
	"paras":
 	{
 		"ScheduleID":0			// 日程ID号(类型int)： 0 - 65535
 		"option":"A"			// 日程操作(类型string)：长度为1，暂指定3个操作：A(新增日程)，U(更新日程)，D(删除日程)
		"Day": "1,3,5",   		// 日程操作窗帘的日期，以星期为单位 
								// 取值(类型string): "1,2,3,4,5,6,7", 1-7代表周一到周日，每天之间用逗号(,)隔开，可以只设1天，也可以设置多天
		"StartHour": 18,     	// 操作窗帘的小时时间(类型int) : 0 - 23
		"StartMinute": 12,   	// 操作窗帘的分钟时间(类型int) : 0-59
		"Command": "OPEN" 		// 命令(类型string) : OPEN -- 打开窗帘  CLOSE -- 关闭窗帘
	}
}
```

##### 属性获取定义

属性是指设备当前的一些状态，由设备端主动上报到云端

```
{
	serviceId: SmartCurtain,      // 产品服务ID，固定为SmartCurtain
	data: 
	{
		"CurtainStatus":"ON"   		// 当前窗帘的状态(类型String)：OPEN -- 窗帘开  CLOSE -- 窗帘关
	}
}
```



##### 备注

自动窗帘设备其他相关信息

1. 热点名字(AP)：teamX-Curtain01

2. ProductID: 6136ceba0ad1ed02866fa3b2
3. NodeID: Curtain01
4. DeviceID:6136ceba0ad1ed02866fa3b2_Curtain01
5. Device PWD:12345678