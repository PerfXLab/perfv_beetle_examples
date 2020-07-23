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
static uint16_t *temp_imgBuff;
static pi_buffer_t buffer;
static struct pi_device cam;
static struct pi_device lcd;

static uint16_t color_box =    0xFF07; 


#define GRAY_2_RGB565(gray) ((uint16_t)((((uint8_t)(gray)>>3)<<11)|(((uint8_t)(gray)>>2)<<5)|((uint8_t)(gray)>>3)))

/*
    YUV sample Y only.
*/
uint16_t* yuv_only_sample_y(uint16_t * input_img , uint16_t width,uint16_t high)
{
    uint16_t yuv_color = 0;
    uint8_t y_color = 0;

    for(uint16_t in_y = 0; in_y < high; in_y++)
    {
        for(uint16_t in_x = 0; in_x < width; in_x++)
        {
            yuv_color =  *(input_img + in_y * width + in_x);
            y_color = (yuv_color >> 8) & 0xFF;

            *(input_img + in_y * width + in_x) = GRAY_2_RGB565(y_color);
        }
    }

    return input_img;
}


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
                    *(buff_imag + buf_y*buff_width + buf_x) = color_MSB_to_LSB (*(buff_imag + buf_y*buff_width + buf_x));
				}
			}
		}
	}
}

#if 0
/*
diplay by world
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
					writeFillRect(&lcd,(buff_x - buff_x_off),(buff_y - buff_y_off),1,1, color_MSB_to_LSB( *(buff_imag + (buff_y * buff_width) + buff_x ) ) );
				}		
			}
		}
	}
}
#endif

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
    pi_buffer_set_format(&buffer, CAMERA_WIDTH, CAMERA_HEIGHT, 1, PI_BUFFER_FORMAT_RGB565);

    //writeFillRect(&lcd, 0, 0, LCD_WIDTH, LCD_HEIGHT, 0x00F8);

    printf("Main loop start\n");
    while (1)
    {

        //printf("Camera start.\n");
        pi_camera_control(&cam, PI_CAMERA_CMD_START, 0);
        pi_camera_capture(&cam, imgBuff0, CAMERA_WIDTH * CAMERA_HEIGHT * sizeof(uint16_t));
        pi_camera_control(&cam, PI_CAMERA_CMD_STOP, 0);
        //printf("Camera stop.\n");

        /*
            yuv convert to Y and Y convert to RGB
        */
        temp_imgBuff = yuv_only_sample_y(imgBuff0,CAMERA_WIDTH,CAMERA_HEIGHT);
        modiry_buff_for_box( temp_imgBuff,CAMERA_WIDTH,CAMERA_HEIGHT , 3);

        //printf("display start.\n");
        pi_display_write(&lcd, &buffer, 0, 0, LCD_WIDTH, LCD_HEIGHT);
        //printf("display over.\n");
    }

    pmsis_exit(0);
}

int main(void)
{
    printf("\n\t*** PMSIS Camera with LCD Test ***\n\n");
    return pmsis_kickoff((void *) test_camera_with_lcd);
}
