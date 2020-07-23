#include "stdio.h"

/* PMSIS includes. */
#include "pmsis.h"

/* PMSIS BSP includes. */
#include "bsp/bsp.h"
#include "bsp/camera.h"
#include "bsp/display.h"

#include "bsp/display/st7735s.h"

/* Demo includes. */
#include "setup.h"

static pi_task_t task;
static uint16_t *imgBuff0;
static uint16_t *lcd_buff;
static pi_buffer_t buffer;
static struct pi_device lcd;
#if defined(USE_BRIDGE)
static uint64_t fb;
#endif  /* USE_BRIDGE */

//#define display_word

void modiry_buff_for_black_white( uint16_t* buff_imag,uint16_t buff_width,uint16_t buff_high )
{
	for(uint16_t buf_y=0;buf_y<buff_high;buf_y++)
	{
		for(uint16_t buf_x=0;buf_x <buff_width;buf_x++ )
		{
			if(buf_x < 40)
			{
				*(buff_imag + buf_y*buff_width + buf_x) = 0xFFFF;
			}else{
				*(buff_imag + buf_y*buff_width + buf_x) = 0x0;
			}
		}
	}

}

static int32_t open_display(struct pi_device *device)
{
    struct pi_st7735s_conf ili_conf;
    pi_st7735s_conf_init(&ili_conf);
    pi_open_from_conf(device, &ili_conf);
    if (pi_display_open(device))
    {
        printf("Failed to open display\n");
        return -1;
    }
    return 0;
}

void test_camera_with_lcd(void)
{
    printf("Entering main controller...\n");

    imgBuff0 = (uint16_t *) pmsis_l2_malloc((CAMERA_WIDTH * CAMERA_HEIGHT) * sizeof(uint16_t));
    if (imgBuff0 == NULL)
    {
        printf("Failed to allocate Memory for Image \n");
        pmsis_exit(-1);
    }

    lcd_buff = (uint16_t *) pmsis_l2_malloc((LCD_WIDTH * LCD_HEIGHT) * sizeof(uint16_t));
    if (imgBuff0 == NULL)
    {
        printf("Failed to allocate Memory for Image \n");
        pmsis_exit(-1);
    }

    if (open_display(&lcd))
    {
        printf("Failed to open display\n");
        pmsis_exit(-3);
    }

    buffer.data = imgBuff0;
    pi_buffer_init(&buffer, PI_BUFFER_TYPE_L2, imgBuff0);
    pi_buffer_set_format(&buffer, LCD_WIDTH, LCD_HEIGHT, 1, PI_BUFFER_FORMAT_RGB565);

#ifdef display_word
    /* set the color for background */
    writeFillRect(&lcd, 0, 0, 80, 160, 0x00F8);

    /* set the size for the word which whill displayed */
    setTextWrap(&lcd,2);

    /* set the start (x,y) coordinate,which the start area for display some word  */
    setCursor(&lcd, 10, 40);

    /*  the words  for dispaly  */
    writeText(&lcd,"hello", 2 );

    while(1);

#else

    memset(imgBuff0,0xFF,(LCD_WIDTH * LCD_HEIGHT) * sizeof(uint16_t) );

    /* drow video memory for black_white  */
    modiry_buff_for_black_white(imgBuff0, LCD_WIDTH, LCD_HEIGHT);

    /* use udma to display the video memeory  */
    pi_display_write(&lcd, &buffer, 0, 0, 80, 160);

    while(1);
#endif

    pmsis_exit(0);
}

int main(void)
{
    printf("\n\t*** PMSIS LCD(only) Test ***\n\n");
    return pmsis_kickoff((void *) test_camera_with_lcd);
}

