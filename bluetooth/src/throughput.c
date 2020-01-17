/******************** (C) COPYRIGHT 2016 STMicroelectronics ********************
* File Name          : throughput.c
* Author             : AMS - VMA RF  Application team
* Version            : V1.0.0
* Date               : 08-February-2016
* Description        : This file handles bytes received from USB and the init
*                      function. 
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include "gp_timer.h"
#include "ble_const.h" 
#include "bluenrg1_stack.h"
#include "app_state.h"
#include "throughput.h"
#include "osal.h"
#include "gatt_db.h"
#include "SDK_EVAL_Config.h"

/* External variables --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/

#define ADV_INTERVAL_MIN        200     // ms
#define ADV_INTERVAL_MAX        300     // ms

#if CLIENT
#define SELF_ADDRESS		0xAA, 0x00, 0x03, 0xE1, 0x80, 0x92
#define SERVER_ADDRESS	0xBB, 0x00, 0x03, 0xE1, 0x80, 0x92
#define ROLE						GAP_CENTRAL_ROLE
#else
#define SELF_ADDRESS		0xBB, 0x00, 0x03, 0xE1, 0x80, 0x92
#define CLIENT_ADDRESS	0xAA, 0x00, 0x03, 0xE1, 0x80, 0x92
#define ROLE						GAP_PERIPHERAL_ROLE
#endif

#define CMD_BUFF_SIZE 512

/* Private macros ------------------------------------------------------------*/ 

/* Private variables ---------------------------------------------------------*/
uint16_t tx_handle;
uint8_t* attr_value;
uint8_t length;

uint8_t connInfo[20];
volatile int app_flags = SET_CONNECTABLE;
volatile uint16_t connection_handle = 0;

/* UUIDs */
UUID_t UUID_Tx;
UUID_t UUID_Rx;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : THROUGHPUT_DeviceInit.
* Description    : Init the throughput test.
* Input          : none.
* Return         : Status.
*******************************************************************************/
uint8_t THROUGHPUT_DeviceInit(void)
{
  uint8_t ret;
  uint16_t service_handle;
  uint16_t dev_name_char_handle;
  uint16_t appearance_char_handle;
  uint8_t name[] = {'B', 'l', 'u', 'e', 'N', 'R', 'G', '1'};
  
  uint8_t role = ROLE;
  uint8_t bdaddr[] = {SELF_ADDRESS};
  
  /* Configure Public address */
  ret = aci_hal_write_config_data(CONFIG_DATA_PUBADDR_OFFSET, CONFIG_DATA_PUBADDR_LEN, bdaddr);
  if(ret != BLE_STATUS_SUCCESS){
    printf("Setting BD_ADDR failed: 0x%02x\r\n", ret);
    return ret;
  }

  /* Set the TX power to -2 dBm */
  aci_hal_set_tx_power_level(1, 4);

  /* GATT Init */
  ret = aci_gatt_init();
  if (ret != BLE_STATUS_SUCCESS) {
    printf ("Error in aci_gatt_init(): 0x%02x\r\n", ret);
    return ret;
  } else {
    printf ("aci_gatt_init() --> SUCCESS\r\n");
  }
  
  /* GAP Init */
  ret = aci_gap_init(role, 0x00, 0x08, &service_handle, 
                     &dev_name_char_handle, &appearance_char_handle);
  if (ret != BLE_STATUS_SUCCESS) {
    printf ("Error in aci_gap_init() 0x%02x\r\n", ret);
    return ret;
  } else {
    printf ("aci_gap_init() --> SUCCESS\r\n");
  }

  /* Set the device name */
  ret = aci_gatt_update_char_value_ext(0, service_handle, dev_name_char_handle, 0, sizeof(name), 0, sizeof(name), name);
  if (ret != BLE_STATUS_SUCCESS) {
    printf ("Error in Gatt Update characteristic value 0x%02x\r\n", ret);
    return ret;
  } else {
    printf ("aci_gatt_update_char_value_ext() --> SUCCESS\r\n");
  }

  ret = Add_Throughput_Service();
  if (ret != BLE_STATUS_SUCCESS) {
    printf("Error in Add_Throughput_Service 0x%02x\r\n", ret);
    return ret;
  } else {
    printf("Add_Throughput_Service() --> SUCCESS\r\n");
  } 
  return BLE_STATUS_SUCCESS;
}

/*******************************************************************************
* Function Name  : Make_Connection.
* Description    : If the device is a Client create the connection. Otherwise puts
*                  the device in discoverable mode.
* Input          : none.
* Return         : none.
*******************************************************************************/
void Make_Connection(void)
{  
  tBleStatus ret;
  #if CLIENT
	//tBDAddr bdaddr = {SERVER_ADDRESS};
	tBDAddr bdaddr = {0xBB, 0x00, 0x03, 0xE1, 0x80, 0x92};
	ret = aci_gap_create_connection(0x0004, 0x0004, PUBLIC_ADDR, bdaddr, PUBLIC_ADDR, 0x0006, 0x0006, 0, 60, 2000 , 2000); 
	if (ret != BLE_STATUS_SUCCESS)
	{
		printf("Error while starting connection: 0x%04x\r\n", ret);
			Clock_Wait(100);        
	}
	#else
		//uint8_t local_name[] = {AD_TYPE_COMPLETE_LOCAL_NAME,'B','l','u','e','C','a','r','d','i','o'};
		uint8_t local_name[] = {AD_TYPE_COMPLETE_LOCAL_NAME, 'A', 'U', 'R', 'O', 'R', 'A', ' ', 'C', 'A', 'R', 'D', 'I', 'O'};
		/* disable scan response */
		hci_le_set_scan_response_data(0,NULL);
		
		ret = aci_gap_set_discoverable(ADV_IND, 0, 0, PUBLIC_ADDR, NO_WHITE_LIST_USE,
																	 sizeof(local_name), local_name, 0, NULL, 0, 0);
		if (ret != BLE_STATUS_SUCCESS)
			printf ("Error in aci_gap_set_discoverable(): 0x%02x\r\n", ret);
		else
			printf ("aci_gap_set_discoverable() --> SUCCESS\r\n");
	#endif
}

BOOL search_switch = FALSE;
/*******************************************************************************
* Function Name  : APP_Tick.
* Description    : Tick to run the application state machine.
* Input          : none.
* Return         : none.
*******************************************************************************/
 void APP_Tick(void (*fptr_while_connected)(void))
{
  tBleStatus ret;
  
  if(APP_FLAG(SET_CONNECTABLE))
  {
    Make_Connection();
    APP_FLAG_CLEAR(SET_CONNECTABLE);
  }
	
  if (APP_FLAG(CONNECTED) && !APP_FLAG(END_READ_TX_CHAR_HANDLE))
  {
    if (!APP_FLAG(START_READ_TX_CHAR_HANDLE))
    {
      /* Discovery TX characteristic handle by UUID 128 bits */
			printf("Discovering Characteristcs...\r\n");
      const uint8_t charUuid128_TX[16] = {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0x01,0xf2,0x73,0xd9};

      Osal_MemCpy(&UUID_Tx.UUID_16, charUuid128_TX, 16);
      ret = aci_gatt_disc_char_by_uuid(connection_handle, 0x0001, 0xFFFF,UUID_TYPE_128,&UUID_Tx);
      if (ret != 0) 
        printf ("Error in aci_gatt_disc_char_by_uuid() for TX characteristic: 0x%04x\r\n", ret);
      else
        printf ("aci_gatt_disc_char_by_uuid() for TX characteristic --> SUCCESS\r\n");
      APP_FLAG_SET(START_READ_TX_CHAR_HANDLE);
    }
  }
	
	if(APP_FLAG(TX_BUFFER_FULL)){
		printf("tx buffer is full!\n");
		return;
	}
	
  if(APP_FLAG(CONNECTED) && APP_FLAG(END_READ_TX_CHAR_HANDLE) && !APP_FLAG(NOTIFICATIONS_ENABLED))
  {
		printf("Enabling notifications...\r\n");
    uint8_t client_char_conf_data[] = {0x01, 0x00}; // Enable notifications
    struct timer t;
    Timer_Set(&t, CLOCK_SECOND*10);
    
    while(aci_gatt_write_char_desc(connection_handle, tx_handle+2, 2, client_char_conf_data)==BLE_STATUS_NOT_ALLOWED){ //TX_HANDLE;
        printf("Radio module is busy\r\n");
        if(Timer_Expired(&t)) break;
    }
    APP_FLAG_SET(NOTIFICATIONS_ENABLED);
  } 
	
	if(APP_FLAG(CONNECTED))
	{
			if (fptr_while_connected)
				fptr_while_connected();
	}
  
}/* end APP_Tick() */



_Bool APP_UpdateTX(uint8_t *sendbuf, uint8_t size)
{
	if(APP_FLAG(CONNECTED))	{
		tBleStatus ret = aci_gatt_update_char_value_ext( 
													connection_handle, 
													ServHandle, 
													TXCharHandle, 
													1,		// 0x01: Notification
													size, // Char_Length Total length
													0,		//Value_Offset
													size, //length 
													sendbuf);
	
		if(ret != BLE_STATUS_SUCCESS){
			printf("Updating characteristic value failed! 0x%04x\r\n", ret);
			return FALSE;
		}
		else
		{
			printf("Upd: %s\r\n", (char*)sendbuf);
		}
		return TRUE;
	}
	return FALSE; // Yes, If we have not sent data - we say that false! it's logical
}
//

/* ***************** BlueNRG-1 Stack Callbacks ********************************/

/*******************************************************************************
 * Function Name  : hci_le_connection_complete_event.
 * Description    : This event indicates that a new connection has been created.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void hci_le_connection_complete_event(uint8_t Status,
                                      uint16_t Connection_Handle,
                                      uint8_t Role,
                                      uint8_t Peer_Address_Type,
                                      uint8_t Peer_Address[6],
                                      uint16_t Conn_Interval,
                                      uint16_t Conn_Latency,
                                      uint16_t Supervision_Timeout,
                                      uint8_t Master_Clock_Accuracy)

{ 
  connection_handle = Connection_Handle;
  
  APP_FLAG_SET(CONNECTED);
}/* end hci_le_connection_complete_event() */

/*******************************************************************************
 * Function Name  : hci_disconnection_complete_event.
 * Description    : This event occurs when a connection is terminated.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void hci_disconnection_complete_event(uint8_t Status,
                                      uint16_t Connection_Handle,
                                      uint8_t Reason)
{
   APP_FLAG_CLEAR(CONNECTED);
  /* Make the device connectable again. */
  APP_FLAG_SET(SET_CONNECTABLE);
  APP_FLAG_CLEAR(NOTIFICATIONS_ENABLED);
  APP_FLAG_CLEAR(TX_BUFFER_FULL);

  APP_FLAG_CLEAR(START_READ_TX_CHAR_HANDLE);
  APP_FLAG_CLEAR(END_READ_TX_CHAR_HANDLE);
  APP_FLAG_CLEAR(START_READ_RX_CHAR_HANDLE); 
  APP_FLAG_CLEAR(END_READ_RX_CHAR_HANDLE);

}/* end hci_disconnection_complete_event() */


/*******************************************************************************
 * Function Name  : aci_gatt_attribute_modified_event.
 * Description    : This event occurs when an attribute is modified.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void aci_gatt_attribute_modified_event(uint16_t Connection_Handle,
                                       uint16_t Attr_Handle,
                                       uint16_t Offset,
                                       uint16_t Attr_Data_Length,
                                       uint8_t Attr_Data[])
{
  Attribute_Modified_CB(Attr_Handle, Attr_Data_Length, Attr_Data);  
}

/*******************************************************************************
 * Function Name  : aci_gatt_tx_pool_available_event.
 * Description    : This event occurs when a TX pool available is received.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void aci_gatt_tx_pool_available_event(uint16_t Connection_Handle,
                                      uint16_t Available_Buffers)
{       
  /* It allows to notify when at least 2 GATT TX buffers are available */
  APP_FLAG_CLEAR(TX_BUFFER_FULL);
} 

/*******************************************************************************
 * Function Name  : aci_gatt_disc_read_char_by_uuid_resp_event.
 * Description    : This event occurs when a discovery read characteristic by UUID response.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void aci_gatt_disc_read_char_by_uuid_resp_event(uint16_t Connection_Handle,
                                                uint16_t Attribute_Handle,
                                                uint8_t Attribute_Value_Length,
                                                uint8_t Attribute_Value[])
{
  printf("aci_gatt_disc_read_char_by_uuid_resp_event, Connection Handle: 0x%04x\r\n", Connection_Handle);
  if (APP_FLAG(START_READ_TX_CHAR_HANDLE) && !APP_FLAG(END_READ_TX_CHAR_HANDLE))
  {
    tx_handle = Attribute_Handle;
    printf("TX Char Handle 0x%04x\r\n", tx_handle);
  }
}

/*******************************************************************************
 * Function Name  : aci_gatt_notification_event.
 * Description    : This event occurs when a notification is received.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void aci_gatt_notification_event(uint16_t Connection_Handle,
                                 uint16_t Attribute_Handle,
                                 uint8_t Attribute_Value_Length,
                                 uint8_t Attribute_Value[])
{ 
  uint16_t attr_handle;
  attr_value = Attribute_Value;
  length = Attribute_Value_Length;
  attr_handle = Attribute_Handle;
	
    if(attr_handle == tx_handle+1)
    {
			printf("Rec:");
      for(int i = 0; i < Attribute_Value_Length; i++) 
          printf("%c", Attribute_Value[i]);
			printf("\r\n");
    }
}

/*******************************************************************************
 * Function Name  : aci_gatt_proc_complete_event.
 * Description    : This event occurs when a GATT procedure complete is received.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void aci_gatt_proc_complete_event(uint16_t Connection_Handle,
                                  uint8_t Error_Code)
{ 
  if (APP_FLAG(START_READ_TX_CHAR_HANDLE) && !APP_FLAG(END_READ_TX_CHAR_HANDLE))
  {
    printf("aci_GATT_PROCEDURE_COMPLETE_Event for START_READ_TX_CHAR_HANDLE \r\n");
    APP_FLAG_SET(END_READ_TX_CHAR_HANDLE);
  }
  else if (APP_FLAG(START_READ_RX_CHAR_HANDLE) && !APP_FLAG(END_READ_RX_CHAR_HANDLE))
  {
    printf("aci_GATT_PROCEDURE_COMPLETE_Event for START_READ_TX_CHAR_HANDLE \r\n");
    APP_FLAG_SET(END_READ_RX_CHAR_HANDLE);
  }
}

