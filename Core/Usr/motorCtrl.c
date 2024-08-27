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

    if (start_time == 0) // ��ʼ����ʼʱ�䣬���ڵ�һ�ν���ʱ����
    {
        start_time = HAL_GetTick();
    }

    if (HAL_GetTick() - start_time <= START_TIME)
    {
        // �������ʱ�ĳ��ٶ�
        CarMotoCtrl(GOTO_LIMIT_SPEED(left, START_SPEED), GOTO_LIMIT_SPEED(right, START_SPEED));
    }
    else if (!is_moving)
    {
        // ��ʼ�����˶�
        CarMotoCtrl(left, right);
        is_moving = true;
    }

    // ��龭����ʱ���Ƿ�ﵽָ����ʱ��
    if (HAL_GetTick() - start_time >= time && is_moving)
    {
        // ֹͣ�����������״̬
        CarMotoCtrl(0, 0);
        is_moving = false;
        start_time = 0; // ���ÿ�ʼʱ��
    }
}

// ��ǰ�˶�����
void MotoGoStrightCtrl(uint32_t time)
{
    MotoCtrlTime(time, FORWARD_SPEED, FORWARD_SPEED);
}

// ����˶�����
void MotoGoBackCtrl(uint32_t time)
{
    MotoCtrlTime(time, -BACKWARD_SPEED, -BACKWARD_SPEED);
}

// ��ת�˶�����
void MotoTurnLeftCtrl(uint32_t time)
{
    MotoCtrlTime(time, -TURN_SPEED, TURN_SPEED);
}

// ��ת�˶�����
void MotoTurnRightCtrl(uint32_t time)
{
    MotoCtrlTime(time, TURN_SPEED, -TURN_SPEED);
}

// ɲ������
void MotoBreakCtrl(uint32_t time)
{
    MotoCtrlTime(time, 0, 0);
}

void MotoCtrlTimeBlocking(uint32_t time, int16_t left, int16_t right)
{
    uint32_t start_time = HAL_GetTick();

    // ��ʼ�������
    CarMotoCtrl(GOTO_LIMIT_SPEED(left, START_SPEED), GOTO_LIMIT_SPEED(right, START_SPEED));

    // �ȴ�����ʱ�����
    while (HAL_GetTick() - start_time <= START_TIME)
    {
        // ��ѭ�����ȴ������׶ν���
    }

    // �����������н׶�
    CarMotoCtrl(left, right);

    // ����ֱ��ָ��ʱ�����
    while (HAL_GetTick() - start_time < time)
    {
        // ��ѭ�����ȴ��˶�ʱ�����
    }

    // ʱ�䵽�ֹͣ���
    CarMotoCtrl(0, 0);
}

// ��ǰ�˶�����
void MotoGoStrightCtrlBlocking(uint32_t time)
{
    MotoCtrlTimeBlocking(time, FORWARD_SPEED, FORWARD_SPEED);
}

// ����˶�����
void MotoGoBackCtrlBlocking(uint32_t time)
{
    MotoCtrlTimeBlocking(time, -BACKWARD_SPEED, -BACKWARD_SPEED);
}

// ��ת�˶�����
void MotoTurnLeftCtrlBlocking(uint32_t time)
{
    MotoCtrlTimeBlocking(time, -TURN_SPEED, TURN_SPEED);
}

// ��ת�˶�����
void MotoTurnRightCtrlBlocking(uint32_t time)
{
    MotoCtrlTimeBlocking(time, TURN_SPEED, -TURN_SPEED);
}

// ɲ������
void MotoBreakCtrlBlocking(uint32_t time)
{
    MotoCtrlTimeBlocking(time, 0, 0);
}