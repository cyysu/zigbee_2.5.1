/**@file DemoApp.c
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

#include "OSAL.h"
#include "ZGlobals.h"
#include "AF.h"
#include "aps_groups.h"
#include "ZDApp.h"

#include "DemoApp.h"

#include "OnBoard.h"

#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"


const cId_t DemoApp_ClusterList[DemoApp_MAX_CLUSTERS] =
{
	DemoApp_1_CLUSTERID,
	DemoApp_2_CLUSTERID
};

const SimpleDescriptionFormat_t DemoApp_SimpleDesc =
{
	DemoApp_ENDPOINT,
	DemoApp_PROFID,
	DemoApp_DEVICEID,
	DemoApp_DEVICE_VERSION,
	DemoApp_FLAGS,
	DemoApp_MAX_CLUSTERS,
	(cId_t *)DemoApp_ClusterList,
	DemoApp_MAX_CLUSTERS,
	(cId_t *)DemoApp_ClusterList
};

uint8 DemoApp_TaskID;
endPointDesc_t DemoApp_epDesc;
devStates_t DemoApp_NwkState;
uint8 DemoApp_TransID;

afAddrType_t DemoApp_Broadcast_DstAddr;
afAddrType_t DemoApp_Group_DstAddr;

aps_Group_t DemoApp_Group;

uint8 DemoAppFlashCounter = 0;

void DemoApp_HandleKeys(uint8 shift, uint8 keys);
void DemoApp_MessageMSGCB(afIncomingMSGPacket_t *pckt);
void DemoApp_SendBroadcastMessage( void );
void DemoApp_SendMulticastMessage();

void DemoApp_UartRecv(void);
void DemoApp_UartCallBack(uint8 port, uint8 event);

/**
 * @brief		EP�����ʼ��
 * @param[in]	����ID
 * @param[out] 	void
 * @return		void
 */
void DemoApp_Init(uint8 task_id)
{
	DemoApp_TaskID = task_id;
	DemoApp_NwkState = DEV_INIT;
	DemoApp_TransID = 0;

	/* ���EP������ */
	DemoApp_epDesc.endPoint = DemoApp_ENDPOINT;
	DemoApp_epDesc.task_id = &DemoApp_TaskID;
	DemoApp_epDesc.simpleDesc = (SimpleDescriptionFormat_t *)&DemoApp_SimpleDesc;
	DemoApp_epDesc.latencyReq = noLatencyReqs;

	/* ��AF��Ǽ�EP���� */
	afRegister(&DemoApp_epDesc);

	/* ע�ᰴ����Ϣ */
	RegisterForKeys(DemoApp_TaskID);

	/* ��ʼ������ */
	uart_init(DemoApp_UartCallBack);
	uint8 *send_buf = "SerialApp_Init\n";  
	HalUARTWrite(SERIAL_APP_PORT, send_buf, osal_strlen((char *)send_buf));

	/* ��ʼ���㲥��ַ */
	DemoApp_Broadcast_DstAddr.addrMode = (afAddrMode_t)AddrBroadcast;
	DemoApp_Broadcast_DstAddr.endPoint = DemoApp_ENDPOINT;
	DemoApp_Broadcast_DstAddr.addr.shortAddr = NWK_BROADCAST_SHORTADDR;
	/* ��ʼ�����ַ */
	DemoApp_Group_DstAddr.addrMode = (afAddrMode_t)afAddrGroup;
	DemoApp_Group_DstAddr.endPoint = DemoApp_ENDPOINT;
	DemoApp_Group_DstAddr.addr.shortAddr = DemoApp_GROUP_ID;

	/* �豸Ĭ�ϼӵ�Group 1 */
	DemoApp_Group.ID = DemoApp_GROUP_ID;
	osal_memcpy(DemoApp_Group.name, "Group 1", 7 );
	aps_AddGroup(DemoApp_ENDPOINT, &DemoApp_Group);
}

/**
 * @brief		EP�����¼�������
 * @param[in]	task_id: 	EP����ID
 * @param[in]	events: 	EP�����¼�
 * @param[out] 	void
 * @return		δ������¼�
 */
uint16 DemoApp_ProcessEvent(uint8 task_id, uint16 events)
{
	afIncomingMSGPacket_t *MSGpkt;

	afDataConfirm_t *afDataConfirm;
	ZStatus_t sentStatus;

	/* ϵͳ�¼� */
	if ( events & SYS_EVENT_MSG )
	{
		MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive(DemoApp_TaskID);
		while (MSGpkt)
		{
			switch (MSGpkt->hdr.event)
			{
				/* ������Ϣ */
				case KEY_CHANGE:
					DemoApp_HandleKeys(((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys);
					break;

				/* RF���յ����� */
				case AF_INCOMING_MSG_CMD:
					DemoApp_MessageMSGCB(MSGpkt);
					break;

				/* ��������״̬�ı� */
				case ZDO_STATE_CHANGE:
					DemoApp_NwkState = (devStates_t)(MSGpkt->hdr.status);

					if (DEV_ZB_COORD == DemoApp_NwkState)
					{
						HalLedBlink(HAL_LED_1, 1, 90, 500);
						osal_start_timerEx(DemoApp_TaskID,
					          DemoApp_SEND_PERIODIC_MSG_EVT,
					          DemoApp_SEND_PERIODIC_MSG_TIMEOUT);
					}
					else if (DEV_ROUTER == DemoApp_NwkState)
					{
						HalLedBlink(HAL_LED_1, 2, 80, 500);
					}
					else if (DEV_END_DEVICE == DemoApp_NwkState)
					{
						HalLedBlink(HAL_LED_1, 3, 70, 500);
					}
					break;

				/* ƥ��������������Ӧ,���Զ�ƥ�� */
				case ZDO_NEW_DSTADDR:
					break;

				/* ���� AF_DataRequest()����������������ɹ���ָʾ */
				case AF_DATA_CONFIRM_CMD:
					afDataConfirm = (afDataConfirm_t *)MSGpkt;
					sentStatus = afDataConfirm->hdr.status;
					if ( sentStatus != ZSuccess )
					{
						;		// ����û�з��ͳɹ�
					}
					break;
					
				default:
					break;
			}

			osal_msg_deallocate((uint8 *)MSGpkt);
			MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( DemoApp_TaskID );
		}

		return (events ^ SYS_EVENT_MSG);
	}

	/* ��ʱ���ж��¼� */
	if (events & DemoApp_SEND_PERIODIC_MSG_EVT)
	{
		DemoApp_SendBroadcastMessage();

		/* ���ö�ʱ�� */
		osal_start_timerEx(DemoApp_TaskID, DemoApp_SEND_PERIODIC_MSG_EVT,
			(DemoApp_SEND_PERIODIC_MSG_TIMEOUT + (osal_rand() & 0x00FF)));

		return (events ^ DemoApp_SEND_PERIODIC_MSG_EVT);
	}

	return 0;
}

/**
 * @brief		������Ϣ������
 * @param[in]	shift: 	˫��������
 * @param[in]	keys: 	����������
 * @param[out] 	void
 * @return		void
 */
void DemoApp_HandleKeys(uint8 shift, uint8 keys)
{
	uint8 send_buf[5] = {0};  

	if (keys & HAL_KEY_SW_6)
	{
		HalLedBlink(HAL_LED_1, 1, 10, 500);
		osal_memcpy(send_buf, "SW_6", osal_strlen("SW_6"));
		HalUARTWrite(SERIAL_APP_PORT, send_buf, osal_strlen((char *)send_buf));
	}
}

/**
 * @brief		RF���ݽ��մ�����
 * @param[in]	pkt: 	RF���ݰ�
 * @param[out] 	void
 * @return		void
 */
void DemoApp_MessageMSGCB(afIncomingMSGPacket_t *pkt)
{
	uint8 send_buf[5] = {0};

	switch (pkt->clusterId)
	{
		case DemoApp_1_CLUSTERID:
			/* �������յ��㲥���� */
			HalLedBlink(HAL_LED_1, 1, 10, 500);
			
			osal_memcpy(send_buf, pkt->cmd.Data, pkt->cmd.DataLength);
			HalUARTWrite(SERIAL_APP_PORT, send_buf, osal_strlen((char *)send_buf));
			break;

		case DemoApp_2_CLUSTERID:
			/* �������յ��ಥ���� */
			HalLedBlink(HAL_LED_1, 1, 10, 500);
			
			osal_memcpy(send_buf, pkt->cmd.Data, pkt->cmd.DataLength);
			HalUARTWrite(SERIAL_APP_PORT, send_buf, osal_strlen((char *)send_buf));
			break;
	}
}

/**
 * @brief		�����Է�����Ϣ
 * @param[in]	void
 * @param[out] 	void
 * @return		void
 */
void DemoApp_SendPeriodicMessage(void)
{
	DemoApp_SendMulticastMessage();

	return;
}

/**
 * @brief		���͹㲥��Ϣ
 * @param[in]	void
 * @param[out] 	void
 * @return		void
 */
void DemoApp_SendBroadcastMessage()
{
	uint8 *sendBuf = "{1}";

	if (afStatus_SUCCESS == AF_DataRequest(&DemoApp_Broadcast_DstAddr, &DemoApp_epDesc,
							DemoApp_1_CLUSTERID,
							3,
							sendBuf,
							&DemoApp_TransID,
							AF_DISCV_ROUTE,
							AF_DEFAULT_RADIUS))
	{
		HalLedBlink(HAL_LED_1, 1, 5, 500);
	}
	else
	{
		;
	}

	return;
}

/**
 * @brief		�����鲥��Ϣ
 * @param[in]	void
 * @param[out] 	void
 * @return		void
 */
void DemoApp_SendMulticastMessage()
{
	uint8 *sendBuf = "{1}";

	if (afStatus_SUCCESS == AF_DataRequest(&DemoApp_Group_DstAddr, &DemoApp_epDesc,
							DemoApp_2_CLUSTERID,
							3,
							sendBuf,
							&DemoApp_TransID,
							AF_DISCV_ROUTE,
							AF_DEFAULT_RADIUS))
	{
		HalLedBlink(HAL_LED_1, 1, 5, 500);
	}
	else
	{
		;
	}

	return;
}

/**
 * @brief		���ڽ��պ���
 * @param[in]	void
 * @param[out] 	void
 * @return		void
 */
void DemoApp_UartRecv(void)
{
	uint8 rx_buffer[SERIAL_APP_TX_MAX+1] = {0};
	uint8 tx_buffer[SERIAL_APP_TX_MAX+1] = {0};
	uint16 rx_len = 0;

	if (rx_len = HalUARTRead(SERIAL_APP_PORT, rx_buffer, SERIAL_APP_TX_MAX))
	{
		sprintf((char *)tx_buffer, "Recv %d bytes: %s\n", rx_len, (char *)rx_buffer);
		HalUARTWrite(SERIAL_APP_PORT, tx_buffer, osal_strlen((char *)tx_buffer));
	}

	return;
}

/**
 * @brief		���ڻص�����
 * @param[in]	port:	
 * @param[in]	event:	�����¼�
 * @param[out] 	void
 * @return		void
 */
void DemoApp_UartCallBack(uint8 port, uint8 event)
{
	if (event & (HAL_UART_RX_FULL | HAL_UART_RX_ABOUT_FULL | HAL_UART_RX_TIMEOUT))
	{
		DemoApp_UartRecv();
	}

	return;
}

