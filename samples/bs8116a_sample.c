#include <rtthread.h>

#if defined(RT_USING_FINSH) && defined(FINSH_USING_MSH)
#include <finsh.h>
#include "bs8116a.h"

/* 芯片1对应的键值 */
static rt_uint8_t keyvalue1[15] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
/* 芯片2对应的键值 */
static rt_uint8_t keyvalue2[15] = {16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30};
/* 芯片3对应的键值 */
static rt_uint8_t keyvalue3[6]  = {31, 32, 33, 34, 35, 36};
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
/* 使用BS8116A芯片的数量 */
rt_uint8_t BS8116A_CHIP_NUMS = 3;

/* 按键触发调用函数 */
void got_key_callback(rt_uint8_t key_code) {
    rt_kprintf("got_key:%d\n", key_code);
}

static void bs8116a_test_func(void) {
    rt_kprintf("BS8116A_CHIP_NUMS : %d\n", BS8116A_CHIP_NUMS);

    /* 需要与 BS8116A_CHIP_NUMS 数量对应*/
    static struct bs8116a_irq_arg b_i_a[3];

    /* 芯片1的参数配置*/
    static struct bs8116a_key_arg b_k_a_1 = {
        .key_value = keyvalue1,
        .key_size = 15,
        .init_buff = touch_key_init_buff,
    };
    b_i_a[0].key = &b_k_a_1;
    b_i_a[0].irq_pin_id = 29;
    strcpy(b_i_a[0].bus_name, "i2c1soft");

    /* 芯片2的参数配置*/
    static struct bs8116a_key_arg b_k_a_2 = {
        .key_value = keyvalue2,
        .key_size = 15,
        .init_buff = touch_key_init_buff,
    };
    b_i_a[1].key = &b_k_a_2;
    b_i_a[1].irq_pin_id = 28;
    strcpy(b_i_a[1].bus_name, "i2c2soft");

    /* 芯片3的参数配置*/
    static struct bs8116a_key_arg b_k_a_3 = {
        .key_value = keyvalue3,
        .key_size = 6,
        .init_buff = touch_key_init_buff,
    };
    b_i_a[2].key = &b_k_a_3;
    b_i_a[2].irq_pin_id = 27;
    strcpy(b_i_a[2].bus_name, "i2c3soft");

    /* 申请i2c总线函数 */
    keypad_init(b_i_a);
    /* 对BS8116A芯片进行初始化配置，需要在上电后的8秒内调用 */
    init_buf_to_bus(b_i_a);
    /* 注册中断引脚 绑定中断函数 */
    key_irq_entry(b_i_a);
}
MSH_CMD_EXPORT(bs8116a_test_func, bs8116a test function.);
#endif /* defined(RT_USING_FINSH) && defined(FINSH_USING_MSH) */