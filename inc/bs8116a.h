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

#ifndef __BS8116A_H_
#define __BS8116A_H_

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* 使用的BS8116A芯片数量 需要使用者进行定义和赋值 */
extern rt_uint8_t BS8116A_CHIP_NUMS;

/* 按键参数结构体 */
struct bs8116a_key_arg {
	rt_uint8_t *key_value;          /* 键值对应关系数组 */
	rt_uint8_t key_size;            /* 实际使用的按键个数 */
	rt_uint8_t *init_buff;          /* 初始化芯片的参数数组 */
};

/* 中断配置结构体 */
struct bs8116a_irq_arg {
	rt_uint8_t irq_pin_id;          /* 中断引脚编号 */
	struct rt_i2c_bus_device *bus;  /* i2c设备结构体 */
	struct bs8116a_key_arg *key;    /* 按键参数结构体 */
	char bus_name[16];              /* 使用的i2c设备名 */
};

/* 申请i2c总线函数 */
extern void keypad_init(struct bs8116a_irq_arg arg[]);

/* 对BS8116A芯片进行初始化配置，需要在上电后的8秒内调用 */
extern void init_buf_to_bus(struct bs8116a_irq_arg arg[]);

/* 注册中断引脚 绑定中断函数 */
extern void key_irq_entry(struct bs8116a_irq_arg arg[]);

/* 回调用户定义的函数 需要使用者实现方法体 */
extern void got_key_callback(rt_uint8_t key_code);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* __BS8116A_H_ */
