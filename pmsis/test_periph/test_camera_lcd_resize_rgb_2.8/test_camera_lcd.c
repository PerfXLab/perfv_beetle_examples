#include "stdio.h"

/* PMSIS includes. */
#include "pmsis.h"

/* PMSIS BSP includes. */
#include "bsp/bsp.h"
#include "bsp/camera.h"
#include "bsp/display.h"
#include "bsp/display/yt280L030.h"
/* Demo includes. */
#include "setup.h"

static pi_task_t task;
static uint16_t *imgBuff0;
static uint16_t *crop_buff;
static pi_buffer_t buffer;
static pi_buffer_t display_buffer;
static struct pi_device cam;
static struct pi_device lcd;


static uint16_t color_box =    0xFF07;

//#define  crop_factor_0
#define  crop_factor_2
//#define  crop_factor_4
//#define  crop_factor_8

#if defined(crop_factor_0)
static uint16_t crop_width =  320;
static uint16_t crop_high =  240;
static uint16_t crop_factor = 1;
static uint16_t buff_x_off =  0;        //must be zero
static uint16_t buff_y_off =  0;        //must be zero
#elif defined(crop_factor_2)
static uint16_t crop_width =  160;
static uint16_t crop_high =  120;
static uint16_t crop_factor = 2;
static uint16_t buff_x_off =  20;
static uint16_t buff_y_off =  40;
#elif defined(crop_factor_4)
static uint16_t crop_width =  80;
static uint16_t crop_high =  60;
static uint16_t crop_factor = 4;
static uint16_t buff_x_off =  20;
static uint16_t buff_y_off =  40;
#elif defined(crop_factor_8)
static uint16_t crop_width =  40;
static uint16_t crop_high =  30;
static uint16_t crop_factor = 8;
static uint16_t buff_x_off =  20;
static uint16_t buff_y_off =  40;
#endif

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

/*
lcd size: 320 *240
buff: 320*240  diplay_buff: 160*120   crop_factor: 2
buff: 320*240  diplay_buff: 80*60   crop_factor: 4
buff: 320*240  diplay_buff: 40*30   crop_factor: 8
*/
void scale_buff_for_lcd( uint16_t* buff_imag,uint16_t* crop_buff,
                    uint16_t buff_width,uint16_t buff_high ,  
                    uint16_t crop_width, uint16_t crop_high ,
                    uint16_t crop_factor)
{
	uint16_t scale_buf_x = 0;
	uint16_t scale_buf_y = 0;

	for(uint16_t buf_x = 1; (scale_buf_x = buf_x*crop_factor) <= (buff_width); buf_x++ )
	{
			for(uint16_t buf_y = 1; (scale_buf_y = buf_y*crop_factor) <= (buff_high); buf_y++ )
			{
				*(crop_buff + (buf_y-1) * crop_width + (buf_x-1) ) =   color_MSB_to_LSB( *( buff_imag + (scale_buf_y-1) * buff_width + (scale_buf_x-1) ) ) ;
			}	
	}

}

/*
draw box.
*/
void modiry_buff_for_box( uint16_t* buff_imag,uint16_t buff_width,uint16_t buff_high ,  uint16_t interval)
{
    
	for(uint16_t buf_y = 0; buf_y < buff_high; buf_y++)
	{
		for(uint16_t buf_x = 0; buf_x < buff_width; buf_x++)
		{
			if((buf_y == interval) || (buf_y == buff_high - interval -1))
			{
				*(buff_imag + buf_y*buff_width + buf_x) = (color_box);
			}else{
				if((buf_x == interval) || (buf_x == buff_width - interval -1) )     //
				{
					*(buff_imag + buf_y*buff_width + buf_x) = (color_box);
				}else{
                   // *(buff_imag + buf_y*buff_width + buf_x) = color_MSB_to_LSB (*(buff_imag + buf_y*buff_width + buf_x));
				}
			}
		}
	}
}

/*
display by pixel.
*/
void display_world(struct pi_device *device, uint16_t* buff_imag,
        uint16_t buff_width,uint16_t buff_high  ,
        uint16_t lcd_width, uint16_t lcd_high,  
        uint16_t lcd_x_off, uint16_t lcd_y_off)
{
    if( ((lcd_x_off + buff_width) > lcd_width) || ((lcd_y_off + buff_high) > lcd_high  ) )
    {
        printf(" invalid parameter !!!\n ");
        return;
    }

   for(uint16_t off_y = 0; off_y < buff_high; off_y++)
   {
       for(uint16_t off_x=0; off_x < buff_width; off_x++)
       {  
           writeFillRect(&lcd,(off_x + lcd_x_off),(off_y + lcd_y_off),1,1, color_MSB_to_LSB(*(buff_imag + (buff_width * off_y) + off_x ))  );
       }
   }
}

static int32_t open_display(struct pi_device *device)
{
    struct pi_yt280_conf ili_conf;
    pi_yt280_conf_init(&ili_conf);
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
    crop_buff = (uint16_t *) pmsis_l2_malloc((crop_width * crop_high) * sizeof(uint16_t));
    if (imgBuff0 == NULL)
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

    pi_display_ioctl(&lcd, PI_YT280_IOCTL_ORIENTATION, (void *) PI_YT280_ORIENTATION_90);

    buffer.data = imgBuff0;
    pi_buffer_init(&buffer, PI_BUFFER_TYPE_L2, imgBuff0);

    display_buffer.data = crop_buff;
    pi_buffer_init(&display_buffer, PI_BUFFER_TYPE_L2, crop_buff);

    pi_buffer_set_format(&buffer, CAMERA_WIDTH, CAMERA_HEIGHT, 1, PI_BUFFER_FORMAT_RGB565);
    pi_buffer_set_format(&display_buffer, crop_width, crop_high, 1, PI_BUFFER_FORMAT_RGB565);

    //writeFillRect(&lcd, 0, 0, 320, 240, 0x00F8);

    printf("Main loop start\n");
    while (1)
    {
        printf("Camera start.\n");
        pi_camera_control(&cam, PI_CAMERA_CMD_START, 0);
        pi_camera_capture(&cam, imgBuff0, CAMERA_WIDTH * CAMERA_HEIGHT * sizeof(uint16_t));
        pi_camera_control(&cam, PI_CAMERA_CMD_STOP, 0);
        printf("Camera stop.\n");

        scale_buff_for_lcd( imgBuff0, crop_buff,CAMERA_WIDTH, CAMERA_HEIGHT ,  crop_width, crop_high,crop_factor);
		modiry_buff_for_box( crop_buff,crop_width,crop_high , 3);

        printf("display start.\n");
		display_world(&lcd,crop_buff,crop_width, crop_high,LCD_WIDTH,LCD_HEIGHT,buff_x_off,buff_y_off);
        printf("display over.\n");
    }

    pmsis_exit(0);
}

int main(void)
{
    printf("\n\t*** PMSIS Camera with LCD Test ***\n\n");
    return pmsis_kickoff((void *) test_camera_with_lcd);
}
