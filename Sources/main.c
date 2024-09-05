#include "../Includes/APP/APP.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototype
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

void PIT_init(uint32_t period);

volatile uint16_t adc_value_0 = 0;

void PIT_init(uint32_t period)
{
    SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
    PIT->MCR &= ~PIT_MCR_MDIS_MASK;

    PIT->CHANNEL[0].LDVAL = period - 1;
    PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK | PIT_TCTRL_TEN_MASK;

    NVIC_EnableIRQ(PIT_IRQn);
    NVIC_SetPriority(ADC0_IRQn, 1);
    NVIC_SetPriority(PIT_IRQn, 2);
}

void PIT_IRQHandler(void)
{

    if (PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK)
    {
        ADC0_Read(DADP3_DAD3);
        adc_value_0 = ADC_returnValue();
        PIT->CHANNEL[0].TFLG &= PIT_TFLG_TIF_MASK;
    }
}

int main(void)
{
    Init_LED_Red();
    Init_LED_Green();
    ADC0_Init();
    PIT_init(48000);

    while (1)
    {
        uint32_t duty_cycle_0 = (adc_value_0 * 4800) / (65535);

        if (((PIT->CHANNEL[0].CVAL)) < duty_cycle_0)
        {
            HAL_GPIO_WritePin(GPIOD, LED_GREEN_PIN, 1);
            HAL_GPIO_WritePin(GPIOE, LED_RED_PIN, 1);
        }
        else
        {
            HAL_GPIO_WritePin(GPIOD, LED_GREEN_PIN, 0);
            HAL_GPIO_WritePin(GPIOE, LED_RED_PIN, 0);
        }
    }

    return 0;
}
