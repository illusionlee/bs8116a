# bs8116a

## 1、介绍

**bs8116a** 是合泰的bs8116a-3的触摸按键芯片的中断使用软件包。

### 1.1 目录结构

| 名称 | 说明 |
| ---- | ---- |
| examples | 例子目录 |
| inc  | 头文件目录 |
| src  | 源代码目录 |

### 1.2 许可证

`bs8116a` 软件包使用 `MIT` 软件包许可协议，请见 `bs8116a/LICENSE` 文件。

### 1.3 依赖

- RT-Thread 3.0+

## 2、如何打开 bs8116a

使用 bs8116a package 需要在 RT-Thread 的包管理器中选择它，具体路径如下：

```shell
RT-Thread online packages
    tools packages --->
        [*] bs8116a: Touch key of HOLTEK BS8116A-3.
```

然后让 RT-Thread 的包管理器自动更新，或者使用 `pkgs --update` 命令更新包到 BSP 中。

## 3、使用 bs8116a

### 初始化键值对应表

定义一个数组，按照实际按键键值进行定义，如：

```c
/* 芯片1对应的键值 */
static rt_uint8_t keyvalue1[15] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
```

### 芯片初始化

芯片可以进行参数配置，所以需要对参数进行定义：

```c
/* 按键的初始化数组 */
static rt_uint8_t touch_key_init_buff[22] = {
    0x00,                           /* 触发模式 */
    0x00, 0x83, 0xF3,               /* 固定协议 */
    0x98,                           /* 省电模式 */
    0x20, 0x20, 0x20, 0x20, 0x20,   /* 触摸阈值 */
    0x20, 0x20, 0x20, 0x20, 0x20,   /* 触摸阈值 */
    0x20, 0x20, 0x20, 0x20, 0x20,   /* 触摸阈值 */
    0x40,                           /* 中断模式 */
    0x2E                            /* SUM校验和 */
};
```

具体设置，请参考官方数据手册。

### 芯片数量定义

```c
/* 使用BS8116A芯片的数量 */
rt_uint8_t BS8116A_CHIP_NUMS = 3;
```

### 实现按键回调函数：

```c
/* 按键触发调用函数 */
void got_key_callback(rt_uint8_t key_code) {
    rt_kprintf("got_key:%d\n", key_code);
}
```

## 4、示例演示

在 MSH 中输入命令 `bs8116a_test_func`，点击BS8116A-3的触摸按键，可以在串口助手上看到输出了对应的键值。

```
msh >bs8116a_test_func
BS8116A_CHIP_NUMS : 3
Find i2c1soft device OK!
Find i2c2soft device OK!
Find i2c3soft device OK!
msh >
msh >got_key:1
got_key:8
got_key:15
got_key:22
got_key:31
```

## 5、注意事项

- 需要menuconfig 中配置I2C使能，且按照实际i2c数量进行配置。
- 必须实现 `got_key_callback`方法
- 必须定义`BS8116A_CHIP_NUMS`
- 键值对应关系根据实际PCB布局进行定义

## 6、联系方式

* 维护：[illusionlee](https://github.com/illusionlee)
* 主页：https://github.com/illusionlee/bs8116a.git
