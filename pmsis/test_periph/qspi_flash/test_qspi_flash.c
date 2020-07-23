/* PMSIS includes */
#include "pmsis.h"
#include "bsp/flash.h"
#include "bsp/flash/spiflash.h"

/* Variables used. */
#define BUFFER_SIZE      ( 20 )

static uint8_t *buff, *rcv_buff;
static uint32_t spi_buff;
static struct pi_device flash;
static struct pi_spiflash_conf conf;

void test_spi_flash(void)
{
    printf("Entering main controller\n\n");

    uint32_t errors = 0;
    struct pi_flash_info flash_info;

    buff = (uint8_t *) pmsis_l2_malloc((uint32_t) BUFFER_SIZE);
    if (buff == NULL)
    {
        printf("buff alloc failed !\n");
        pmsis_exit(-1);
    }

    rcv_buff = (uint8_t *) pmsis_l2_malloc((uint32_t) BUFFER_SIZE);
    if (rcv_buff == NULL)
    {
        printf("rcv_buff alloc failed !\n");
        pmsis_exit(-2);
    }

    for (uint32_t i=0; i<(uint32_t) BUFFER_SIZE; i++)
    {
        buff[i] = i & 0xFF;
        rcv_buff[i] = 0;
    }

    /* Init & open flash. */
    pi_spiflash_conf_init(&conf);
    pi_open_from_conf(&flash, &conf);
    if (pi_flash_open(&flash))
    {
        printf("Error flash open !\n");
        pmsis_exit(-3);
    }

    flash_info.sector_size = 1<<12;
    flash_info.flash_start = 0x0;
    
    spi_buff = flash_info.flash_start;

    pi_flash_erase_sector(&flash, spi_buff);
    printf("Erase sync done.\n");

    pi_flash_read(&flash, spi_buff, rcv_buff, (uint32_t) BUFFER_SIZE);
    printf("Read sync done.\n");

    for (uint32_t i=0; i<(uint32_t) BUFFER_SIZE; i++)
    {
        printf("-----%d-%2x----\n ",i,  rcv_buff[i]);
        if ( rcv_buff[i] != 0xFF)
        {
            errors++;
            printf("{%d-%2x}\n ",i,  rcv_buff[i]);
        }

    }

    printf("\nqspiflash transfer done with %ld error(s) !\n", errors);
    printf("\nTest %s with %ld error(s) !\n", (errors) ? "failed" : "success", errors);

    for (uint32_t i=0; i<(uint32_t) BUFFER_SIZE; i++)
    {
        buff[i] = i & 0xFF;
        rcv_buff[i] = 0;
    }

    pi_flash_program(&flash, spi_buff, buff, (uint32_t) BUFFER_SIZE);
    printf("write sync done.\n");
    pi_flash_read(&flash, spi_buff, rcv_buff, (uint32_t) BUFFER_SIZE);
    printf("Read sync done.\n");

    for (uint32_t i=0; i<(uint32_t) BUFFER_SIZE; i++)
    {
       printf("-----%d-%2x----\n ",i,  rcv_buff[i]);
        if ( rcv_buff[i] != buff[i])
        {
            errors++;
            printf("{%d-%2x-%2x}\n ",i,  rcv_buff[i],buff[i]);
        }
    }

    pmsis_l2_malloc_free(buff, (uint32_t) BUFFER_SIZE);
    pmsis_l2_malloc_free(rcv_buff, (uint32_t) BUFFER_SIZE);
    pi_flash_close(&flash);

    printf("\nqspiflash transfer done with %ld error(s) !\n", errors);
    printf("\nTest %s with %ld error(s) !\n", (errors) ? "failed" : "success", errors);

    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS spiflash Test ***\n\n");
    return pmsis_kickoff((void *) test_spi_flash);
}
