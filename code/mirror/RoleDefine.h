#ifndef _ROLEDEFINE_H
#define _ROLEDEFINE_H

const QString vocation[] = {
	QString::fromLocal8Bit("սʿ"),
	QString::fromLocal8Bit("��ʦ"),
	QString::fromLocal8Bit("��ʿ")
};

struct RoleInfo {
	QString name;			//��ɫ��
//	quint32 vocation;		//ְҵ
	quint64 coin;			//���
	quint64 gold;			//Ԫ��
	quint64 reputation;		//����
	quint64 exp;			//��ǰ����ֵ
	quint64 lvExp;			//��ǰ�������辭��ֵ
	quint32 level;			//�ȼ�
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


#endif //#ifndef _ROLEDEFINE_H

