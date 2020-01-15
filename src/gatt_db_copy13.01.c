
#include <stdio.h>
#include <string.h>
#include "ble_const.h" 
#include "bluenrg1_stack.h"
#include "osal.h"
#include "app_state.h"
#include "SDK_EVAL_Config.h"
#include "clock.h"
#include "throughput.h"

#define COPY_UUID_128(uuid_struct, uuid_15, uuid_14, uuid_13, uuid_12, uuid_11, uuid_10, uuid_9, uuid_8, uuid_7, uuid_6, uuid_5, uuid_4, uuid_3, uuid_2, uuid_1, uuid_0) \
  do {\
  	uuid_struct.uuid128[0] = uuid_0; uuid_struct.uuid128[1] = uuid_1; uuid_struct.uuid128[2] = uuid_2; uuid_struct.uuid128[3] = uuid_3; \
	uuid_struct.uuid128[4] = uuid_4; uuid_struct.uuid128[5] = uuid_5; uuid_struct.uuid128[6] = uuid_6; uuid_struct.uuid128[7] = uuid_7; \
	uuid_struct.uuid128[8] = uuid_8; uuid_struct.uuid128[9] = uuid_9; uuid_struct.uuid128[10] = uuid_10; uuid_struct.uuid128[11] = uuid_11; \
	uuid_struct.uuid128[12] = uuid_12; uuid_struct.uuid128[13] = uuid_13; uuid_struct.uuid128[14] = uuid_14; uuid_struct.uuid128[15] = uuid_15; \
	}while(0)

uint16_t ServHandle;//, TXCharHandle, RXCharHandle;
uint16_t 
	IKM_ECG_TXCharHandle, 
	IKM_ECG_LIVECharHandle, 
	IKM_DEVNAMECharHandle, 
	IKM_DEVNUMBERCharHandle,
	IKM_USER_NAMECharHandle,
	IKM_USER_SNAMECharHandle,
	IKM_USER_PNAMECharHandle,
	IKM_USER_DATECharHandle,
	IKM_USER_DOCTORCharHandle,
	IKM_USER_HOSPITALCharHandle,
	IKM_USER_COMMENTCharHandle,
	IKM_USER_PHONECharHandle,
	IKM_FVT_ENABLECharHandle,
	IKM_FVT_ECG_RECCharHandle,
	IKM_FVT_INTERVALCharHandle,
	IKM_FVT_DURATIONCharHandle,
	IKM_VT_ENABLECharHandle,
	IKM_VT_ECG_RECCharHandle,
	IKM_VT_PERIODCharHandle,
	IKM_VT_DURATIONCharHandle,
	IKM_ASYST_ENABLECharHandle,
	IKM_ASYST_ECG_RECCharHandle,
	IKM_ASYST_DURATIONCharHandle,
	IKM_BRADY_ENABLECharHandle,
	IKM_BRADY_ECG_RECCharHandle,
	IKM_BRADY_INTERVALCharHandle,
	IKM_BRADY_DURATIONCharHandle,
	IKM_ATAF_ENABLECharHandle,
	IKM_ATAF_ECG_RECCharHandle,
	IKM_ATAF_INTERVALCharHandle,
	IKM_ATAF_ECG_LIMITCharHandle,
	IKM_SYMPTOM_ENABLECharHandle,
	IKM_SYMPTOM_ECG_LIMITCharHandle;

/* UUIDs */
Service_UUID_t service_uuid;
Char_UUID_t char_uuid;

/*******************************************************************************
* Function Name  : Add_Throughput_Service
* Description    : Add the 'Throughput' service.
* Input          : None
* Return         : Status.
*******************************************************************************/
uint8_t Add_Throughput_Service(void)
{
  uint8_t ret;

  /*
  UUIDs:
  D973F200-B19E-11E2-9E96-0800200C9A66
  D973F201-B19E-11E2-9E96-0800200C9A66
  D973F202-B19E-11E2-9E96-0800200C9A66
  */

  const uint8_t uuid[16]												= {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0x00,0xf2,0x73,0xd9};
  const uint8_t charUuid_ECG_TX[16]							= {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0x01,0xf2,0x73,0xd9};
  const uint8_t charUuid_ECG_LIVE[16]						= {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0x02,0xf2,0x73,0xd9};
  const uint8_t charUuid_DEVNAME[16]						= {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0x03,0xf2,0x73,0xd9};
  const uint8_t charUuid_DEVNUMBER[16]					= {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0x04,0xf2,0x73,0xd9};
  const uint8_t charUuid_USER_NAME[16]					= {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0x05,0xf2,0x73,0xd9};
  const uint8_t charUuid_USER_SNAME[16]					= {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0x06,0xf2,0x73,0xd9};
  const uint8_t charUuid_USER_PNAME[16]					= {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0x07,0xf2,0x73,0xd9};
  const uint8_t charUuid_USER_DATE[16]					= {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0x08,0xf2,0x73,0xd9};
  const uint8_t charUuid_USER_DOCTOR[16]				= {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0x09,0xf2,0x73,0xd9};
  const uint8_t charUuid_USER_HOSPITAL[16]			= {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0x0a,0xf2,0x73,0xd9};
  const uint8_t charUuid_USER_COMMENT[16]				= {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0x0b,0xf2,0x73,0xd9};
  const uint8_t charUuid_USER_PHONE[16]					= {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0x0c,0xf2,0x73,0xd9};
  const uint8_t charUuid_FVT_ENABLE[16]					= {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0x0d,0xf2,0x73,0xd9};
  const uint8_t charUuid_FVT_ECG_REC[16]				= {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0x0e,0xf2,0x73,0xd9};
  const uint8_t charUuid_FVT_INTERVAL[16]				= {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0x0f,0xf2,0x73,0xd9};
  const uint8_t charUuid_FVT_DURATION[16]				= {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0x10,0xf2,0x73,0xd9};
  const uint8_t charUuid_VT_ENABLE[16]					= {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0x11,0xf2,0x73,0xd9};
  const uint8_t charUuid_VT_ECG_REC[16]					= {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0x12,0xf2,0x73,0xd9};
	const uint8_t charUuid_VT_PERIOD[16]					= {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0x13,0xf2,0x73,0xd9};
  const uint8_t charUuid_VT_DURATION[16]				= {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0x14,0xf2,0x73,0xd9};
	const uint8_t charUuid_ASYST_ENABLE[16]				= {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0x15,0xf2,0x73,0xd9};
  const uint8_t charUuid_ASYST_ECG_REC[16]			= {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0x16,0xf2,0x73,0xd9};
	const uint8_t charUuid_ASYST_DURATION[16]			= {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0x17,0xf2,0x73,0xd9};
  const uint8_t charUuid_BRADY_ENABLE[16]				= {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0x18,0xf2,0x73,0xd9};
	const uint8_t charUuid_BRADY_ECG_REC[16]			= {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0x19,0xf2,0x73,0xd9};
  const uint8_t charUuid_BRADY_INTERVAL[16]			= {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0x1a,0xf2,0x73,0xd9};
	const uint8_t charUuid_BRADY_DURATION[16]			= {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0x1b,0xf2,0x73,0xd9};
  const uint8_t charUuid_ATAF_ENABLE[16] 				= {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0x1c,0xf2,0x73,0xd9};
	const uint8_t charUuid_ATAF_ECG_REC[16]				= {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0x1d,0xf2,0x73,0xd9};
	const uint8_t charUuid_ATAF_INTERVAL[16]			= {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0x1e,0xf2,0x73,0xd9};
	const uint8_t charUuid_ATAF_ECG_LIMIT[16]			= {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0x1f,0xf2,0x73,0xd9};
	const uint8_t charUuid_SYMPTOM_ENABLE[16]			= {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0x20,0xf2,0x73,0xd9};
	const uint8_t charUuid_SYMPTOM_ECG_LIMIT[16]	=	{0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0x21,0xf2,0x73,0xd9};


//  Osal_MemCpy(&service_uuid.Service_UUID_128, uuid, 16);
//  ret = aci_gatt_add_service(UUID_TYPE_128, &service_uuid, PRIMARY_SERVICE, 6, &ServHandle); 
//  if (ret != BLE_STATUS_SUCCESS) goto fail;  
	
	Osal_MemCpy(&service_uuid.Service_UUID_128, uuid, 16);
	ret = aci_gatt_add_service(UUID_TYPE_128, &service_uuid, PRIMARY_SERVICE, 99, &ServHandle);
	if (ret != BLE_STATUS_SUCCESS) goto fail;

	Osal_MemCpy(&char_uuid.Char_UUID_128, charUuid_ECG_TX, 16);
  ret =  aci_gatt_add_char(ServHandle, UUID_TYPE_128, &char_uuid, 20, CHAR_PROP_READ|CHAR_PROP_NOTIFY, ATTR_PERMISSION_NONE, 0,
                16, 1, &IKM_ECG_TXCharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;
	
	Osal_MemCpy(&char_uuid.Char_UUID_128, charUuid_ECG_LIVE, 16);
  ret =  aci_gatt_add_char(ServHandle, UUID_TYPE_128, &char_uuid, 1, CHAR_PROP_READ|CHAR_PROP_WRITE, ATTR_PERMISSION_NONE, 0,
                16, 1, &IKM_ECG_LIVECharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;
	
	Osal_MemCpy(&char_uuid.Char_UUID_128, charUuid_DEVNAME, 16);
  ret =  aci_gatt_add_char(ServHandle, UUID_TYPE_128, &char_uuid, 64, CHAR_PROP_READ, ATTR_PERMISSION_NONE, 0,
                16, 1, &IKM_DEVNAMECharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;
	
	Osal_MemCpy(&char_uuid.Char_UUID_128, charUuid_DEVNUMBER, 16);
  ret =  aci_gatt_add_char(ServHandle, UUID_TYPE_128, &char_uuid, 64, CHAR_PROP_READ, ATTR_PERMISSION_NONE, 0,
                16, 1, &IKM_DEVNUMBERCharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;

	Osal_MemCpy(&char_uuid.Char_UUID_128, charUuid_USER_NAME, 16);
  ret =  aci_gatt_add_char(ServHandle, UUID_TYPE_128, &char_uuid, 128, CHAR_PROP_READ, ATTR_PERMISSION_NONE, 0,
                16, 1, &IKM_USER_NAMECharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;
	
	Osal_MemCpy(&char_uuid.Char_UUID_128, charUuid_USER_SNAME, 16);
  ret =  aci_gatt_add_char(ServHandle, UUID_TYPE_128, &char_uuid, 128, CHAR_PROP_READ|CHAR_PROP_WRITE, ATTR_PERMISSION_NONE, 0,
                16, 1, &IKM_USER_SNAMECharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;
	
	Osal_MemCpy(&char_uuid.Char_UUID_128, charUuid_USER_PNAME, 16);
  ret =  aci_gatt_add_char(ServHandle, UUID_TYPE_128, &char_uuid, 128, CHAR_PROP_READ|CHAR_PROP_WRITE, ATTR_PERMISSION_NONE, 0,
                16, 1, &IKM_USER_PNAMECharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;
	
	Osal_MemCpy(&char_uuid.Char_UUID_128, charUuid_USER_DATE, 16);
  ret =  aci_gatt_add_char(ServHandle, UUID_TYPE_128, &char_uuid, 20, CHAR_PROP_READ|CHAR_PROP_WRITE, ATTR_PERMISSION_NONE, 0,
                16, 1, &IKM_USER_DATECharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;
	
	Osal_MemCpy(&char_uuid.Char_UUID_128, charUuid_USER_DOCTOR, 16);
  ret =  aci_gatt_add_char(ServHandle, UUID_TYPE_128, &char_uuid, 128, CHAR_PROP_READ|CHAR_PROP_WRITE, ATTR_PERMISSION_NONE, 0,
                16, 1, &IKM_USER_DOCTORCharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;
	
	Osal_MemCpy(&char_uuid.Char_UUID_128, charUuid_USER_HOSPITAL, 16);
  ret =  aci_gatt_add_char(ServHandle, UUID_TYPE_128, &char_uuid, 128, CHAR_PROP_READ|CHAR_PROP_WRITE, ATTR_PERMISSION_NONE, 0,
                16, 1, &IKM_USER_HOSPITALCharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;
	
	Osal_MemCpy(&char_uuid.Char_UUID_128, charUuid_USER_COMMENT, 16);
  ret =  aci_gatt_add_char(ServHandle, UUID_TYPE_128, &char_uuid, 128, CHAR_PROP_READ|CHAR_PROP_WRITE, ATTR_PERMISSION_NONE, 0,
                16, 1, &IKM_USER_COMMENTCharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;
	
	Osal_MemCpy(&char_uuid.Char_UUID_128, charUuid_USER_PHONE, 16);
  ret =  aci_gatt_add_char(ServHandle, UUID_TYPE_128, &char_uuid, 128, CHAR_PROP_READ|CHAR_PROP_WRITE, ATTR_PERMISSION_NONE, 0,
                16, 1, &IKM_USER_PHONECharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;
	
	Osal_MemCpy(&char_uuid.Char_UUID_128, charUuid_FVT_ENABLE, 16);
  ret =  aci_gatt_add_char(ServHandle, UUID_TYPE_128, &char_uuid, 1, CHAR_PROP_READ|CHAR_PROP_WRITE, ATTR_PERMISSION_NONE, 0,
                16, 1, &IKM_FVT_ENABLECharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;
	
	Osal_MemCpy(&char_uuid.Char_UUID_128, charUuid_FVT_ECG_REC, 16);
  ret =  aci_gatt_add_char(ServHandle, UUID_TYPE_128, &char_uuid, 1, CHAR_PROP_READ|CHAR_PROP_WRITE, ATTR_PERMISSION_NONE, 0,
                16, 1, &IKM_FVT_ECG_RECCharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;
	
	Osal_MemCpy(&char_uuid.Char_UUID_128, charUuid_FVT_INTERVAL, 16);
  ret =  aci_gatt_add_char(ServHandle, UUID_TYPE_128, &char_uuid, 4, CHAR_PROP_READ|CHAR_PROP_WRITE, ATTR_PERMISSION_NONE, 0,
                16, 1, &IKM_FVT_INTERVALCharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;
	
	Osal_MemCpy(&char_uuid.Char_UUID_128, charUuid_FVT_DURATION, 16);
  ret =  aci_gatt_add_char(ServHandle, UUID_TYPE_128, &char_uuid, 4, CHAR_PROP_READ|CHAR_PROP_WRITE, ATTR_PERMISSION_NONE, 0,
                16, 1, &IKM_FVT_DURATIONCharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;

	Osal_MemCpy(&char_uuid.Char_UUID_128, charUuid_VT_ENABLE, 16);
  ret =  aci_gatt_add_char(ServHandle, UUID_TYPE_128, &char_uuid, 1, CHAR_PROP_READ|CHAR_PROP_WRITE, ATTR_PERMISSION_NONE, 0,
                16, 1, &IKM_VT_ENABLECharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;
	
	Osal_MemCpy(&char_uuid.Char_UUID_128, charUuid_VT_ECG_REC, 16);
  ret =  aci_gatt_add_char(ServHandle, UUID_TYPE_128, &char_uuid, 1, CHAR_PROP_READ|CHAR_PROP_WRITE, ATTR_PERMISSION_NONE, 0,
                16, 1, &IKM_VT_ECG_RECCharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;
	
	Osal_MemCpy(&char_uuid.Char_UUID_128, charUuid_VT_PERIOD, 16);
  ret =  aci_gatt_add_char(ServHandle, UUID_TYPE_128, &char_uuid, 4, CHAR_PROP_READ|CHAR_PROP_WRITE, ATTR_PERMISSION_NONE, 0,
                16, 1, &IKM_VT_PERIODCharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;
	
	Osal_MemCpy(&char_uuid.Char_UUID_128, charUuid_VT_DURATION, 16);
  ret =  aci_gatt_add_char(ServHandle, UUID_TYPE_128, &char_uuid, 2, CHAR_PROP_READ|CHAR_PROP_WRITE, ATTR_PERMISSION_NONE, 0,
                16, 1, &IKM_VT_DURATIONCharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;
	
	Osal_MemCpy(&char_uuid.Char_UUID_128, charUuid_BRADY_ENABLE, 16);
  ret =  aci_gatt_add_char(ServHandle, UUID_TYPE_128, &char_uuid, 1, CHAR_PROP_READ|CHAR_PROP_WRITE, ATTR_PERMISSION_NONE, 0,
                16, 1, &IKM_BRADY_ENABLECharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;
	
	Osal_MemCpy(&char_uuid.Char_UUID_128, charUuid_BRADY_ECG_REC, 16);
  ret =  aci_gatt_add_char(ServHandle, UUID_TYPE_128, &char_uuid, 1, CHAR_PROP_READ|CHAR_PROP_WRITE, ATTR_PERMISSION_NONE, 0,
                16, 1, &IKM_BRADY_ECG_RECCharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;
	
	Osal_MemCpy(&char_uuid.Char_UUID_128, charUuid_BRADY_INTERVAL, 16);
  ret =  aci_gatt_add_char(ServHandle, UUID_TYPE_128, &char_uuid, 4, CHAR_PROP_READ|CHAR_PROP_WRITE, ATTR_PERMISSION_NONE, 0,
                16, 1, &IKM_BRADY_INTERVALCharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;
	
	Osal_MemCpy(&char_uuid.Char_UUID_128, charUuid_BRADY_DURATION, 16);
  ret =  aci_gatt_add_char(ServHandle, UUID_TYPE_128, &char_uuid, 2, CHAR_PROP_READ|CHAR_PROP_WRITE, ATTR_PERMISSION_NONE, 0,
                16, 1, &IKM_BRADY_DURATIONCharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;
	
	Osal_MemCpy(&char_uuid.Char_UUID_128, charUuid_ASYST_ENABLE, 16);
  ret =  aci_gatt_add_char(ServHandle, UUID_TYPE_128, &char_uuid, 1, CHAR_PROP_READ|CHAR_PROP_WRITE, ATTR_PERMISSION_NONE, 0,
                16, 1, &IKM_ASYST_ENABLECharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;
	
	Osal_MemCpy(&char_uuid.Char_UUID_128, charUuid_ASYST_ECG_REC, 16);
  ret =  aci_gatt_add_char(ServHandle, UUID_TYPE_128, &char_uuid, 1, CHAR_PROP_READ|CHAR_PROP_WRITE, ATTR_PERMISSION_NONE, 0,
                16, 1, &IKM_ASYST_ECG_RECCharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;
	
	Osal_MemCpy(&char_uuid.Char_UUID_128, charUuid_ASYST_DURATION, 16);
  ret =  aci_gatt_add_char(ServHandle, UUID_TYPE_128, &char_uuid, 2, CHAR_PROP_READ|CHAR_PROP_WRITE, ATTR_PERMISSION_NONE, 0,
                16, 1, &IKM_ASYST_DURATIONCharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;
	
	Osal_MemCpy(&char_uuid.Char_UUID_128, charUuid_ATAF_ENABLE, 16);
  ret =  aci_gatt_add_char(ServHandle, UUID_TYPE_128, &char_uuid, 1, CHAR_PROP_READ|CHAR_PROP_WRITE, ATTR_PERMISSION_NONE, 0,
                16, 1, &IKM_ATAF_ENABLECharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;
	
	Osal_MemCpy(&char_uuid.Char_UUID_128, charUuid_ATAF_ECG_REC, 16);
  ret =  aci_gatt_add_char(ServHandle, UUID_TYPE_128, &char_uuid, 1, CHAR_PROP_READ|CHAR_PROP_WRITE, ATTR_PERMISSION_NONE, 0,
                16, 1, &IKM_ATAF_ECG_RECCharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;
	
	Osal_MemCpy(&char_uuid.Char_UUID_128, charUuid_ATAF_INTERVAL, 16);
  ret =  aci_gatt_add_char(ServHandle, UUID_TYPE_128, &char_uuid, 4, CHAR_PROP_READ|CHAR_PROP_WRITE, ATTR_PERMISSION_NONE, 0,
                16, 1, &IKM_ATAF_INTERVALCharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;
	
	Osal_MemCpy(&char_uuid.Char_UUID_128, charUuid_ATAF_ECG_LIMIT, 16);
  ret =  aci_gatt_add_char(ServHandle, UUID_TYPE_128, &char_uuid, 2, CHAR_PROP_READ|CHAR_PROP_WRITE, ATTR_PERMISSION_NONE, 0,
                16, 1, &IKM_ATAF_ECG_LIMITCharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;
	
	Osal_MemCpy(&char_uuid.Char_UUID_128, charUuid_SYMPTOM_ENABLE, 16);
  ret =  aci_gatt_add_char(ServHandle, UUID_TYPE_128, &char_uuid, 1, CHAR_PROP_READ|CHAR_PROP_WRITE, ATTR_PERMISSION_NONE, 0,
                16, 1, &IKM_SYMPTOM_ENABLECharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;

	Osal_MemCpy(&char_uuid.Char_UUID_128, charUuid_SYMPTOM_ECG_LIMIT, 16);
  ret =  aci_gatt_add_char(ServHandle, UUID_TYPE_128, &char_uuid, 2, CHAR_PROP_READ|CHAR_PROP_WRITE, ATTR_PERMISSION_NONE, 0,
                16, 1, &IKM_SYMPTOM_ECG_LIMITCharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;
//  printf("Chat Service added.\nTX Char Handle %04X, RX Char Handle %04X\n", TXCharHandle, RXCharHandle);
  return BLE_STATUS_SUCCESS; 

fail:
  printf("Error while adding Chat service.\n");
  return BLE_STATUS_ERROR ;
}

/*******************************************************************************
* Function Name  : Attribute_Modified_CB
* Description    : Callback when RX/TX attribute handle is modified.
* Input          : Handle of the characteristic modified. Handle of the attribute
*                  modified and data written
* Return         : None.
*******************************************************************************/
void Attribute_Modified_CB(uint16_t handle, uint8_t data_length, uint8_t *att_data)
{
//  if(handle == RXCharHandle + 1){
//    for(int i = 0; i < data_length; i++)
//      printf("%c", att_data[i]);
//  }
//  else if(handle == TXCharHandle + 2){        
//    if(att_data[0] == 0x01)
//      APP_FLAG_SET(NOTIFICATIONS_ENABLED);
//  }
//  else {
//    printf("Unexpected handle: 0x%04d.\n",handle);
//  }
}


