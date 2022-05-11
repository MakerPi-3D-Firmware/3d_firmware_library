#ifndef IRQ_INTERFACE_H
#define IRQ_INTERFACE_H

extern void TIM3_IRQHandler_process(void); ///< 定时器3，温度检测执行
extern void TIM4_IRQHandler_process(void); ///< 定时器4，电机执行入口

#endif // IRQ_INTERFACE_H













