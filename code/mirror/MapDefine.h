#ifndef _MAP_DEFINE_H
#define _MAP_DEFINE_H

#include "def_DataType.h"

const qint32 
	mapCount_major = 9,
	mapCount_minor = 10;

struct mirror_map {
	mapID ID;
	quint32 imageID;
	QString name;
};

const mirror_map map_major[mapCount_major] = {
	{ 0, 0, QStringLiteral("���ִ�") },
	{ 1, 0, QStringLiteral("����") },
	{ 2, 0, QStringLiteral("����") },
	{ 3, 0, QStringLiteral("������") },
	{ 4, 0, QStringLiteral("���µ�") },
	{ 5, 0, QStringLiteral("��ħ��") },
	{ 6, 0, QStringLiteral("ħ����") },
	{ 7, 0, QStringLiteral("��ɫ��ͼ") },
	{ 8, 0, QStringLiteral("ʥ��þ�") }
};

const mirror_map map_minor[mapCount_major][mapCount_minor] = {
	{ 0, 0, QStringLiteral("���ִ�") },
	{ 0, 10, QStringLiteral("�������") ,
	  1, 11, QStringLiteral("�����˹�Ĺ") ,
	  2, 12, QStringLiteral("��") ,
	  3, 13, QStringLiteral("�������")
	},
	{ 0, 20, QStringLiteral("���س���") ,
	  1, 21, QStringLiteral("����ɽ��"),
	  2, 22, QStringLiteral("��򼶴"),
	  3, 23, QStringLiteral("��"),
	  4, 24, QStringLiteral("�������")
	},
	{	0, 30, QStringLiteral("�����ų���"),
		1,31, QStringLiteral("�����Թ�"),
		2,32, QStringLiteral("����Ͽ��"),
		3,33, QStringLiteral("Ͽ�ȹ㳡"),
		4,34, QStringLiteral("����֮��"),
		5,35, QStringLiteral("��ħ��Ѩ")
	},
	{ 0,40, QStringLiteral("ʬħ��") ,
	  1,41, QStringLiteral("��ħ��") ,
	  2,42, QStringLiteral("ţħ����") 
	},
	{ 0,50, QStringLiteral("��ħ����") ,
	  1,51, QStringLiteral("��ħ����") ,
	  2,52, QStringLiteral("�����") ,
	  3,53, QStringLiteral("ħ���") ,
	  4,54, QStringLiteral("�����") ,
	  5,55, QStringLiteral("���ߴ���") ,
	  6,56, QStringLiteral("��ħ��")
	},
	{ 0,60, QStringLiteral("ħ������") ,
	  1,61, QStringLiteral("ħ����") ,
	  2,62, QStringLiteral("ħ����̳") ,
	  3,63, QStringLiteral("ħ����") ,
	  4,64, QStringLiteral("ħ������") ,
	  5,65, QStringLiteral("ħ��Ѫ��") 
	},
	{ 0,70, QStringLiteral("��װ����")
	},
	{ 0,80, QStringLiteral("ʥ��þ�1"),
	1,81, QStringLiteral("ʥ��þ�2"),
	2,13, QStringLiteral("ʥ��þ�3"),
	3,22, QStringLiteral("ʥ��þ�4"),
	4,23, QStringLiteral("ʥ��þ�5"),
	5,71, QStringLiteral("���߽���"),
	6,24, QStringLiteral("ʥ��þ�7"),
	7,55, QStringLiteral("ʥ��þ�8"),
	8,33, QStringLiteral("ʥ��þ�9"),
	9,42, QStringLiteral("ʥ��þ�10")
	}
};

#endif