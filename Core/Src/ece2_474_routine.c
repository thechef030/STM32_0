/*
 * 아래쪽 HAL 콜백은 자동 생성된 인터럽트 처리 코드와 실습 함수를 연결합니다.
 * 인터럽트에서는 처리할 일을 기록하고 빠르게 돌아옵니다.
 * 인터럽트 함수 안에서는 HAL_Delay, scanf, getchar, printf를 사용하지 않습니다.
 */
#include "ece2_474.h"
#include "main.h"
/* 주변장치 변수의 extern 선언은 ece2_474.h에 있습니다. */

/* UART 인터럽트 수신 실습 때 필요한 버퍼의 주석을 해제합니다.
 * 파일 범위에 두면 수신 완료 함수에서도 같은 버퍼를 사용할 수 있습니다.
 */
// static uint8_t uart2_rx_ece2;
// static uint8_t uart3_rx_ece2;

/* 주변장치 초기화가 끝난 뒤, 메인 루프에 들어가기 전에 한 번 호출됩니다. */
void ece2_474_init(void)
{
    /* TIM6: 100 ms마다 tim6_isr_p4()를 호출합니다. */
    // HAL_TIM_Base_Start_IT(&htim6);

    /* TIM3: PB4에서 1 kHz PWM을 출력합니다. 비교값 500은 50%입니다. */
    // HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
    // __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 500U);

    /* UART2: PC에서 한 바이트를 받으면 uart2_isr_p6()를 호출합니다.
     * 위의 uart2_rx_ece2 버퍼도 주석을 해제해야 합니다.
     * 수신 완료 함수에서 HAL_UART_Receive_IT를 다시 호출해야 다음 입력도 받습니다.
     * getchar·fgets·scanf로 같은 UART를 읽는 방식과 함께 사용하지 않습니다.
     */
    // HAL_UART_Receive_IT(&huart2, &uart2_rx_ece2, 1U);

    /* UART3: FPGA에서 한 바이트를 받으면 uart3_isr_p5()를 호출합니다.
     * 위의 uart3_rx_ece2 버퍼도 주석을 해제해야 합니다.
     * UART는 한 번 수신한 뒤, 수신 완료 함수에서 다시 수신을 요청합니다.
     */
    // HAL_UART_Receive_IT(&huart3, &uart3_rx_ece2, 1U);

    /* GPIO 입력 인터럽트는 이미 켜져 있어 별도 시작 호출이 필요 없습니다.
     * SPI·I2C와 UART 송신도 별도 시작 없이 필요한 시점에 송수신 함수를 호출합니다.
     */
}

/* 메인 루프에서 반복해서 호출됩니다. */
void polling_routine(void)
{
    /* static 변수는 다음 호출에서도 값을 유지합니다. 시간 단위는 ms입니다. */
    static uint32_t interval_ms = 1000U;
    static uint32_t last_toggle_ms = 0U;
    static uint32_t last_press_ms = 0U;
    static uint8_t press_seen = 0U;
    static GPIO_PinState previous = GPIO_PIN_RESET;

    uint32_t now_ms = HAL_GetTick();
    GPIO_PinState current = HAL_GPIO_ReadPin(
        BUTTON_ece2_GPIO_Port, BUTTON_ece2_Pin);

    /* LOW -> HIGH인 누른 순간만 처리합니다. 길게 눌러도 한 번만 바뀝니다.
     * 첫 입력은 바로 받고, 그 뒤 200 ms 이내의 반복 입력은 무시합니다.
     */
    if (current == GPIO_PIN_SET && previous == GPIO_PIN_RESET
        && (press_seen == 0U || now_ms - last_press_ms >= 200U))
    {
        last_press_ms = now_ms;
        press_seen = 1U;

        /* 1000 -> 2000 -> 4000 -> 8000 -> 1000 ms */
        /* {{순환한다는 의미의 코드이다}}*/
        interval_ms = (interval_ms == 8000U) ? 1000U : interval_ms * 2U;

        /* 누른 시점부터 새 간격을 재어 다음 LED 상태 변경에 적용합니다. */
        last_toggle_ms = now_ms;
    }
    previous = current;

    /* {{기다리는 함수를 사용하지 않으므로 LED 점멸 중에도 버튼을 읽는다.}}
     * unsigned 시간 차이를 사용해 HAL_GetTick()의 값이 순환해도 처리합니다.
     */
    if (now_ms - last_toggle_ms >= interval_ms)
    {
        last_toggle_ms = now_ms;
        /*{{과제에 따라서 LED 상태가 변경된다}}*/
#if defined(STM32_0_EXTERNAL_LED_PB0)
        /* 과제 2: PB0에 연결한 외부 LED */
        HAL_GPIO_TogglePin(GPIO_OUT1_ece2_GPIO_Port, GPIO_OUT1_ece2_Pin);
#else
        /* 과제 1: PA5 내장 LED */
        HAL_GPIO_TogglePin(LED_ece2_GPIO_Port, LED_ece2_Pin);
#endif
    }
}

/* 1. 사용자 버튼 PC13을 눌렀을 때 호출됩니다. 우선순위: 7.
 * 버튼의 접점 때문에 한 번 눌러도 여러 번 호출될 수 있습니다.
 */
static void button_isr_p7(void)
{
    /* 실습 코드 작성: 버튼 이벤트를 기록하거나 LED 상태를 변경합니다. */
}

/* 2. TIM6를 인터럽트 방식으로 시작하면 100 ms마다 호출됩니다. 우선순위: 4.
 * 시작 호출: HAL_TIM_Base_Start_IT(&htim6);
 */
static void tim6_isr_p4(void)
{
    /* 실습 코드 작성: 횟수를 세거나 메인 루프에서 처리할 표시를 남깁니다. */
}

/* 3. PC 통신 UART2의 요청한 수신이 완료되면 호출됩니다. 우선순위: 6.
 * 먼저 수신 버퍼를 준비하고 HAL_UART_Receive_IT로 수신을 요청해야 합니다.
 * 다음 입력도 받으려면 처리 후 수신을 다시 요청합니다.
 * 현재 표준 입력은 기다리는 방식입니다. 같은 UART의 인터럽트 수신과 함께 쓰지 않습니다.
 */
static void uart2_isr_p6(void)
{
    /* 실습 코드 작성: PC 수신 버퍼를 처리하고 다음 수신을 요청합니다. */
}

/* 4. FPGA 통신 UART3의 요청한 수신이 완료되면 호출됩니다. 우선순위: 5.
 * 먼저 수신 버퍼를 준비하고 HAL_UART_Receive_IT로 수신을 요청해야 합니다.
 * 다음 입력도 받으려면 처리 후 수신을 다시 요청합니다.
 */
static void uart3_isr_p5(void)
{
    /* 실습 코드 작성: FPGA 수신 버퍼를 처리하고 다음 수신을 요청합니다. */
}

/* PA0: 풀업 입력의 HIGH -> LOW 변화. 우선순위: 8. */
static void gpio_in_pullup_isr_p8_ece2(void)
{
    /* 실습 코드 작성 */
}

/* PA1: 풀다운 입력의 LOW -> HIGH 변화. 우선순위: 9. */
static void gpio_in_pulldown_isr_p9_ece2(void)
{
    /* 실습 코드 작성 */
}

/* 아래는 HAL 콜백과 실습 함수를 연결하는 코드입니다. */

/* CubeMX의 EXTI 처리 코드에서 버튼 실습 함수로 연결합니다. */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == BUTTON_ece2_Pin)
    {
        button_isr_p7();
    }
    else if (GPIO_Pin == GPIO_IN_PULLUP_ece2_Pin)
    {
        gpio_in_pullup_isr_p8_ece2();
    }
    else if (GPIO_Pin == GPIO_IN_PULLDOWN_ece2_Pin)
    {
        gpio_in_pulldown_isr_p9_ece2();
    }
}

/* CubeMX의 타이머 처리 코드에서 TIM6 실습 함수로 연결합니다. */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM6)
    {
        tim6_isr_p4();
    }
}

/* UART 수신 완료 시 PC 통신과 FPGA 통신을 구분합니다. */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        uart2_isr_p6();
    }
    else if (huart->Instance == USART3)
    {
        uart3_isr_p5();
    }
}
