#ifndef _MONSTERDEFINE_H
#define _MONSTERDEFINE_H

#include <QImage>

struct MonsterInfo {
	quint32 ID;
	QString name;
	QImage Head;
	qint32 level;
	qint32 exp;
	qint32 hp_m;		//�������ֵ
	qint32 hp_c;		//��ǰ����ֵ
	qint32 hp_r;		//ÿ�������ָ���
	qint32 mp_m;		//���ħ��ֵ
	qint32 mp_c;		//��ǰħ��ֵ
	qint32 mp_r;		//ÿ��ħ���ָ���
	qint32 ap_m;		//���ŭ��ֵ���㶨10
	qint32 ap_c;		//��ǰŭ��ֵ
	qint32 ap_r;		//ÿ��ŭ���ָ���,�㶨1
	quint32 DC;			//�﹥
	quint32 MC;			//ħ��
	quint32 SC;			//����
	quint32 AC;			//���
	quint32 MAC;		//ħ��
	quint32 extrarate;	
	quint32 extrahurt;
	quint32 penetrate;
	double	Speed;		//�����ٶ�
};

#endif //#ifndef _MONSTERDEFINE_H

