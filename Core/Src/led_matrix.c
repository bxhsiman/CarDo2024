#include "led_matrix.h"

#define MATRIX_COL  64
#define COL_NUM     32
#define SHOW_SPEED  50

uint8_t g_sys_mode = 0;

uint8_t g_col_start = 0;
uint16_t g_col_cnt = 0;

uint8_t g_ShowData[MATRIX_COL] = {
        0x10, 0x8, 0xc, 0xfe, 0x3, 0x48, 0x24, 0x16, 0x83, 0x82, 0xfa, 0x2, 0x12, 0x36, 0x66, 0x0, 0x84, 0x54, 0x7c,
        0x27, 0x74, 0xdc, 0x0, 0x8, 0x89, 0x89, 0xfd, 0xd, 0xb, 0xb, 0x8, 0x0, 0x4, 0x4, 0x4, 0xff, 0x84, 0x84, 0xbf,
        0xa4, 0xa4, 0xa4, 0xbf, 0x84, 0x84, 0x84, 0x4, 0x0, 0x20, 0x20, 0x20, 0xaf, 0xdd, 0x7d, 0x1d, 0xf, 0x1d, 0xfd,
        0x1d, 0x2f, 0x20, 0x20, 0x20, 0x0
};

uint32_t g_ShowPos[COL_NUM] = {
        0xFFFFFFFE, 0xFFFFFFFD, 0xFFFFFFFB, 0xFFFFFFF7,
        0xFFFFFFEF, 0xFFFFFFDF, 0xFFFFFFBF, 0xFFFFFF7F,
        0xFFFFFEFF, 0xFFFFFDFF, 0xFFFFFBFF, 0xFFFFF7FF,
        0xFFFFEFFF, 0xFFFFDFFF, 0xFFFFBFFF, 0xFFFF7FFF,
        0xFFFEFFFF, 0xFFFDFFFF, 0xFFFBFFFF, 0xFFF7FFFF,
        0xFFEFFFFF, 0xFFDFFFFF, 0xFFBFFFFF, 0xFF7FFFFF,
        0xFEFFFFFF, 0xFDFFFFFF, 0xFBFFFFFF, 0xF7FFFFFF,
        0xEFFFFFFF, 0xDFFFFFFF, 0xBFFFFFFF, 0x7FFFFFFF,
};

void led_matrix_init(void) {
    GPIO_InitTypeDef GPIO_InitStruct;
    uint8_t i;

    GPIO_InitStruct.Pin = LED_SHIFT_CLK_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LED_SHIFT_CLK_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LED_LATCH_CLK_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LED_LATCH_CLK_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LED_SHIFT_DATA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LED_SHIFT_DATA_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = SENSOR_PWR_ON_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(SENSOR_PWR_ON_GPIO_Port, &GPIO_InitStruct);

    HAL_GPIO_WritePin(LED_SHIFT_CLK_GPIO_Port, LED_SHIFT_CLK_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_LATCH_CLK_GPIO_Port, LED_LATCH_CLK_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_SHIFT_DATA_GPIO_Port, LED_SHIFT_DATA_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SENSOR_PWR_ON_GPIO_Port, SENSOR_PWR_ON_Pin, GPIO_PIN_SET);
    for (i = 0; i < (COL_NUM + 8); i++) {
        HAL_GPIO_WritePin(LED_SHIFT_CLK_GPIO_Port, LED_SHIFT_CLK_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(LED_SHIFT_CLK_GPIO_Port, LED_SHIFT_CLK_Pin, GPIO_PIN_RESET);
    }
    HAL_GPIO_WritePin(LED_LATCH_CLK_GPIO_Port, LED_LATCH_CLK_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED_LATCH_CLK_GPIO_Port, LED_LATCH_CLK_Pin, GPIO_PIN_RESET);
}

void move_led_matrix(void) {
    g_col_cnt++;

    if (g_col_cnt > SHOW_SPEED)  //
    {
        g_col_cnt = 0;
        if (g_col_start < (MATRIX_COL - 1)) g_col_start++;
        else g_col_start = 0;
    }
}

void show_led_matrix(void) {
    uint32_t i, j, k;
    uint8_t show_char;
    uint32_t show_pos;


    for (i = 0; i < COL_NUM; i++) {
        show_pos = g_ShowPos[i];
        for (k = 0; k < 16; k++) {
            if (show_pos & 0x80000000)
                HAL_GPIO_WritePin(LED_SHIFT_DATA_GPIO_Port, LED_SHIFT_DATA_Pin, GPIO_PIN_SET);
            else
                HAL_GPIO_WritePin(LED_SHIFT_DATA_GPIO_Port, LED_SHIFT_DATA_Pin, GPIO_PIN_RESET);
            show_pos = show_pos << 1;
            HAL_GPIO_WritePin(LED_SHIFT_CLK_GPIO_Port, LED_SHIFT_CLK_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(LED_SHIFT_CLK_GPIO_Port, LED_SHIFT_CLK_Pin, GPIO_PIN_RESET);
        }

        j = g_col_start + i;
        if (j > (MATRIX_COL - 1)) j -= MATRIX_COL;
        show_char = g_ShowData[j];

        for (k = 0; k < 8; k++) {
            if (show_char & 0x80)
                HAL_GPIO_WritePin(LED_SHIFT_DATA_GPIO_Port, LED_SHIFT_DATA_Pin, GPIO_PIN_SET);
            else
                HAL_GPIO_WritePin(LED_SHIFT_DATA_GPIO_Port, LED_SHIFT_DATA_Pin, GPIO_PIN_RESET);
            show_char = show_char << 1;
            HAL_GPIO_WritePin(LED_SHIFT_CLK_GPIO_Port, LED_SHIFT_CLK_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(LED_SHIFT_CLK_GPIO_Port, LED_SHIFT_CLK_Pin, GPIO_PIN_RESET);
        }

        for (k = 0; k < 16; k++) {
            if (show_pos & 0x80000000)
                HAL_GPIO_WritePin(LED_SHIFT_DATA_GPIO_Port, LED_SHIFT_DATA_Pin, GPIO_PIN_SET);
            else
                HAL_GPIO_WritePin(LED_SHIFT_DATA_GPIO_Port, LED_SHIFT_DATA_Pin, GPIO_PIN_RESET);
            show_pos = show_pos << 1;
            HAL_GPIO_WritePin(LED_SHIFT_CLK_GPIO_Port, LED_SHIFT_CLK_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(LED_SHIFT_CLK_GPIO_Port, LED_SHIFT_CLK_Pin, GPIO_PIN_RESET);
        }

        show_char = g_ShowData[j];

        for (k = 0; k < 8; k++) {
            if (show_char & 0x80)
                HAL_GPIO_WritePin(LED_SHIFT_DATA_GPIO_Port, LED_SHIFT_DATA_Pin, GPIO_PIN_SET);
            else
                HAL_GPIO_WritePin(LED_SHIFT_DATA_GPIO_Port, LED_SHIFT_DATA_Pin, GPIO_PIN_RESET);
            show_char = show_char << 1;
            HAL_GPIO_WritePin(LED_SHIFT_CLK_GPIO_Port, LED_SHIFT_CLK_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(LED_SHIFT_CLK_GPIO_Port, LED_SHIFT_CLK_Pin, GPIO_PIN_RESET);
        }

        HAL_GPIO_WritePin(LED_LATCH_CLK_GPIO_Port, LED_LATCH_CLK_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(LED_LATCH_CLK_GPIO_Port, LED_LATCH_CLK_Pin, GPIO_PIN_RESET);
    }
}
