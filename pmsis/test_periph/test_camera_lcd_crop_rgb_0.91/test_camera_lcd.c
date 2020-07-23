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
static pi_buffer_t cam_buffer;
static struct pi_device cam;
static struct pi_device lcd;

static uint16_t color_box =  0xFF07;
static uint16_t buff_x_off =  00;
static uint16_t buff_y_off =  00;

/*
converted red and blue.
*/
uint16_t color_R_to_B(uint16_t color)
{
    uint16_t tmpcolor =0;
    tmpcolor |= ( (color >> 11) & 0x1F );
    tmpcolor |= ((color & 0x1F) << 11) ;
    tmpcolor |= (color & 0x7E0)  ;
    color = tmpcolor;

    return color;
}

/*
converted byte order.
*/
uint16_t  color_MSB_to_LSB (uint16_t color)
{
    uint16_t tmpcolor = 0;
    tmpcolor |= ((color >> 8) & 0xFF);
    tmpcolor |= ((color & 0xFF) << 8);
    color = tmpcolor;

    return color;
}

/*
converted color.
*/
uint16_t converted_color(uint16_t color)
{
    uint16_t tmpcolor = 0;
    tmpcolor = color_R_to_B(color);
    color = color_MSB_to_LSB(tmpcolor);
    return color;
}

#ifdef DISPLAY_SIZE_320X240
/*
display buff : 320*240

buff[0][] = { x1, x2, x3, ..., x319};
buff[1][] = { x1, x2, x3, ..., x319};
...
buff[239][] = { x1, x2, x3, ..., x319};

but lcd display buff : 80 * 160

we need do like this:
	buff y is 240.  take it for lcd x (80).  scale one third.(1/3)
	buff x is 320.  take it for lcd y (160). scale one second.(1/2)


lcd_buff :80 * 160

lcd_buff[0][] = {x1,x2,x3,...,x79};
lcd_buff[1][] = {x1,x2,x3,...,x79};
...
lcd_buff[159][] = {x1,x2,x3,...,x79};
*/
void scale_buff_for_lcd( uint16_t* buff_imag,uint16_t* lcd_buff,uint16_t buff_width,uint16_t buff_high ,  uint16_t lcd_width, uint16_t lcd_high)
{
	uint16_t scale_buf_x = 0;
	uint16_t scale_buf_y = 0;

	for(uint16_t buf_x = 1; (scale_buf_x = buf_x*2) <= (buff_width); buf_x++ )		//buff width. scale to 160
	{
			for(uint16_t buf_y = 1; (scale_buf_y = buf_y*3) <= (buff_high); buf_y++ )		//buff high,scale to 80
			{
				*(lcd_buff + (buf_x-1) * lcd_width + (buf_y-1) ) = *( buff_imag + (scale_buf_y-1) * buff_width + (scale_buf_x-1) );
			}	
	}

}

#else

/*
320 *160

*/
void scale_buff_for_lcd( uint16_t* buff_imag,uint16_t* lcd_buff,uint16_t buff_width,uint16_t buff_high ,  uint16_t lcd_width, uint16_t lcd_high)
{
	uint16_t scale_buf_x = 0;
	uint16_t scale_buf_y = 0;

	for(uint16_t buf_x = 1; (scale_buf_x = buf_x*2) <= (buff_width); buf_x++ )		//buff width. scale to 160
	{
			for(uint16_t buf_y = 1; (scale_buf_y = buf_y*2) <= (buff_high); buf_y++ )		//buff high,scale to 80
			{
				*(lcd_buff + (buf_x-1) * lcd_width + (buf_y-1) ) = *( buff_imag + (scale_buf_y-1) * buff_width + (scale_buf_x-1) );
			}	
	}

}
#endif

/*
draw box.
*/
void modiry_buff_for_box( uint16_t* buff_imag,uint16_t buff_width,uint16_t buff_high ,  uint16_t buff_x_off, uint16_t buff_y_off)
{

	for(uint16_t buf_y = 0; buf_y < buff_high; buf_y++)
	{
		for(uint16_t buf_x = 0; buf_x < buff_width; buf_x++)
		{
			if((buf_y == 1 + buff_y_off) || (buf_y == 158 + buff_y_off))
			{
				//buff_imag[buf_y][buf_x] = color_box;

				*(buff_imag + buf_y*buff_width + buf_x) = color_box;
			}else{
				if((buf_x == 1+ buff_x_off) || (buf_x == 78+ buff_x_off) )     //
				{
					*(buff_imag + buf_y*buff_width + buf_x) = color_box;
				}else{
					*(buff_imag + buf_y*buff_width + buf_x) = color_MSB_to_LSB(*(buff_imag + buf_y*buff_width + buf_x));
				}
			}
		}
	}
}

#if 0
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
#endif

/*
Render global:
	uint16_t* buff_imag	:  the buffer for display.
	uint16_t buff_width :  the width of the diplay buffer
	uint16_t buff_high  :  the high of the diaplay buffer
	uint16_t lcd_width  :  the width of the lcd`s width pixel
	uint16_t lcd_high	:  the hight of the lcd`s high pixel
	uint16_t buff_x_off	:  the offset(x axis)  of the display buffer  where display relative to the buff
	uint16_t buff_y_off	:  the offset(y axis)  of the display buffer  where display relative to the buff

*/
void display_world(struct pi_device *device, uint16_t* buff_imag,uint16_t buff_width,uint16_t buff_high  ,uint16_t lcd_width, uint16_t lcd_high,  uint16_t buff_x_off, uint16_t buff_y_off)
{

	for(uint16_t buff_y=0; buff_y < buff_high; buff_y++ )
	{
		for(uint16_t buff_x=0; buff_x < buff_width; buff_x++)
		{
			if((buff_x >= buff_x_off) && (buff_y >= buff_y_off))
			{
				if(((buff_x - buff_x_off) < lcd_width) && ( (buff_y - buff_y_off) < lcd_high ))	
				{
					writeFillRect(&lcd,(buff_x - buff_x_off),(buff_y - buff_y_off),1,1, color_MSB_to_LSB(*(buff_imag + (buff_y * buff_width) + buff_x )) );
				}		
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

static int32_t open_camera_ov7725(struct pi_device *device)
{
    struct pi_ov7725_conf cam_conf;
    pi_ov7725_conf_init(&cam_conf);
    cam_conf.format = PI_CAMERA_QVGA;
    pi_open_from_conf(device, &cam_conf);
    if (pi_camera_open(device))
    {
        return -1;
    }
    return 0;
}


static int32_t open_camera(struct pi_device *device)
{
    return open_camera_ov7725(device);

    return -1;
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
    if (lcd_buff == NULL)
    {
        printf("Failed to allocate Memory for Image \n");
        pmsis_exit(-1);
    }

    if (open_camera(&cam))
    {
        printf("Failed to open camera\n");
        pmsis_exit(-2);
    }

    if (open_display(&lcd))
    {
        printf("Failed to open display\n");
        pmsis_exit(-3);
    }

	cam_buffer.data = imgBuff0;
    buffer.data = lcd_buff;

	pi_buffer_init(&cam_buffer, PI_BUFFER_TYPE_L2, imgBuff0);
    pi_buffer_init(&buffer, PI_BUFFER_TYPE_L2, lcd_buff);

    pi_buffer_set_format(&buffer, LCD_WIDTH, LCD_HEIGHT, 10, PI_BUFFER_FORMAT_RGB565);
	pi_buffer_set_format(&cam_buffer, CAMERA_WIDTH, CAMERA_HEIGHT, 1, PI_BUFFER_FORMAT_RGB565);

    writeFillRect(&lcd, 0, 0, 80, 160, 0x00F8);
    
    printf("Main loop start\n");
    while (1)
    {

        //printf("Camera start.\n");
    	pi_camera_control(&cam, PI_CAMERA_CMD_START, 0);
        pi_camera_capture(&cam, imgBuff0, CAMERA_WIDTH * CAMERA_HEIGHT * sizeof(uint16_t));
        pi_camera_control(&cam, PI_CAMERA_CMD_STOP, 0);
        //printf("Camera stop.\n");

		scale_buff_for_lcd( imgBuff0, lcd_buff,CAMERA_WIDTH, CAMERA_HEIGHT ,  LCD_WIDTH, LCD_HEIGHT);
		modiry_buff_for_box( lcd_buff,LCD_WIDTH,LCD_HEIGHT , buff_x_off,buff_y_off);

       // printf("display start.\n");
#ifdef USE_UDMA_FOR_DISPLAY
		pi_display_write(&lcd, &buffer, 0, 0, LCD_WIDTH, LCD_HEIGHT);
#else
		display_world(&lcd,lcd_buff,LCD_WIDTH, LCD_HEIGHT,LCD_WIDTH,LCD_HEIGHT,buff_x_off,buff_y_off);
#endif 
       // printf("display over.\n");
		
    }

    pmsis_exit(0);
}

int main(void)
{
    printf("\n\t*** PMSIS Camera with LCD Test ***\n\n");
    return pmsis_kickoff((void *) test_camera_with_lcd);
}
