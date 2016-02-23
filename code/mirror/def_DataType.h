#ifndef _DEF_TYPE_H
#define _DEF_TYPE_H

#include <stdint.h>
#include <QMap>

typedef int32_t mapID;
typedef int32_t skillID;
typedef int32_t itemID;
typedef int32_t monsterID;
typedef int32_t playerID;

enum Vocation
{
	Voc_Unknow,
	Voc_Warrior,
	Voc_Magic,
	Voc_Taoist
};

enum LeftWindow
{
	LW_role,
	LW_smithy
};

struct FightInfoStatistics
{
	mapID whatsMap;						//��һ�ιһ����ڵ�ͼ
	int32_t minutes;					//�������һ���ʱ��
	int32_t nCount_victory;				//������ʤ������
	int32_t nCount_StraightVictory;		//�����������ʤ����
	int32_t nCount_fail;				//������ʧ�ܴ���
	int32_t nCount_draw;				//��������ʱ����
	QMap<monsterID, int32_t> killMonster;	//��Ҫ��¼��ɱ�����������
};

#endif	//#ifndef _DEF_TYPE_H