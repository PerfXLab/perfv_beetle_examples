#include "stdio.h"
#include "pmsis.h"
#include "bsp/bsp.h"
#include "bsp/ble.h"
#include "bsp/ble/hc08.h"

#define AT_RESP_ARRAY_LENGTH       ( 256 )

static struct pi_device ble;

static uint8_t rx_buffer[AT_RESP_ARRAY_LENGTH];
static uint8_t tx_buffer[AT_RESP_ARRAY_LENGTH];

static struct end_operator end_op = {.first_operator = '%',.second_operator = '%'};

void test_ble(void)
{
    int32_t errors = 0;
    uint32_t buff_size = 0;;
    struct pi_hc08_conf conf = {0};
    char* AT_cmd = NULL;

    pi_ble_hc08_conf_init(&ble, &conf);
    pi_open_from_conf(&ble, &conf);
    errors += pi_ble_open(&ble);
    if (errors)
    {
        printf("Failed to open hc08 ble %ld\n", errors);
        pmsis_exit(-1);
    }

    //pi_ble_ioctl(&ble, PI__HC22_MODIFY_END_OPERATOR, &end_op);

    printf("BLE UART init done\n");

    printf("welcome to perfxlab !\n");
    memcpy(tx_buffer,"welcome to perfxlab !\n",23);
    pi_ble_data_send(&ble,tx_buffer,23);

    hc08_t *hc08 = (hc08_t *) ble.data;

    printf("\n\n");
    AT_cmd = hc08_cmds[HC08_AT_TEST_OK].at_cmd;
    pi_ble_at_cmd(&ble, AT_cmd, (void *) rx_buffer, 1);
    printf("---(%s)---BLE response: %s\n",AT_cmd, rx_buffer);

    printf("\n\n");

    AT_cmd = hc08_cmds[HC08_AT_QUERY_MODULE_PARAMS].at_cmd;
    pi_ble_at_cmd(&ble, AT_cmd, (void *) rx_buffer, 1);
    printf("---(%s)---BLE response:\n",AT_cmd);
    printf("%s\n", rx_buffer);
    printf("\n\n");

    // pi_ble_at_cmd(&ble, "AT+ROLE=M", (void *) rx_buffer, 1);
    // printf("---(AT+ROLE=M)---BLE response:\n");
    // printf("%s\n", rx_buffer);
    // printf("\n\n");

    // pi_time_wait_us(500000);

    // pi_ble_at_cmd(&ble, "AT+ROLE=S", (void *) rx_buffer, 1);
    // printf("---(AT+ROLE=S)---BLE response:\n");
    // printf("%s\n", rx_buffer);
    // printf("\n\n");

    AT_cmd = hc08_cmds[HC08_AT_QUERY_VERSION].at_cmd;
    pi_ble_at_cmd(&ble, AT_cmd, (void *) rx_buffer, 1);
    printf("---(%s)---BLE response:\n",AT_cmd);
    printf("%s\n", rx_buffer);
    printf("\n\n");

    AT_cmd = hc08_cmds[HC08_AT_SET_NAME].at_cmd;
    pi_ble_at_cmd(&ble, AT_cmd, (void *) rx_buffer, 1);
    printf("---(%s)---BLE response:\n",AT_cmd);
    printf("%s\n", rx_buffer);
    printf("\n\n");

    AT_cmd = hc08_cmds[HC08_AT_QUERY_NAME].at_cmd;
    pi_ble_at_cmd(&ble, AT_cmd, (void *) rx_buffer, 1);
    printf("---(%s)---BLE response:\n",AT_cmd);
    printf("%s\n", rx_buffer);
    printf("\n\n");

    AT_cmd = hc08_cmds[HC08_AT_QUERY_MODULE_PARAMS].at_cmd;
    pi_ble_at_cmd(&ble, AT_cmd, (void *) rx_buffer, 1);
    printf("---(%s)---BLE response:\n",AT_cmd);
    printf("%s\n", rx_buffer);
    printf("\n\n");

    AT_cmd = hc08_cmds[HC08_AT_SET_MASTER].at_cmd;
    pi_ble_at_cmd(&ble, AT_cmd, (void *) rx_buffer, 1);
    printf("---(%s)---BLE response:\n",AT_cmd);
    printf("%s\n", rx_buffer);
    printf("\n\n");

    pi_time_wait_us(500000);
    printf("------BLE set Master OK\n");
#if 1
    while (1)
    {
        memset(rx_buffer,0,AT_RESP_ARRAY_LENGTH);
        printf("\n----waite recive data: \n");
        pi_ble_data_get(&ble, rx_buffer, 0);

        printf("----rx_buffer: (%s)---the data len:(%d)\n", rx_buffer,hc08->buffer_len);
        printf("----rx_buffer: \n");
        for(int i=0;i<hc08->buffer_len;i++)
        {
            printf("-(%x)\n", rx_buffer[i]);
        }

        printf("send a data:  !\n");
        printf("-----(%s)!\n",rx_buffer);
        pi_ble_data_send(&ble,rx_buffer,hc08->buffer_len);
    }
#endif

    printf("Exiting BLE test\n");

    pmsis_exit(0);
}

int main(void)
{
    printf("Start HC08 BLE module test\n");
    return pmsis_kickoff((void *) test_ble);
}
