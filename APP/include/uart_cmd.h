/*
 * uart_cmd.h
 *
 *  Created on: Mar 21, 2025
 *      Author: Administrator
 */

#ifndef INCLUDE_UART_CMD_H_
#define INCLUDE_UART_CMD_H_

/**
 * @brief UART硬件初始化
 */
void app_uart_init(void);

/**
 * @brief UART任务初始化（TMOS任务注册）
 */
void uart_task_init(void);

#endif /* INCLUDE_UART_CMD_H_ */
