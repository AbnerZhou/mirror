#ifndef _MAP_DEFINE_H
#define _MAP_DEFINE_H

const qint32 
	mapCount_major = 9,
	mapCount_minor = 10;

struct mirror_map {
	qint32 ID;
	QString name;
};

const mirror_map map_major[mapCount_major] = {
	{ 0, QString::fromLocal8Bit("���ִ�") },
	{ 1, QString::fromLocal8Bit("����") },
	{ 2, QString::fromLocal8Bit("����") },
	{ 3, QString::fromLocal8Bit("������") },
	{ 4, QString::fromLocal8Bit("���µ�") },	
	{ 5, QString::fromLocal8Bit("��ħ��") },
	{ 6, QString::fromLocal8Bit("ħ����") },
	{ 7, QString::fromLocal8Bit("��ɫ��ͼ") },
	{ 8, QString::fromLocal8Bit("��Ȩ��ͼ") }
};

const mirror_map map_minor[mapCount_major][mapCount_minor] = {
	{ 0, QString::fromLocal8Bit("���ִ�") },
	{ 0, QString::fromLocal8Bit("�������") ,
	  1, QString::fromLocal8Bit("�����˹�Ĺ") ,
	  2, QString::fromLocal8Bit("��") ,
	  3, QString::fromLocal8Bit("�������")
	},
	{ 0, QString::fromLocal8Bit("���س���") ,
	  1, QString::fromLocal8Bit("����ɽ��"),
	  2, QString::fromLocal8Bit("��򼶴"),
	  3, QString::fromLocal8Bit("��"),
	  4, QString::fromLocal8Bit("�������")
	},
	{	0, QString::fromLocal8Bit("�����ų���"),
		1, QString::fromLocal8Bit("�����Թ�"),
		2, QString::fromLocal8Bit("����Ͽ��"),
		3, QString::fromLocal8Bit("Ͽ�ȹ㳡"),
		4, QString::fromLocal8Bit("����֮��"),
		5, QString::fromLocal8Bit("��ħ��Ѩ")
	},
	{ 0, QString::fromLocal8Bit("��ħ��") ,
	  1, QString::fromLocal8Bit("ʬħ��") ,
	  2, QString::fromLocal8Bit("ţħ����") 
	},
	{ 0, QString::fromLocal8Bit("��ħ����") ,
	  1, QString::fromLocal8Bit("��ħ����") ,
	  2, QString::fromLocal8Bit("�����") ,
	  3, QString::fromLocal8Bit("�����") ,
	  4, QString::fromLocal8Bit("ħ���") ,
	  5, QString::fromLocal8Bit("��ħ��") ,
	  6, QString::fromLocal8Bit("���ߴ���")
	},
	{ 0, QString::fromLocal8Bit("ħ������") ,
	  1, QString::fromLocal8Bit("ħ����կ") ,
	  2, QString::fromLocal8Bit("ħ����̳") ,
	  3, QString::fromLocal8Bit("ħ����") ,
	  4, QString::fromLocal8Bit("ħ������") ,
	  5, QString::fromLocal8Bit("ħ��Ѫ��") 
	},
	{ 0, QString::fromLocal8Bit("��װ����")
	},
	{ 0, QString::fromLocal8Bit("ʥ��þ�1"),
	1, QString::fromLocal8Bit("ʥ��þ�2"),
	2, QString::fromLocal8Bit("ʥ��þ�3"),
	3, QString::fromLocal8Bit("ʥ��þ�4"),
	4, QString::fromLocal8Bit("ʥ��þ�5"),
	5, QString::fromLocal8Bit("���߽���"),
	6, QString::fromLocal8Bit("ʥ��þ�7"),
	7, QString::fromLocal8Bit("ʥ��þ�8"),
	8, QString::fromLocal8Bit("ʥ��þ�9"),
	9, QString::fromLocal8Bit("ʥ��þ�10")
	}
};

#endif