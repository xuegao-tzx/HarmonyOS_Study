/****************************************************************************

MIT License

Copyright (c) 2021 唐佐林
WeChat : delphi_tang
EMail: delphi_tang@dt4sw.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*****************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "utils_file.h"
#include "dtpython.h"

const char* iotsmoke_py = {
"print(\'Python iotsmoke Code Begin\')"                            "\r\n"    
"import iotsmoke"                                                  "\r\n" 
"import os"                                                        "\r\n"  
                                                                   "\r\n"
"iotsmoke.boardled_init()"                                           "\r\n"
"print(\'boardled_init end\')"                                      "\r\n" 
                                                                    "\r\n"
"iotsmoke.device_init()"                                             "\r\n"                 
                                                                     "\r\n"
"iotsmoke.network_config()"                                          "\r\n"
                                                                     "\r\n"
"iotsmoke.connect_iot_platform()"                                    "\r\n"
"os.sleep(1)"                                                     "\r\n"
"iotsmoke.sensor_task()"                                             "\r\n"
"print(\'Python iotsmoke Code end\')"                              "\r\n" 
"os.sleep(1)"                                                      "\r\n"
};

static int WriteScript(const char* name, const char* script)
{
    int fd = UtilsFileOpen(name, O_CREAT_FS | O_TRUNC_FS | O_WRONLY_FS, 0);

    if( fd > 0 )
    {
        int n = strlen(script);
        int i = 0;

        for(i=0; i<n; i++)
        {
            UtilsFileWrite(fd, script + i, 1);
        }

        UtilsFileClose(fd);
    }

    return fd;
}

static void PrepareScript(const char* name, const char* script)
{
    int fd = UtilsFileOpen(name, O_RDONLY_FS, 0);

    if( fd > 0 )
    {
        printf("\'%s\' is existed!\n", name);

        UtilsFileClose(fd);
    }
    else
    {
        printf("Create \'%s\'...\n", name);

        if( WriteScript(name, script) > 0 )
        {
            printf("\'%s\' is created!\n", name);
        }
    }
}

static void* Smoke_Python_Demo_Task(const char* arg)
{
    printf("Smoke_Python_Demo_Task()\n");

    PrepareScript("iotsmoke.py", iotsmoke_py);           // 在设备上创建文件：iotsmoke.py

    DTPython_Init();                                     // 初始化Python环境

    DTPython_RunCode("print(\'Python Code Begin\')");    // 执行Python语句：print('Python Code Begin')

    DTPython_RunFile("iotsmoke.py");                     // 执行Python文件：iotsmoke.py

    DTPython_RunCode("print(\'Python Code End\')");      // 执行Python语句：print('Python Code End')

    DTPython_Deinit();                                   // 清理Python环境

    return (void*)arg;
}

static void Smoke_Python_Demo_Entry(void)
{
    osThreadAttr_t attr = {0};

    printf("Smoke_Python_Demo_Entry()\n");

    attr.name = "Smoke_Python_Demo_Task";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024 * 30;
    attr.priority = osPriorityNormal;

    if (osThreadNew((osThreadFunc_t)Smoke_Python_Demo_Task, NULL, &attr) == NULL)
    {
        printf("Falied to create Smoke Python Demo Task!\n");
    }
}

SYS_RUN(Smoke_Python_Demo_Entry);
