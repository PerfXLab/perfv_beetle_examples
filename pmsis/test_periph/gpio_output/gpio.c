/**
 * GPIO input test using push button on Gapuino.
 * Before using check your board configuration.
 */

/* PMSIS includes */
#include "pmsis.h"


void test_gpio(void)
{
    pi_gpio_pin_configure(NULL, PI_GPIO_A3_PAD_15_B1, PI_GPIO_OUTPUT);
    pi_gpio_pin_configure(NULL, PI_GPIO_A2_PAD_14_A2, PI_GPIO_OUTPUT);
    
    
    while(1)
    {
        pi_gpio_pin_write(NULL, PI_GPIO_A3_PAD_15_B1, 0);
        pi_gpio_pin_write(NULL, PI_GPIO_A2_PAD_14_A2, 0);
        pi_time_wait_us(120000);
        pi_gpio_pin_write(NULL, PI_GPIO_A3_PAD_15_B1, 1);
        pi_gpio_pin_write(NULL, PI_GPIO_A2_PAD_14_A2, 1);
        pi_time_wait_us(120000);
    }

    pmsis_exit(0);
}


/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS GPIO Input ***\n\n");
    return pmsis_kickoff((void *) test_gpio);
}

