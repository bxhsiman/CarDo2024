#include "motorCtrl.h"

static int16_t GOTO_LIMIT_SPEED(int16_t speed, int16_t limit)
{
    if (speed > 0)
        return limit;
    else if (speed < 0)
        return -limit;
    else
        return speed;
}

void MotoCtrlTime(uint32_t time, int16_t left, int16_t right)
{
    static uint32_t start_time = 0;
    static bool is_moving = false;

    if (start_time == 0) // 初始化开始时间，仅在第一次进入时设置
    {
        start_time = HAL_GetTick();
    }

    if (HAL_GetTick() - start_time <= START_TIME)
    {
        // 启动电机时的初速度
        CarMotoCtrl(GOTO_LIMIT_SPEED(left, START_SPEED), GOTO_LIMIT_SPEED(right, START_SPEED));
    }
    else if (!is_moving)
    {
        // 开始正常运动
        CarMotoCtrl(left, right);
        is_moving = true;
    }

    // 检查经过的时间是否达到指定的时间
    if (HAL_GetTick() - start_time >= time && is_moving)
    {
        // 停止电机，并重置状态
        CarMotoCtrl(0, 0);
        is_moving = false;
        start_time = 0; // 重置开始时间
    }
}

// 向前运动控制
void MotoGoStrightCtrl(uint32_t time)
{
    MotoCtrlTime(time, FORWARD_SPEED, FORWARD_SPEED);
}

// 向后运动控制
void MotoGoBackCtrl(uint32_t time)
{
    MotoCtrlTime(time, -BACKWARD_SPEED, -BACKWARD_SPEED);
}

// 左转运动控制
void MotoTurnLeftCtrl(uint32_t time)
{
    MotoCtrlTime(time, -TURN_SPEED, TURN_SPEED);
}

// 右转运动控制
void MotoTurnRightCtrl(uint32_t time)
{
    MotoCtrlTime(time, TURN_SPEED, -TURN_SPEED);
}

// 刹车控制
void MotoBreakCtrl(uint32_t time)
{
    MotoCtrlTime(time, 0, 0);
}

void MotoCtrlTimeBlocking(uint32_t time, int16_t left, int16_t right)
{
    uint32_t start_time = HAL_GetTick();

    // 初始启动电机
    CarMotoCtrl(GOTO_LIMIT_SPEED(left, START_SPEED), GOTO_LIMIT_SPEED(right, START_SPEED));

    // 等待启动时间结束
    while (HAL_GetTick() - start_time <= START_TIME)
    {
        // 空循环，等待启动阶段结束
    }

    // 进入正常运行阶段
    CarMotoCtrl(left, right);

    // 阻塞直到指定时间结束
    while (HAL_GetTick() - start_time < time)
    {
        // 空循环，等待运动时间结束
    }

    // 时间到达，停止电机
    CarMotoCtrl(0, 0);
}

// 向前运动控制
void MotoGoStrightCtrlBlocking(uint32_t time)
{
    MotoCtrlTimeBlocking(time, FORWARD_SPEED, FORWARD_SPEED);
}

// 向后运动控制
void MotoGoBackCtrlBlocking(uint32_t time)
{
    MotoCtrlTimeBlocking(time, -BACKWARD_SPEED, -BACKWARD_SPEED);
}

// 左转运动控制
void MotoTurnLeftCtrlBlocking(uint32_t time)
{
    MotoCtrlTimeBlocking(time, -TURN_SPEED, TURN_SPEED);
}

// 右转运动控制
void MotoTurnRightCtrlBlocking(uint32_t time)
{
    MotoCtrlTimeBlocking(time, TURN_SPEED, -TURN_SPEED);
}

// 刹车控制
void MotoBreakCtrlBlocking(uint32_t time)
{
    MotoCtrlTimeBlocking(time, 0, 0);
}