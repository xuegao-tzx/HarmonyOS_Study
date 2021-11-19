### 智能手表Profile定义

#### 添加电话号码：

```
{
    "service_id":"SmartWatch",// 产品服务ID，固定
    "command_name":"AddPhoneNumber",// 设置命令，固定,添加电话号码
    "paras":{
    	"name":"father",// 命令参数(string)，选项为联系人名称，固定长度24
        "PhoneNumber":"18688888888"    // 命令参数(string)，选项为电话号码，固定长度11
    }
}
```
####  删除电话号码：

```
{
    "service_id":"SmartWatch",    // 产品服务ID，固定
    "command_name":"DeletePhoneNumber",   // 拨打号码命令名字，固定
    "paras":{
    	"name":"father",//命令参数(string)，选项为联系人名称，固定长度24
    }
}
```

####  日程预约提醒模式：

```
{
    "service_id":"SmartWatch",// 产品服务ID，固定
    "command_name":"SetSchedule",// 日程预约命令名字，固定
    "paras":{
        "ScheduleID":"0", // 日程ID号（int）,取值为0-65535， 用于服务端对设备日程的增、删、改
        "option":"A", // 日程操作类型（string）,取值为“A”（增加）、“D”（删除）、“U”（更新）
        "Day":"1,2,3,4,5,6,7",// 日程的天（星期几）类型（string）,已逗号区分的数字，“1,2,3,4,5,6,7”,或者下划线连接的数字，比方“1-7”标识周一到周日
        "StartHour":11, // 日程的起始时辰，类型（int）,取值为0-23
        "StartMinute":59, // 日程的起始分，类型（int), 取值为0-59
         "Command":{"name":"喝水"},// 日程的预约提醒内容，类型（string-UTF8格式中文）,例如：起床，吃饭，睡觉，吃药，喝水，写作业
        "DurationMinutes":5// 日程的预约提醒持续时间，类型（int）
    }
}
```

上报GPS地位位置和电话簿联系人信息：

```
{
    "service_id":"SmartWatch",// 产品服务ID，固定
    "data":{
        "UTCdatetime":"20210916072022.000",//UTC时间，获取到的定位信息的时间
        "logitude":"114.062765",//经度，类型（string）,没有则上报为""
        "latitude":"22.661522",//纬度，类型（string）,没有则上报为""
        "satellitesinview":"4"//卫星可见颗数，类型（int),没有则上报为""
        "father":"18686666666",// 命令参数(string)，选项为电话簿联系人，固定长度11，若未设置，上报"";
    	"mother":"18688888888"// 命令参数(string)，选项为电话簿联系人，固定长度11，若未设置，上报"";
    }
}
```

####  产品的其他信息定义

+ 当前产品的ProductID固定为：613ad28b0109930287315230
+ 产品进入配网状态的AP热点名字定义：teamX+"-"+"NodeID"模式，比方有个设备的NodeID为SmartWatch01,则对应的配网（softap）模式下的ssid为teamX-SmartWatch01
+ 设备ID的构成方式为，ProductID+"_"+"NodeID"模式，比方有个设备的NodeID为SmartWatch01,则对应的deviceID为613ad28b0109930287315230_SmartWatch01
+ 和设备关联的ProductID/NodeID/SSID/SSID_PWD/配网方式等信息可以从NFC卡片中获取

  

