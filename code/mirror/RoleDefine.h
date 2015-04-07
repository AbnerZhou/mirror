#ifndef _ROLEDEFINE_H
#define _ROLEDEFINE_H

struct RoleInfo {
	QString name;
	quint64 coin;
	quint64 gold;
	quint64 reputation;
	quint64 exp;
	quint64 lvExp;
	quint32 level;
	quint32 hp_m;
	quint32 mp_m;
	quint32 ap_m;
	quint32 strength;
	quint32 wisdom;
	quint32 spirit;
	quint32 life;
	quint32 agility;
	quint32 potential;
};

struct MonsterInfo {
	QString name;
	quint32 ID;
	quint32 level;
	quint32 exp;
	quint32 hp_m;		//�������ֵ
	quint32 hp_c;		//��ǰ����ֵ
	quint32 hp_r;		//ÿ�������ָ���
	quint32 mp_m;		//���ħ��ֵ
	quint32 mp_c;		//��ǰħ��ֵ
	quint32 mp_r;		//ÿ��ħ���ָ���
	quint32 ap_m;		//���ŭ��ֵ���㶨10
	quint32 ap_c;		//��ǰŭ��ֵ
	quint32 ap_r;		//ÿ��ŭ���ָ���,�㶨1
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

#endif //#ifndef _ROLEDEFINE_H

