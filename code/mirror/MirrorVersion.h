#ifndef _MIRRORVERSION_H_
#define _MIRRORVERSION_H_

#include <stdint.h>

//ÿ���������ֵΪ999
const int32_t version_major = 1,
	version_minor = 2,
	version_build = 2;

//const qint32 SaveFileVer = 99;						//����
const qint32 SaveFileVer = 15;					//�浵�ļ��汾
const QString SaveFileName = "save.sxv";			//�浵�ļ�����
const QString BackFileName = "save.backup";			//���ݴ浵�ļ���
const QString RecoveryFileName = "save.recovery";	//���ָ��Ĵ浵
const QString SettingFileName = "./setting.ini";	//�����ĵ���·����

#endif //#ifndef _MIRRORVERSION_H_