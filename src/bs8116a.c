/**
 * The MIT License
 *
 * Copyright (c) 2019 Illusion Lee
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <rtthread.h>
#include <rtdevice.h>
#include "bs8116a.h"

/* BS8116A 触摸引脚数量 */
#define KEYBOARD_SIZE             16
/* BS8116A 获取键值地址 */
static rt_uint8_t GET_DATA_ADDR = 0x08;
/* BS8116A 参数配置地址 */
static rt_uint8_t SET_ATTR_ADDR = 0xB0;
/* BS8116A总线地址 */
static rt_uint8_t BS8116A_ADDR  = 0x50;
/* BS8116A 初始化数组的大小 */
static rt_uint8_t INIT_BUF_SIZE = 0x16;
/* BS8116A引脚对应keycode */
static rt_uint16_t key_array[KEYBOARD_SIZE] = {
    0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080,
    0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000, 0x8000
};

/* 写BS8116A寄存器 */
static rt_err_t write_reg(struct rt_i2c_bus_device *bus, rt_uint8_t reg, rt_uint8_t *data, rt_uint8_t len) {
    rt_uint8_t buf[len + 1];
    rt_uint8_t i;
    struct rt_i2c_msg msgs;

    buf[0] = reg;
    for (i = 0; i < len; i++) {
        buf[i + 1] = data[i];
    }
    msgs.addr  = BS8116A_ADDR;
    msgs.flags = 0;
    msgs.buf   = buf;
    msgs.len   = len + 1;

    /* 调用I2C设备接口传输数据 */
    if (rt_i2c_transfer(bus, &msgs, 1) == 1) {
        return RT_EOK;
    } else {
        rt_kprintf("write_reg rt_i2c_transfer RT_ERROR[%d]\n", RT_ERROR);
        return -RT_ERROR;
    }
}

/* 对BS8116A芯片进行初始化配置，需要在上电后的8秒内调用 */
void init_buf_to_bus(struct bs8116a_irq_arg irq_arg[]) {
    int i_num = 0;
    int ret = RT_EOK;
    for (i_num = 0; i_num < BS8116A_CHIP_NUMS; ++i_num) {
        ret = write_reg(irq_arg[i_num].bus, SET_ATTR_ADDR, irq_arg[i_num].key->init_buff, INIT_BUF_SIZE);
        if (ret != RT_EOK) {
            rt_kprintf("init_buf_to_bus %s RT_ERROR[%d]\n", irq_arg[i_num].bus_name, RT_ERROR);
        }
    }
}

/* 使用i2c获取触摸按键的key */
static rt_err_t get_key(struct rt_i2c_bus_device *bus, rt_uint8_t *reg, rt_uint8_t *value) {
    struct rt_i2c_msg msgs[2];
    msgs[0].addr  = BS8116A_ADDR;
    msgs[0].flags = 0;
    msgs[0].len   = 1;
    msgs[0].buf   = reg;

    msgs[1].addr  = BS8116A_ADDR;
    msgs[1].flags = RT_I2C_RD;
    msgs[1].buf   = value;
    msgs[1].len   = 2;

    if (rt_i2c_transfer(bus, msgs, 2) == 2) {
        return RT_EOK;
    } else {
        rt_kprintf("get_key rt_i2c_transfer RT_ERROR[%d]\n", RT_ERROR);
        return -RT_ERROR;
    }
}

/* 将BS8116A获取到的 touch_code 转换为用户定义的键值函数 */
static rt_uint8_t get_key_code(struct bs8116a_key_arg *key, rt_uint16_t touch_code) {
    rt_uint8_t loop_key_i = 0;
    for (loop_key_i = 0; loop_key_i < key->key_size; loop_key_i++) {
        if (key_array[loop_key_i] == touch_code) {
            return key->key_value[loop_key_i];
        }
    }
    return 0xFF;
}

/* 申请i2c总线函数 */
void keypad_init(struct bs8116a_irq_arg irq_arg[]) {
    int i_num = 0;
    for (i_num = 0; i_num < BS8116A_CHIP_NUMS; ++i_num) {
        irq_arg[i_num].bus = (struct rt_i2c_bus_device *)rt_device_find(irq_arg[i_num].bus_name);

        if (irq_arg[i_num].bus == RT_NULL) {
            rt_kprintf("Can't find %s device!\n", irq_arg[i_num].bus_name);
        } else {
            rt_kprintf("Find %s device OK!\n", irq_arg[i_num].bus_name);
        }
    }
}

/* 中断处理函数 */
void *get_irq_key_entry(void *arg) {
    struct bs8116a_irq_arg *irq_arg = (struct bs8116a_irq_arg *)arg;
    rt_uint16_t KeyCode     = 0;
    rt_uint8_t touched_code = 0;
    rt_uint8_t key_value[2] = {0, 0};

    get_key(irq_arg->bus, &GET_DATA_ADDR, key_value);
    KeyCode = key_value[0] + (key_value[1] << 8);

    if (KeyCode) {
        touched_code = get_key_code(irq_arg->key, KeyCode);
        if (touched_code != 0xFF) {
            /* 回调用户定义的函数 */
            got_key_callback(touched_code);
        }
    }
}

/* 注册中断引脚 绑定中断函数 */
void key_irq_entry(struct bs8116a_irq_arg irq_arg[]) {
    rt_uint8_t i_num = 0;
    for (i_num = 0; i_num < BS8116A_CHIP_NUMS; ++i_num) {
        if (irq_arg[i_num].bus != RT_NULL) {
            rt_pin_mode(irq_arg[0].irq_pin_id, PIN_MODE_INPUT_PULLUP);
            rt_pin_attach_irq(irq_arg[i_num].irq_pin_id, PIN_IRQ_MODE_FALLING, get_irq_key_entry, (void *)&irq_arg[i_num]);
            rt_pin_irq_enable(irq_arg[i_num].irq_pin_id, PIN_IRQ_ENABLE);
        } else {
            rt_kprintf("%s device isn't Initialize!\n", irq_arg[i_num].bus_name);
        }
    }
}
