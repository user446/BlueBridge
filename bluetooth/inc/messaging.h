#ifndef __MESSAGE_H
#define __MESSAGE_H

#include "BlueNRG1_conf.h"
#include "SDK_EVAL_Config.h"

static const uint8_t		COMMAND_PROPERTY_LEN		=	3;
static const uint16_t 	RESULT_MAX_LEN = 131;

//Protocol command											= {id, length};
static const uint8_t	IKM_ECG_TX[]						= {0x20, 0x20, 20};
static const uint8_t	IKM_ECG_LIVE[]					= {0x20, 0x21, 1};
static const uint8_t	IKM_DEVNAME[]						= {0x20, 0x22, 64};
static const uint8_t	IKM_DEVNUMBER[]					= {0x20, 0x23, 64};
static const uint8_t	IKM_USER_NAME[]					= {0x20, 0x24, 128};
static const uint8_t	IKM_USER_SNAME[]				= {0x20, 0x25, 128};
static const uint8_t	IKM_USER_PNAME[]				= {0x20, 0x26, 128};
static const uint8_t	IKM_USER_DATE[]					= {0x20, 0x27, 20};
static const uint8_t	IKM_USER_DOCTOR[]				= {0x20, 0x28, 128};
static const uint8_t	IKM_USER_HOSPITAL[]			= {0x20, 0x29, 128};
static const uint8_t	IKM_USER_COMMENT[]			= {0x20, 0x2A, 128};
static const uint8_t	IKM_USER_PHONE[]				= {0x20, 0x2B, 128};
static const uint8_t	IKM_FVT_ENABLE[]				= {0x20, 0x2C, 1};
static const uint8_t	IKM_FVT_ECG_REC[]				= {0x20, 0x2D, 1};
static const uint8_t	IKM_FVT_INTERVAL[]			= {0x20, 0x2E, 4};
static const uint8_t	IKM_FVT_DURATION[]			= {0x20, 0x2F, 4};
static const uint8_t	IKM_VT_ENABLE[]					= {0x20, 0x30, 1};
static const uint8_t	IKM_VT_ECG_REC[]				= {0x20, 0x31, 1};
static const uint8_t	IKM_VT_PERIOD[]					= {0x20, 0x32, 4};
static const uint8_t	IKM_VT_DURATION[]				= {0x20, 0x33, 2};
static const uint8_t	IKM_ASYST_ENABLE[]			= {0x20, 0x34, 1};
static const uint8_t	IKM_ASYST_ECG_REC[]			= {0x20, 0x35, 1};
static const uint8_t	IKM_ASYST_DURATION[]		= {0x20, 0x36, 4};
static const uint8_t	IKM_BRADY_ENABLE[]			= {0x20, 0x37, 2};
static const uint8_t	IKM_BRADY_ECG_REC[]			= {0x20, 0x38, 1};
static const uint8_t	IKM_BRADY_INTERVAL[]		= {0x20, 0x39, 1};
static const uint8_t	IKM_BRADY_DURATION[]		= {0x20, 0x3A, 2};
static const uint8_t	IKM_ATAF_ENABLE[]				= {0x20, 0x3B, 1};
static const uint8_t	IKM_ATAF_ECG_REC[]			= {0x20, 0x3C, 1};
static const uint8_t	IKM_ATAF_INTERVAL[]			= {0x20, 0x3D, 4};
static const uint8_t	IKM_ATAF_ECG_LIMIT[]		= {0x20, 0x3E, 2};
static const uint8_t	IKM_SYMPTOM_ENABLE[]		= {0x20, 0x3F, 1};
static const uint8_t	IKM_SYMPTOM_ECG_LIMIT[]	= {0x20, 0x40, 2};

static const uint8_t	MESSAGE_ERROR[]					=	{0xDE, 0xAD, 0};

_Bool make_a_message(const uint8_t command[COMMAND_PROPERTY_LEN], uint8_t* message, uint8_t len, uint8_t result[RESULT_MAX_LEN]);

#endif
