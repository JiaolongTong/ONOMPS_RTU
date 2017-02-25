// tcp-otdr.h
// Access OTDR using TCP  for Server mode
#ifndef _TCP_OTDR_H
#define _TCP_OTDR_H
#include "common.h"

/********************************* CMD Macro **************************************/
#define CMD_HOST_START_MEASURE              0x10000000 // 配置测试参数并启动测试
#define CMD_HOST_STOP_MEASURE               0x10000001 // 取消或终止测试
#define CMD_HOST_SET_IP                     0x10000002 // 设置 IP
#define CMD_HOST_NETWORK_IDLE               0x10000004 // 网络空闲
#define CMD_OTDR_UPDATE                     0x20000000 // OTDR 在线升级
#define CMD_DSP_UPLOAD_ALL_DATA             0x90000000 // 上传全部测试数据
#define CMD_DSP_UPLOAD_REF_DATA             0x90000001 // 上传刷新测试数据
#define CMD_RESPONSE_STATE 0xA0000000 // 返回状态码
/******************************* 帧格式定义 **************************************/
#define REV_ID                              0          // 版本号
#define RSVD_VALUE                          0xffffeeee // 帧类型
#define FRAMETYPE_HOST2TARGET               0          // 主控命令类型
#define FRAMETYPE_TARGET2HOST               1          // 响应命令类型
#define FRAME_SYNC_STRING                   "GLinkOtdr-3800M"  // 帧头标志

typedef struct                             //帧头
{
char FrameSync[16];                        // GLinkOtdr-3800M
uint32_t TotalLength;                      // 总帧长
uint32_t Rev;                              // 版本号
uint32_t FrameType;                        // 帧类型
uint32_t Src;                              // 源地址
uint32_t Dst;                              // 目的地址
uint32_t PacketID;                         // 流水号
uint32_t RSVD;                             // 保留
}frame_header_t; 

/**************************** 配置测试参数并启动测试*******************/
typedef struct                            
{
	uint32_t cmd;                              // 命令码  0x10000000
	uint32_t len;                              // 数据长度
	struct                                     // 控制部分
		{
			uint32_t OtdrMode;                     // OTDR 模式
			uint32_t OtdrOptMode;                  // OTDR 优化模式
			uint32_t RSVD;
			uint32_t EnableRefresh;                // 使能或禁止刷新
			uint32_t RefreshPeriod_ms;             // 刷新周期，单位 ms，不能小于 800
		}Ctrl;
	struct                                     // 状态部分
		{
			uint32_t Lambda_nm;                    // 工作光波长，单位 nm
			uint32_t MeasureLength_m;              // 测量长度，单位 m
			uint32_t PulseWidth_ns;                // 光脉冲宽度，单位 ns
			uint32_t MeasureTime_ms;               // 测量时间，单位 ms
			float n;                               // 折射率
			float EndThreshold;                    // 结束门限
			float NonRelectThreshold;              // 非反射门限
		}State;
	uint32_t RSVD;
}start_measure_t;
/****************************** 取消测试 *****************************/
typedef struct
{
		uint32_t cmd;                             // 命令码 0x10000001
		uint32_t len;                             // 数据长度
		// 控制方式
		uint32_t Cancel_Or_Abort;                 // 取消或者终止测试
		uint32_t RSVD;
}OTDR_Cancel_t;


typedef struct                            // IP 地址及子网掩码、默认网关
{
		uint32_t cmd;                             // 命令码 0x10000002
		uint32_t len;                             // 数据长度
		char LocalIPAddr[16];                     // IP
		char LocalIPMask[16];                     // 子网掩码
		char GatewayIP[16];                       // 网关
		uint32_t RSVD;        
}OTDR_IP_t;
/******************* OTDR 上传全部测试数据*********************/
#define DATA_LEN 32000
#define MAX_EVENT_NUM 500
#define RSVD_FLOAT 8192.0                    // 浮点数保留值，表明该值无法确定或无需确定
typedef struct
	{
		uint32_t cmd;                                // 命令码 0x90000000
		uint32_t len;                                // 数据长度
		struct                                       // 测试条件信息
		{
			uint32_t SampleRate_Hz;
			uint32_t MeasureLength_m;
			uint32_t PulseWidth_ns;
			uint32_t Lambda_nm;
			uint32_t MeasureTime_ms;
			float    n;
			float    FiberLength;                     // 光学长度
			float    FiberLoss;                       // 全程损耗
			float    FiberAttenCoef;                  // 链衰减系数
			float    NonRelectThreshold;              // 非反射门限
			float    EndThreshold;                    // 结束门限
			uint32_t OtdrMode;                        // OTDR 模式：实时、平均
			uint32_t MeasureMode;                     // 测试模式： 自动、手动
		}MeasureParam;
		struct                                        // 测试数据信息
		{
			uint32_t DataNum;
			uint16_t dB_x1000[DATA_LEN];
		}OtdrData;
		struct                                                           // 事件点信息单元
			{
				uint32_t EventNum;                                       // 事件点数目
				struct                                                   // 事件点
					{
						uint32_t EventXlabel;                            // 事件点在上传数据数组中的序号
						uint32_t EventType;                              // 事件点类型
						float    EventReflectLoss;                       // 反射损耗 / 回波损耗
						float    EventInsertLoss;                        // 插入损耗
						float    AttenCoef;                              // 与下一事件点之间衰减系数
						float    EventTotalLoss;                         // 事件点累计损耗
					}EventPoint[MAX_EVENT_NUM];
			}Event;
		uint32_t RSVD;                               //软件版本号         
	}otdr_result_t;
/********************** OTDR 上传刷新测试数据*****************/
typedef struct                               //
	{
		uint32_t cmd;                                // 命令码 0x90000001
		uint32_t len;                                // 数据长度

		struct                                       // 测试数据信息
			{
				uint32_t DataNum;
				uint16_t dB_x1000[DATA_LEN];
			}OtdrData;
		uint32_t RSVD;
	}otdr_ref_t;


/********************* OTDR 回帧命令数据结构 ***************/
#define STATE_CODE_CMD_OK                  0 // 成功处理命令
#define STATE_CODE_FRAME_SYNC_ERROR        1 // 非法帧起始字符串
#define STATE_CODE_REV_ERROR               2 // 非法版本号
#define STATE_CODE_FRAME_TYPE_ERROR        3 // 非法帧类型
#define STATE_CODE_CMD_ID_ERROR            4 // 非法命令标识
#define STATE_CODE_PACKET_LENTH_ERROR      5 // 非法数据长度
#define STATE_CODE_ML_OR_PW_ERROR          16 // 量程或脉宽非法
#define STATE_CODE_N_ERROR                 17 // 群折射率非法
#define STATE_CODE_NR_ERROR                18 // 非反射门限非法
#define STATE_CODE_OTDR_BUSY_MEASURE       19 // 非法请求测试
#define STATE_CODE_IP_ERROR                20 // 非法 IP 地址
/******************** 在线升级状态码   ********************/
#define STATE_CODE_FILE_CONTENT_ERROR      100 // 升级文件内容出错
#define STATE_CODE_OTDR_UPDATE_START       101 // 开始升级
#define STATE_CODE_OTDR_UPDATE_FAIL        102 // 升级失败
#define STATE_CODE_OTDR_UPDATE_DONE        103 // 升级完成
/******************** 回复状态数据   **************************/
typedef struct
	{
		uint32_t cmd;                      // 命令码 0xA0000000
		uint32_t len;                      // 数据长度
		uint32_t StateCode;                // 响应码
		uint32_t RSVD;
	}otdr_state_t;
#endif // _TCP_OTDR_H
