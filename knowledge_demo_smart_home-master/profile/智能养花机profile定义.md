# 数字管家自动浇花设备profile定义说明

##### 下发命令定义

###### 开关浇水（SetMotor）

```
{ 
	"service_id": "SmartWatering",                  // 产品服务ID，固定为SmartWatering
	"command_name": "SetMotor",       				// 命令名称，固定为SetMotor
	"paras":
	{
   		"MotorStatus": "ON"               			// 命令值（类型String）：ON/OFF
   		"Capacity": 200 							// 浇水的容量(类型int) : 1 - 500, 一次浇水不能超过500ml
	}
}
```

###### 日程设置(SetSchedule)

```
{
	"service_id": "SmartWatering",      // 产品服务ID，固定为SmartWatering
	"command_name": "SetSchedule",     	// 命令名称，固定为SetSchedule
	"paras":
 	{
 		"ScheduleID":0			// 日程ID号(类型int)： 0 - 65535
 		"option":"A"			// 日程操作(类型string)：长度为1，暂指定3个操作：A(新增日程)，U(更新日程)，D(删除日程)
		"Day": "1,3,5",   		// 开始浇水的日期，以星期为单位 
								// 取值(类型string): "1,2,3,4,5,6,7", 1-7代表周一到周日，每天之间用逗号(,)隔开，可以只设1天，也可以设置多天
		"StartHour": 18,     	// 开始浇水的小时时间(类型int) : 0 - 23
		"StartMinute": 12,   	// 开始浇水的分钟时间(类型int) : 0-59
		"Command":"{"MotorStatus":"ON", "Capacity": 200}"	// 日程到点后需要执行的命令。 MotorStatus:打开/关闭浇水功能,Capacity:浇水容量
	}
}
```

##### 属性获取定义

属性是指设备当前的一些状态，由设备端主动上报到云端

```
{
	serviceId: SmartWatering,      // 产品服务ID，固定为SmartWatering
	data: 
	{
		"MotorStatus":"ON",   		// 当前浇水状态，是否在浇水(类型String)：ON/OFF
		"Temperature":38,			// 当前空气的温度(类型int) 0 - 65535
		"Humidity":58,         		// 当前空气的湿度(类型int)：0 - 65535
		"Moisture":6    			// 当前土壤的水分(类型int)：0 - 65535
	}
}
```



##### 备注

自动浇花设备其他相关信息

1. 热点名字(AP)：teamX-Watering01

2. ProductID: 6135e8fc2cce4f028625ccc9
3. NodeID: Watering01
4. DeviceID:6135e8fc2cce4f028625ccc9_Watering01
5. Device PWD:12345678