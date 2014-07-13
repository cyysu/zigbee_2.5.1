/**@file DemoApp.h
 * @note 2012-2015 HangZhou xxx Co., Ltd. All Right Reserved.
 * @brief 
 * 
 * @author 	 jiangpenghai
 * @date     2014/7/12
 * 
 * @note 
 * @note ��ʷ��¼: 
 * @note    2014/7/12 V1.0 jiangpenghai ����
 * 
 * @warning  
 */

#ifndef __DemoApp_H__
#define __DemoApp_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "./base_func/base_func.h"

#define DemoApp_ENDPOINT			20		///< LED1��EP��(1~240)

#define DemoApp_PROFID				0x0F08
#define DemoApp_DEVICEID			0x0001
#define DemoApp_DEVICE_VERSION		0
#define DemoApp_FLAGS				0

#define DemoApp_MAX_CLUSTERS		2		///< ���Ĵ���
#define DemoApp_1_CLUSTERID			1		///< ��1: �����¼�
#define DemoApp_2_CLUSTERID			2		///< ��2: Ƶ���¼�

#define DemoApp_SEND_PERIODIC_MSG_TIMEOUT	5000		///< ������Ϣ��������
#define DemoApp_SEND_PERIODIC_MSG_EVT		0x0001		///< ��ʱ���ж��¼���
#define DemoApp_GROUP_ID					0x0001

extern void DemoApp_Init( uint8 task_id );
extern UINT16 DemoApp_ProcessEvent( uint8 task_id, uint16 events );

#ifdef __cplusplus
}
#endif

#endif /* __DemoApp_H__ */

