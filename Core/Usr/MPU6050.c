#include <math.h>
#include "mpu6050.h"
#include "tim.h"
#include "soft_IIC.h"

#define RAD_TO_DEG 57.295779513082320876798154814105

#define WHO_AM_I_REG 0x75
#define PWR_MGMT_1_REG 0x6B
#define SMPLRT_DIV_REG 0x19
#define ACCEL_CONFIG_REG 0x1C
#define ACCEL_XOUT_H_REG 0x3B
#define TEMP_OUT_H_REG 0x41
#define GYRO_CONFIG_REG 0x1B
#define GYRO_XOUT_H_REG 0x43

// Setup MPU6050
#define MPU6050_ADDR 0xD0
const uint16_t i2c_timeout = 100;
const double Accel_Z_corrector = 14418.0;

float gyroZ_offset = 0;
float g_yaw = 0;
double gyroZ_sum = 0;
uint32_t timer;

Kalman_t KalmanX = {
        .Q_angle = 0.001f,
        .Q_bias = 0.003f,
        .R_measure = 0.03f
};

Kalman_t KalmanY = {
        .Q_angle = 0.001f,
        .Q_bias = 0.003f,
        .R_measure = 0.03f,
};

uint8_t MPU6050_Init(void)
{
    uint8_t check;
    uint8_t Data;
    HAL_TIM_Base_Start_IT(&htim2);

    I2C_Read(MPU6050_ADDR, WHO_AM_I_REG, &check, 1);

    if (check == 104)  // 0x68 will be returned by the sensor if everything goes well
    {
        Data = 1;
        I2C_Write(MPU6050_ADDR, PWR_MGMT_1_REG, &Data, 1);

        Data = 0x04;
        I2C_Write(MPU6050_ADDR, SMPLRT_DIV_REG, &Data, 1);

        Data = 0x06;
        I2C_Write(MPU6050_ADDR, ACCEL_CONFIG_REG, &Data, 1);

        Data = 0x18;
        I2C_Write(MPU6050_ADDR, GYRO_CONFIG_REG, &Data, 1);
        return 0;
    }
    return 1;
}


void MPU6050_Read_Accel(MPU6050_t *DataStruct)
{
    uint8_t Rec_Data[6];

    // Read 6 BYTES of data starting from ACCEL_XOUT_H register

    // 使用模拟 I2C 读取加速度计数据
    I2C_Read(MPU6050_ADDR, ACCEL_XOUT_H_REG, Rec_Data, 6);

    DataStruct->Accel_X_RAW = (int16_t) (Rec_Data[0] << 8 | Rec_Data[1]);
    DataStruct->Accel_Y_RAW = (int16_t) (Rec_Data[2] << 8 | Rec_Data[3]);
    DataStruct->Accel_Z_RAW = (int16_t) (Rec_Data[4] << 8 | Rec_Data[5]);

    /*** convert the RAW values into acceleration in 'g'
         we have to divide according to the Full scale value set in FS_SEL
         I have configured FS_SEL = 0. So I am dividing by 16384.0
         for more details check ACCEL_CONFIG Register              ****/

    DataStruct->Ax = DataStruct->Accel_X_RAW / 16384.0;
    DataStruct->Ay = DataStruct->Accel_Y_RAW / 16384.0;
    DataStruct->Az = DataStruct->Accel_Z_RAW / Accel_Z_corrector;
}


void MPU6050_Read_Gyro(MPU6050_t *DataStruct)
{
    uint8_t Rec_Data[6];

    // Read 6 BYTES of data starting from GYRO_XOUT_H register

    // 使用模拟 I2C 读取陀螺仪数据
    I2C_Read(MPU6050_ADDR, GYRO_XOUT_H_REG, Rec_Data, 6);

    DataStruct->Gyro_X_RAW = (int16_t) (Rec_Data[0] << 8 | Rec_Data[1]);
    DataStruct->Gyro_Y_RAW = (int16_t) (Rec_Data[2] << 8 | Rec_Data[3]);
    DataStruct->Gyro_Z_RAW = (int16_t) (Rec_Data[4] << 8 | Rec_Data[5]);

    /*** convert the RAW values into dps (?/s)
         we have to divide according to the Full scale value set in FS_SEL
         I have configured FS_SEL = 0. So I am dividing by 131.0
         for more details check GYRO_CONFIG Register              ****/

    DataStruct->Gx = DataStruct->Gyro_X_RAW / 131.0;
    DataStruct->Gy = DataStruct->Gyro_Y_RAW / 131.0;
    DataStruct->Gz = DataStruct->Gyro_Z_RAW / 131.0;
}

void MPU6050_Read_Temp(MPU6050_t *DataStruct)
{
    uint8_t Rec_Data[2];
    int16_t temp;

    // Read 2 BYTES of data starting from TEMP_OUT_H_REG register

    // 使用模拟 I2C 读取温度数据
    I2C_Read(MPU6050_ADDR, TEMP_OUT_H_REG, Rec_Data, 2);

    temp = (int16_t) (Rec_Data[0] << 8 | Rec_Data[1]);
    DataStruct->Temperature = (float) ((int16_t) temp / (float) 340.0 + (float) 36.53);
}

void MPU6050_Read_All(MPU6050_t *DataStruct)
{
    uint8_t Rec_Data[14];
    int16_t temp;

    // Read 14 BYTES of data starting from ACCEL_XOUT_H register

    // 使用模拟 I2C 读取所有传感器数据
    I2C_Read(MPU6050_ADDR, ACCEL_XOUT_H_REG, Rec_Data, 14);

    DataStruct->Accel_X_RAW = (int16_t) (Rec_Data[0] << 8 | Rec_Data[1]);
    DataStruct->Accel_Y_RAW = (int16_t) (Rec_Data[2] << 8 | Rec_Data[3]);
    DataStruct->Accel_Z_RAW = (int16_t) (Rec_Data[4] << 8 | Rec_Data[5]);
    temp = (int16_t) (Rec_Data[6] << 8 | Rec_Data[7]);
    DataStruct->Gyro_X_RAW = (int16_t) (Rec_Data[8] << 8 | Rec_Data[9]);
    DataStruct->Gyro_Y_RAW = (int16_t) (Rec_Data[10] << 8 | Rec_Data[11]);
    DataStruct->Gyro_Z_RAW = (int16_t) (Rec_Data[12] << 8 | Rec_Data[13]);

    DataStruct->Ax = DataStruct->Accel_X_RAW / 16384.0;
    DataStruct->Ay = DataStruct->Accel_Y_RAW / 16384.0;
    DataStruct->Az = DataStruct->Accel_Z_RAW / Accel_Z_corrector;
    DataStruct->Temperature = (float) ((int16_t) temp / (float) 340.0 + (float) 36.53);
    DataStruct->Gx = DataStruct->Gyro_X_RAW / 131.0;
    DataStruct->Gy = DataStruct->Gyro_Y_RAW / 131.0;
    DataStruct->Gz = DataStruct->Gyro_Z_RAW / 131.0;

    // Kalman angle solve
    double dt = (double) (HAL_GetTick() - timer) / 1000;
    timer = HAL_GetTick();
    double roll;
    double roll_sqrt = sqrt(
            DataStruct->Accel_X_RAW * DataStruct->Accel_X_RAW + DataStruct->Accel_Z_RAW * DataStruct->Accel_Z_RAW);
    if (roll_sqrt != 0.0)
    {
        roll = atan(DataStruct->Accel_Y_RAW / roll_sqrt) * RAD_TO_DEG;
    }
    else
    {
        roll = 0.0;
    }
    double pitch = atan2(-DataStruct->Accel_X_RAW, DataStruct->Accel_Z_RAW) * RAD_TO_DEG;
    if ((pitch < -90 && DataStruct->KalmanAngleY > 90) || (pitch > 90 && DataStruct->KalmanAngleY < -90))
    {
        KalmanY.angle = pitch;
        DataStruct->KalmanAngleY = pitch;
    }
    else
    {
        DataStruct->KalmanAngleY = Kalman_getAngle(&KalmanY, pitch, DataStruct->Gy, dt);
    }
    if (fabs(DataStruct->KalmanAngleY) > 90)
        DataStruct->Gx = -DataStruct->Gx;
    DataStruct->KalmanAngleX = Kalman_getAngle(&KalmanX, roll, DataStruct->Gy, dt);

}

double Kalman_getAngle(Kalman_t *Kalman, double newAngle, double newRate, double dt)
{
    double rate = newRate - Kalman->bias;
    Kalman->angle += dt * rate;

    Kalman->P[0][0] += dt * (dt * Kalman->P[1][1] - Kalman->P[0][1] - Kalman->P[1][0] + Kalman->Q_angle);
    Kalman->P[0][1] -= dt * Kalman->P[1][1];
    Kalman->P[1][0] -= dt * Kalman->P[1][1];
    Kalman->P[1][1] += Kalman->Q_bias * dt;

    double S = Kalman->P[0][0] + Kalman->R_measure;
    double K[2];
    K[0] = Kalman->P[0][0] / S;
    K[1] = Kalman->P[1][0] / S;

    double y = newAngle - Kalman->angle;
    Kalman->angle += K[0] * y;
    Kalman->bias += K[1] * y;

    double P00_temp = Kalman->P[0][0];
    double P01_temp = Kalman->P[0][1];

    Kalman->P[0][0] -= K[0] * P00_temp;
    Kalman->P[0][1] -= K[0] * P01_temp;
    Kalman->P[1][0] -= K[1] * P00_temp;
    Kalman->P[1][1] -= K[1] * P01_temp;

    return Kalman->angle;
}

void MPU6050_Calibration(void)
{
    MPU6050_t data_t;
    for(uint8_t i = 0; i < 100; i++)
    {
        MPU6050_Read_Gyro(&data_t);
        gyroZ_sum += data_t.Gz;
        HAL_Delay(1);
    }
    gyroZ_offset = (float)(gyroZ_sum / 100.0);
}

void MPU6050_Reset(void)
{
    uint8_t Data;

    // 停止定时器中断
    HAL_TIM_Base_Stop_IT(&htim2);

    // 重置定时器计数器
    timer = HAL_GetTick();

    // 复位 MPU6050 寄存器到初始状态
    // 发送复位命令到 PWR_MGMT_1 寄存器，复位整个设备
    Data = 0x80; // 复位命令
    I2C_Write(MPU6050_ADDR, PWR_MGMT_1_REG, &Data, 1);
    HAL_Delay(100); // 等待复位完成

    // 重新初始化 MPU6050
    MPU6050_Init();

    // 清零角度和偏差滤波器
    KalmanX.angle = 0;
    KalmanX.bias = 0;
    KalmanX.P[0][0] = 0;
    KalmanX.P[0][1] = 0;
    KalmanX.P[1][0] = 0;
    KalmanX.P[1][1] = 0;

    KalmanY.angle = 0;
    KalmanY.bias = 0;
    KalmanY.P[0][0] = 0;
    KalmanY.P[0][1] = 0;
    KalmanY.P[1][0] = 0;
    KalmanY.P[1][1] = 0;

    // 重置陀螺仪Z轴的偏移和累积值
    gyroZ_offset = 0;
    gyroZ_sum = 0;
    g_yaw = 0;

    // 重新启动定时器中断
    HAL_TIM_Base_Start_IT(&htim2);
}
