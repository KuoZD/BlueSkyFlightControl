/**********************************************************************************************************
                                天穹飞控 —— 致力于打造中国最好的多旋翼开源飞控
                                Github: github.com/loveuav/BlueSkyFlightControl
                                技术讨论：bbs.loveuav.com/forum-68-1.html
 * @文件     messageDecoded.c
 * @说明     接收数据解析
 * @版本  	 V1.0
 * @作者     BlueSky
 * @网站     bbs.loveuav.com
 * @日期     2018.06
**********************************************************************************************************/
#include "messageDecode.h"
#include "message.h"
#include "bsklink.h"
#include <string.h>

#include "sensor.h"
#include "gyroscope.h"
#include "magnetometer.h"
#include "accelerometer.h"
#include "barometer.h"
#include "gps.h"
#include "board.h"
#include "ahrs.h"
#include "navigation.h"
#include "flightControl.h"
#include "pid.h"
#include "motor.h"
#include "rc.h"
#include "ublox.h"
#include "flightStatus.h"

static void BsklinkDecodeSensorCaliCmd(BSKLINK_PAYLOAD_SENSOR_CALI_CMD_t payload);
static void BsklinkDecodeSetAttPid(BSKLINK_PAYLOAD_PID_ATT_t payload);
static void BsklinkDecodeSetPosPid(BSKLINK_PAYLOAD_PID_POS_t payload);

/**********************************************************************************************************
*函 数 名: MessageDecode
*功能说明: 消息解析
*形    参: 接收数据
*返 回 值: 无
**********************************************************************************************************/
void MessageDecode(uint8_t data)
{
    static BSKLINK_MSG_t msg;
    static uint32_t i=0;
    
    //对接收到的字节数据进行帧解析，接收完一帧时再继续对帧数据进行解析
    if(BsklinkDecode(&msg, data) == false)
        return;
    
    if(msg.msgid == BSKLINK_MSG_ID_FLIGHT_DATA)
    {
        i++;
    }
    else if(msg.msgid == BSKLINK_MSG_ID_SENSOR_CALI_CMD)    //传感器校准命令解析
    {
        BSKLINK_PAYLOAD_SENSOR_CALI_CMD_t payload;
        memcpy(&payload, msg.payload, msg.length);
        BsklinkDecodeSensorCaliCmd(payload);
    }
    else if(msg.msgid == BSKLINK_MSG_ID_PID_ATT)            //姿态PID解析
    {
        if(msg.length == 0)
        {
            //往地面站发送PID参数
            MessageSendEnable(BSKLINK_MSG_ID_PID_ATT);
        }
        else
        {
            BSKLINK_PAYLOAD_PID_ATT_t payload;
            memcpy(&payload, msg.payload, msg.length);
            //设置姿态PID参数
            BsklinkDecodeSetAttPid(payload);                
        }
    }
    else if(msg.msgid == BSKLINK_MSG_ID_PID_POS)            //位置PID解析
    {
        if(msg.length == 0)
        {
            //往地面站发送PID参数
            MessageSendEnable(BSKLINK_MSG_ID_PID_POS);
        }
        else
        {
            BSKLINK_PAYLOAD_PID_POS_t payload;
            memcpy(&payload, msg.payload, msg.length);
            //设置位置PID参数
            BsklinkDecodeSetPosPid(payload);                
        }
    }
}

/**********************************************************************************************************
*函 数 名: BsklinkDecodeSensorCaliCmd
*功能说明: 传感器校准命令解析
*形    参: 消息
*返 回 值: 无
**********************************************************************************************************/
static void BsklinkDecodeSensorCaliCmd(BSKLINK_PAYLOAD_SENSOR_CALI_CMD_t payload)
{
    if(payload.type == GYRO)			//陀螺仪
    {
        if(payload.caliFlag == true)
        {
            GyroCalibrateEnable();
        }
    }
    else if(payload.type == ACC)		//加速度计
    {
		if(payload.caliFlag == true)
        {
			AccCalibrateEnable();
		}
    }
    else if(payload.type == MAG)		//磁力计
    {
        if(payload.caliFlag == true)
        {
            MagCalibrateEnable();
        }
    }
    else if(payload.type == ANGLE)		//水平
    {
		if(payload.caliFlag == true)
        {
			LevelCalibrateEnable();
		}
    }
    else if(payload.type == ESC)		//电调
    {
    }
}

/**********************************************************************************************************
*函 数 名: BsklinkDecodeSetAttPid
*功能说明: 姿态PID解析
*形    参: 消息
*返 回 值: 无
**********************************************************************************************************/
static void BsklinkDecodeSetAttPid(BSKLINK_PAYLOAD_PID_ATT_t payload)
{
    PID_t pid;
    
    //横滚角速度PID
    pid.kP = payload.roll_kp;
    pid.kI = payload.roll_ki;
    pid.kD = payload.roll_kd;
    FcSetPID(ROLL_INNER, pid);
    //俯仰角速度PID
    pid.kP = payload.pitch_kp;
    pid.kI = payload.pitch_ki;
    pid.kD = payload.pitch_kd;
    FcSetPID(PITCH_INNER, pid);
    //偏航角速度PID
    pid.kP = payload.yaw_kp;
    pid.kI = payload.yaw_ki;
    pid.kD = payload.yaw_kd;
    FcSetPID(YAW_INNER, pid);
    //横滚角度PID
    pid.kP = payload.rollAngle_kp;
    pid.kI = 0;
    pid.kD = 0;
    FcSetPID(ROLL_OUTER, pid);
    //俯仰角度PID
    pid.kP = payload.pitchAngle_kp;
    FcSetPID(PITCH_OUTER, pid);
    //偏航角度PID
    pid.kP = payload.yawAngle_kp;
    FcSetPID(YAW_OUTER, pid);
}    
    
/**********************************************************************************************************
*函 数 名: BsklinkDecodeSetPosPid
*功能说明: 姿态PID解析
*形    参: 消息
*返 回 值: 无
**********************************************************************************************************/
static void BsklinkDecodeSetPosPid(BSKLINK_PAYLOAD_PID_POS_t payload)
{
    PID_t pid;
    
    //X轴速度PID
    pid.kP = payload.velX_kp;
    pid.kI = payload.velX_ki;
    pid.kD = payload.velX_kd;
    FcSetPID(VEL_X, pid);
    //Y轴速度PID
    pid.kP = payload.velY_kp;
    pid.kI = payload.velY_ki;
    pid.kD = payload.velY_kd;
    FcSetPID(VEL_Y, pid);
    //Z轴速度PID
    pid.kP = payload.velZ_kp;
    pid.kI = payload.velZ_ki;
    pid.kD = payload.velZ_kd;
    FcSetPID(VEL_Z, pid);
    //X轴位置PID
    pid.kP = payload.posX_kp;
    pid.kI = 0;
    pid.kD = 0;
    FcSetPID(POS_X, pid);
    //Y轴位置PID
    pid.kP = payload.posY_kp;
    FcSetPID(POS_Y, pid);
    //Z轴位置PID
    pid.kP = payload.posZ_kp;
    FcSetPID(POS_Z, pid);    
}    
        
