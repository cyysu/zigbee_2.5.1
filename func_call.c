// �ڵ�����
zgDeviceLogicalType
	ZG_DEVICETYPE_COORDINATOR
	ZG_DEVICETYPE_ROUTER
	ZG_DEVICETYPE_ENDDEVICE


// LED��
/* LED1~LED4 -> P1_0~P1-3 */
HalLedBlink(HAL_LED_1, 0, 90, 500);			/* ����Ϊ1s, 90%��ʱ���� */
HalLedSet(HAL_LED_1, HAL_LED_MODE_ON);		/* ���� */
HalLedSet(HAL_LED_1, HAL_LED_MODE_OFF);		/* �ص� */
HalLedSet(HAL_LED_1, HAL_LED_MODE_TOGGLE);	/* ��ת */				// HAL_TOGGLE_LED1();


// ���͹㲥��Ϣ
/* �㲥��ַ */
afAddrType_t SampleApp_Broadcast_DstAddr;
SampleApp_Broadcast_DstAddr.addrMode = (afAddrMode_t)AddrBroadcast;
SampleApp_Broadcast_DstAddr.endPoint = SAMPLEAPP_ENDPOINT;
SampleApp_Broadcast_DstAddr.addr.shortAddr = 0xFFFF;
/* Ӧ�ö���(�ն�������) */
endPointDesc_t SampleApp_epDesc;
SampleApp_epDesc.endPoint = SAMPLEAPP_ENDPOINT;
SampleApp_epDesc.task_id = &SampleApp_TaskID;
SampleApp_epDesc.simpleDesc = (SimpleDescriptionFormat_t *)&SampleApp_SimpleDesc;
SampleApp_epDesc.latencyReq = noLatencyReqs;		/* ����ʱ */
uint8 buffer[6] = "hello";
if (afStatus_SUCCESS == AF_DataRequest( &SampleApp_Broadcast_DstAddr, &SampleApp_epDesc,
					SAMPLEAPP_PERIODIC_CLUSTERID,
					1,
					buffer,
					&SampleApp_TransID,
					AF_DISCV_ROUTE,
					AF_DEFAULT_RADIUS ))


// ����/�˳���
aps_Group_t SampleApp_Group;
SampleApp_Group.ID = 0x0001;
osal_memcpy( SampleApp_Group.name, "Group 1", 7  );
aps_Group_t *grp = aps_FindGroup( SAMPLEAPP_ENDPOINT, SAMPLEAPP_FLASH_GROUP );
if ( grp )
{
	/* �˳��� */
	aps_RemoveGroup( SAMPLEAPP_ENDPOINT, SAMPLEAPP_FLASH_GROUP );
}
else
{
	/* ������ */
	aps_AddGroup( SAMPLEAPP_ENDPOINT, &SampleApp_Group );
}


// ������Ϣ����
afAddrType_t SampleApp_Group_DstAddr;
SampleApp_Group_DstAddr.addrMode = (afAddrMode_t)afAddrGroup;
SampleApp_Group_DstAddr.endPoint = SAMPLEAPP_ENDPOINT;
SampleApp_Group_DstAddr.addr.shortAddr = SAMPLEAPP_FLASH_GROUP;
uint8 buffer[6] = "hello";
if (afStatus_SUCCESS = AF_DataRequest( &SampleApp_Group_DstAddr, &SampleApp_epDesc,
                       SAMPLEAPP_FLASH_CLUSTERID,
                       6,
                       buffer,
                       &SampleApp_TransID,
                       AF_DISCV_ROUTE,
                       AF_DEFAULT_RADIUS ))


// ������ʱ���ж�(�жϷ�������Ҫˢ��)
#define SAMPLEAPP_SEND_PERIODIC_MSG_EVT       0x0001		///< ��ʱ���ж�events
#define SAMPLEAPP_SEND_PERIODIC_MSG_TIMEOUT   5000			///< ��ʱ����ʱʱ��
osal_start_timerEx( SampleApp_TaskID,
						SAMPLEAPP_SEND_PERIODIC_MSG_EVT,
						SAMPLEAPP_SEND_PERIODIC_MSG_TIMEOUT );


// Zigbee EP������
const SimpleDescriptionFormat_t SampleApp_SimpleDesc =
{
	SAMPLEAPP_ENDPOINT,              //  �ն˺�
	SAMPLEAPP_PROFID,                //  ����������ն���֧�ֵ�Profile ID������ID���� ID�����ѭ��ZigBee���˵ķ���
	SAMPLEAPP_DEVICEID,              //  �ն�֧�ֵ��豸ID��ID�����ѭZigBee���˵ķ���
	SAMPLEAPP_DEVICE_VERSION,        //  ���ն����豸ִ�е��豸�����İ汾��0x00ΪVersion 1.0
	SAMPLEAPP_FLAGS,                 //  int   AppFlags:4;
	SAMPLEAPP_MAX_CLUSTERS,          //  �ն�֧�ֵ��������Ŀ
	(cId_t *)SampleApp_ClusterList,  //  ָ������Cluster ID�б��ָ��
	SAMPLEAPP_MAX_CLUSTERS,          //  �ն�֧�ֵ��������Ŀ
	(cId_t *)SampleApp_ClusterList   //  ָ�����Cluster ID�б��ָ��
};
// �����ն����ն�������
endPointDesc_t *afFindEndPointDesc( byte endPoint );


// �����¼�����
XxxApp_ProcessEvent()
1. ϵͳ�¼� SYS_EVENT_MSG
��Ϣ
AF_DATA_CONFIRM_CMD			///< ���� AF_DataRequest()������������ɹ���ָʾ
AF_INCOMING_MSG_CMD			///< AF ��Ϣ����ָʾ
KEY_CHANGE					///< ���̶���ָʾ
ZDO_NEW_DSTADDR				///< ƥ��������������Ӧָʾ�����磺�Զ�ƥ�䣩
ZDO_STATE_CHANGE			///< ����״̬�ı�ָʾ(�豸�γɻ��������)


// �ַ�����������
sprintf(char *dst, char *format, ...)
int osal_strlen( char *pString )
void *osal_memcpy( void *dst, const void GENERIC *src, unsigned int len )
void *osal_memset( void *dest, uint8 value, int len )
uint8 osal_memcmp( const void GENERIC *src1, const void GENERIC *src2, unsigned int len )


// ����֧��
Alt+F7	C/C++Compiler/Preprocess
HAL_UART=TRUE		// ���������


// ����
LO_UINT16()
HI_UINT16()
BUILD_UINT16(��8λ, ��8λ)

LO_UINT8()
HI_UINT8()
BUILD_UINT8(��4λ, ��4λ)


/// ��[http://blog.csdn.net/tanqiuwei/article/details/7642716]
// 		�Զ���/������/�Զ�ƥ��
// ����: ��������Э�������ڣ������������ڵ�A��B���������ڵ���л�������(��A�ڵ��incluster��B�ڵ��outcluster)
// ������Ϣ���豸����ZDP_MatchDescReq()�㲥���˹���
// ƥ����豸���Զ�������Ӧ
// ��ZDO�������֤��Ӧ
// ��������Ϣ���豸�����󶨱�����󶨼�¼

//		������
// ����: �����в�һ����Э�������ڣ�������A��B��C��D�ȶ���ڵ㣬A������Outcluster��B��C��D��������Incluster�������ͨ��������������һ��ʱ��������B��C��D�е��κ�һ��������Match�Ĺ���
// �����ڵ�һ��ʱ��(16s)�ڶ���������ZDP_EndDeviceBindReq()����
