# 智能垃圾桶设备profile定义说明

##### 属性获取定义

属性是指设备当前的一些状态，由设备端主动上报到云端

```
{ 
	"service_id": "Trashcan",                  // 产品服务ID，
	"data":{
   		"CapacityUsed": 20,          // 当前垃圾桶已用容量百分比，类型（int）, 取值 0-100
	 }
}
```



##### 备注

告警消息见：[告警定义文件](./alam_info.md)

智能垃圾桶设备其他相关信息

1. 热点名字(AP)：teamX-Trashcan01

2. ProductID:   614456cff74139027d2230ec 
3. NodeID:  Trashcan01
4. DeviceID:  614456cff74139027d2230ec_Trashcan01 
5. Device PWD: 12345678