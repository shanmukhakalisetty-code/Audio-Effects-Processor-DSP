#include "main.h" 
#include "arm_math.h" 
#define AUDIO_BUFFER_SIZE 256 
#define ECHO_DELAY 120 
#define ECHO_DECAY 0.4f 
int16_t adcBuffer[AUDIO_BUFFER_SIZE]; 
int16_t dacBuffer[AUDIO_BUFFER_SIZE]; 
float32_t echoBuffer[AUDIO_BUFFER_SIZE + ECHO_DELAY]; 
ADC_HandleTypeDef hadc1; 
DAC_HandleTypeDef hdac; 
TIM_HandleTypeDef htim2; 
void SystemClock_Config(void); 
static void MX_GPIO_Init(void); 
static void MX_ADC1_Init(void); 
static void MX_DAC_Init(void); 
static void MX_TIM2_Init(void); 
void applyEchoEffect(){ 
    for (int i = 0; i < AUDIO_BUFFER_SIZE; i++) { 
        float32_t dry = (float32_t)adcBuffer[i]; 
        float32_t wet = echoBuffer[i]; 
        float32_t out = dry + (wet * ECHO_DECAY); 
        dacBuffer[i] = (int16_t)out; 
        echoBuffer[i + ECHO_DELAY] = dry; 
    } 
    for (int i = 0; i < ECHO_DELAY; i++){ 
        echoBuffer[i] = echoBuffer[AUDIO_BUFFER_SIZE + i]; 
} 
int main(void) 
{ 
    HAL_Init(); 
    SystemClock_Config(); 
    MX_GPIO_Init(); 
    MX_ADC1_Init(); 
    MX_DAC_Init(); 
    MX_TIM2_Init(); 
    HAL_ADC_Start(&hadc1); 
    HAL_DAC_Start(&hdac, DAC_CHANNEL_1); 
    HAL_TIM_Base_Start(&htim2); 
    while (1){ 
        for (int i = 0; i < AUDIO_BUFFER_SIZE; i++){ 
            HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY); 
            adcBuffer[i] = HAL_ADC_GetValue(&hadc1); 
        } 
        applyEchoEffect(); 
        for (int i = 0; i < AUDIO_BUFFER_SIZE; i++) { 
            HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (uint32_t)(dacBuffer[i] & 
0x0FFF)); 
        } 
    } 
} 
void SystemClock_Config(void){ 
    RCC_OscInitTypeDef RCC_OscInitStruct = {0}; 
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0}; 
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI; 
    RCC_OscInitStruct.HSIState = RCC_HSI_ON; 
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT; 
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE; 
    HAL_RCC_OscConfig(&RCC_OscInitStruct); 
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | 
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2; 
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI; 
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1; 
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1; 
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1; 
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0); 
} 
 
static void MX_GPIO_Init(void){ 
    __HAL_RCC_GPIOA_CLK_ENABLE(); 
} 
static void MX_ADC1_Init(void){ 
    hadc1.Instance = ADC1; 
    hadc1.Init.Resolution = ADC_RESOLUTION_12B; 
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE; 
    hadc1.Init.ContinuousConvMode = DISABLE; 
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT; 
    HAL_ADC_Init(&hadc1); 
    ADC_ChannelConfTypeDef config = {0}; 
    config.Channel = ADC_CHANNEL_0; 
    config.Rank = 1; 
    HAL_ADC_ConfigChannel(&hadc1, &config); 
} 
static void MX_DAC_Init(void) 
{ 
    hdac.Instance = DAC; 
    HAL_DAC_Init(&hdac); 
 
    DAC_ChannelConfTypeDef config = {0}; 
    config.DAC_Trigger = DAC_TRIGGER_NONE; 
    config.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE; 
    HAL_DAC_ConfigChannel(&hdac, &config, DAC_CHANNEL_1); 
} 
static void MX_TIM2_Init(void) 
{ 
    htim2.Instance = TIM2; 
    htim2.Init.Prescaler = 16 - 1; 
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP; 
    htim2.Init.Period = 1000 - 1; 
    HAL_TIM_Base_Init(&htim2); 
}
