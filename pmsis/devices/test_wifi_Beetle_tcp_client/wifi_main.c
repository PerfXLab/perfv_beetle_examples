#include "stdio.h"
#include "pmsis.h"
#include "bsp/bsp.h"
#include "bsp/wifi.h"
#include "bsp/wifi/hc22.h"

#define AT_RESP_ARRAY_LENGTH       ( 256 )

//AT+WSTA=PerfXLab,perfxlab2017
#define ROUTER_NAME             "PerfXLab"
#define ROUTER_PASSWORD         "perfxlab2017"
#define AT_WSTA                 ("AT+WSTA=" ROUTER_NAME "," ROUTER_PASSWORD)

//AT+SOCK=TCPC,192.168.1.104,8080
#define TCP_S_C             "TCPC"
#define IP_TO_CONNECT       "192.168.1.160"
#define PORT_TO_CONNECT     "8080"
#define AT_SOCK             ("AT+SOCK=" TCP_S_C "," IP_TO_CONNECT "," PORT_TO_CONNECT)

static struct pi_device wifi;

static uint8_t rx_buffer[AT_RESP_ARRAY_LENGTH];
static uint8_t tx_buffer[AT_RESP_ARRAY_LENGTH];

static struct end_operator end_op = {.first_operator = '%',.second_operator = '%'};

void test_wifi(void)
{
    int32_t errors = 0;
    uint32_t buff_size = 0;;
    struct pi_hc22_conf conf = {0};
    char* AT_cmd = NULL;

    pi_wifi_hc22_conf_init(&wifi, &conf);
    pi_open_from_conf(&wifi, &conf);
    errors += pi_wifi_open(&wifi);
    if (errors)
    {
        printf("Failed to open hc22 wifi %ld\n", errors);
        pmsis_exit(-1);
    }

    pi_wifi_ioctl(&wifi, PI__HC22_MODIFY_END_OPERATOR, &end_op);

    printf("WIFI UART init done\n");

    printf("welcome to perfxlab !\n");
    // memcpy(tx_buffer,"welcome to perfxlab !\n",23);
    // pi_wifi_data_send(&wifi,tx_buffer,23);

    hc22_t *hc22 = (hc22_t *) wifi.data;

    printf("\n\n");
    
    /* entry the mode of AT */
    AT_cmd = hc22_cmds[HC22_AT_ENTRY_AT_MODE].at_cmd;
    pi_wifi_at_cmd(&wifi, AT_cmd, (void *) rx_buffer, 1);
    printf("---(%s)---WIFI response: %s\n", AT_cmd,rx_buffer);
    printf("\n\n");

    /* set uart information 9600,none,1 */
    AT_cmd = hc22_cmds[HC22_AT_S_UART_INFO].at_cmd;
    pi_wifi_at_cmd(&wifi, AT_cmd, (void *) rx_buffer, 1);
    printf("---(%s)---WIFI response: %s\n", AT_cmd,rx_buffer);
    printf("\n\n");

    /* print uart info */
    AT_cmd = hc22_cmds[HC22_AT_G_UART_INFO].at_cmd;
    pi_wifi_at_cmd(&wifi, AT_cmd, (void *) rx_buffer, 1);
    printf("---(%s)---WIFI response: %s\n", AT_cmd,rx_buffer);
    printf("\n\n");

    /* set the mode for AT mode, AT+MODE=1  */
    AT_cmd = hc22_cmds[HC22_AT_S_AT_MODE].at_cmd;
    pi_wifi_at_cmd(&wifi, AT_cmd, (void *) rx_buffer, 1);
    printf("---(%s)---WIFI response: %s\n", AT_cmd,rx_buffer);
    printf("\n\n");

    /* show the mode for AT mode */
    AT_cmd = hc22_cmds[HC22_AT_G_AT_MODE].at_cmd;
    pi_wifi_at_cmd(&wifi, AT_cmd, (void *) rx_buffer, 1);
    printf("---(%s)---WIFI response: %s\n", AT_cmd,rx_buffer);
    printf("\n\n");

    /* set the mode of wifi module for working  AT+WMODE=STA */
    AT_cmd = hc22_cmds[HC22_AT_SET_WIFI_MODEL_STA].at_cmd;
    pi_wifi_at_cmd(&wifi, AT_cmd, (void *) rx_buffer, 1);
    printf("---(%s)---WIFI response: %s\n", AT_cmd,rx_buffer);
    printf("\n\n");

    /* show the mode of wifi module for working */
    AT_cmd = hc22_cmds[HC22_AT_QUERY_WIFI_MODEL].at_cmd;
    pi_wifi_at_cmd(&wifi, AT_cmd, (void *) rx_buffer, 1);
    printf("---(%s)---WIFI response: %s\n", AT_cmd,rx_buffer);
    printf("\n\n");

    /* set info of the AP,we want to connect AT+WSTA=CHKCPT,cuihukechuangpingtai  */
    AT_cmd = AT_WSTA;
    pi_wifi_at_cmd(&wifi, AT_cmd, (void *) rx_buffer, 1);
    printf("---(%s)---WIFI response: %s\n", AT_cmd,rx_buffer);
    printf("\n\n");

    /* show the info of AP,we set */
    AT_cmd = hc22_cmds[HC22_AT_QUERY_CORRELATION_AP].at_cmd;
    pi_wifi_at_cmd(&wifi, AT_cmd, (void *) rx_buffer, 1);
    printf("---(%s)---WIFI response: %s\n", AT_cmd,rx_buffer);
    printf("\n\n");

    /* set the socket to the on state ,AT+SOCKSTATUS=ON  */
    AT_cmd = hc22_cmds[HC22_AT_S_SOCKET_STATUS].at_cmd;
    pi_wifi_at_cmd(&wifi, AT_cmd, (void *) rx_buffer, 1);
    printf("---(%s)---WIFI response: %s\n", AT_cmd,rx_buffer);
    printf("\n\n");

    /* show the state for socket */
    AT_cmd = hc22_cmds[HC22_AT_G_SOCKET_STATUS].at_cmd;
    pi_wifi_at_cmd(&wifi, AT_cmd, (void *) rx_buffer, 1);
    printf("---(%s)---WIFI response: %s\n", AT_cmd,rx_buffer);
    printf("\n\n");

    /* set info for socket,AT+SOCK=TCPC,10.30.101.202,8080 */
    AT_cmd = AT_SOCK ;                        //"AT+SOCK=TCPC,192.168.1.160,8080";
    pi_wifi_at_cmd(&wifi, AT_cmd, (void *) rx_buffer, 1);
    printf("---(%s)---WIFI response: %s\n", AT_cmd,rx_buffer);
    printf("\n\n");

    /* show the info of socket */
    AT_cmd = hc22_cmds[HC22_AT_G_SOCKET_INFO].at_cmd;
    pi_wifi_at_cmd(&wifi, AT_cmd, (void *) rx_buffer, 1);
    printf("---(%s)---WIFI response: %s\n", AT_cmd,rx_buffer);
    printf("\n\n");

    /* set the IP mode, to make it "DHCP" : AT+WANN=DHCP */
    AT_cmd = hc22_cmds[HC22_AT_SET_STA_PARAMS].at_cmd;
    pi_wifi_at_cmd(&wifi, AT_cmd, (void *) rx_buffer, 1);
    printf("---(%s)---WIFI response: %s\n", AT_cmd,rx_buffer);
    printf("\n\n");

    /* show the IP addr for your wifi module,and the mode for IP.such ad:"DHCP" or "STATIC".
        you`d better make it "DHCP".
     */
    AT_cmd = hc22_cmds[HC22_AT_QUERY_STA_PARAMS].at_cmd;
    pi_wifi_at_cmd(&wifi, AT_cmd, (void *) rx_buffer, 1);
    printf("---(%s)---WIFI response: %s\n", AT_cmd,rx_buffer);
    printf("\n\n");

    /* set tcp auto connection on: AT+SOCKAUTO=ON */
    AT_cmd = hc22_cmds[HC22_AT_S_TCP_AUTO_CONNECT_STATUS].at_cmd;
    pi_wifi_at_cmd(&wifi, AT_cmd, (void *) rx_buffer, 1);
    printf("---(%s)---WIFI response: %s\n", AT_cmd,rx_buffer);
    printf("\n\n");

    /* show the tcp auto connection status */
    AT_cmd = hc22_cmds[HC22_AT_G_TCP_AUTO_CONNECT_STATUS].at_cmd;
    pi_wifi_at_cmd(&wifi, AT_cmd, (void *) rx_buffer, 1);
    printf("---(%s)---WIFI response: %s\n", AT_cmd,rx_buffer);
    printf("\n\n");

    /* reset hc22 */
    AT_cmd = hc22_cmds[HC22_AT_MODULE_RESET].at_cmd;
    pi_wifi_at_cmd(&wifi, AT_cmd, (void *) rx_buffer, 1);
    printf("---(%s)---WIFI response: %s\n", AT_cmd,rx_buffer);
    printf("\n\n");


    printf("------WIFI set clinet OK,but you should wait about 10 seconds !!\n");

    while (1)
    {

        memset(rx_buffer,0,AT_RESP_ARRAY_LENGTH);
        printf("\n----waite recive data: \n");
        pi_wifi_data_get(&wifi, rx_buffer, 0);
        printf("----rx_buffer: (%s)---the data len:(%d)\n", rx_buffer,hc22->buffer_len);
        printf("----rx_buffer: \n");
        for(int i=0;i<hc22->buffer_len;i++)
        {
            printf("-(%x)\n", rx_buffer[i]);
        }

        printf("send a data:  !\n");
        printf("-----(%s)!\n",rx_buffer);
        pi_wifi_data_send(&wifi,rx_buffer,hc22->buffer_len);
    }


    printf("Exiting WIFI test\n");

    pmsis_exit(0);
}

int main(void)
{
    printf("Start HC08 WIFI module test\n");
    return pmsis_kickoff((void *) test_wifi);
}
