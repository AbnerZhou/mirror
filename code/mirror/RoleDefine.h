#ifndef _ROLEDEFINE_H
#define _ROLEDEFINE_H

const QString def_vocation[] = {
	QString::fromLocal8Bit("��ְҵ"),
	QString::fromLocal8Bit("սʿ"),
	QString::fromLocal8Bit("��ʦ"),
	QString::fromLocal8Bit("��ʿ")
};

struct Info_jobAdd {
	quint32 level;			//�ȼ�
	quint32 hp;				//Ѫ
	quint32 mp;				//��
	quint32 dc1;			//����_����
	quint32 dc2;			//����_����
	quint32 mc1;			//ħ��
	quint32 mc2;			//ħ��
	quint32 sc1;			//����
	quint32 sc2;			//����
	quint32 ac1;			//���
	quint32 ac2;			//���
	quint32 mac1;			//ħ��
	quint32 mac2;			//ħ��
};
typedef QMap<quint32, QVector<Info_jobAdd>> mapJobAdd;

struct RoleInfo {
	QString name;			//��ɫ��
	quint32 vocation;		//ְҵ
	quint64 coin;			//���
	quint64 gold;			//Ԫ��
	quint64 reputation;		//����
	quint64 exp;			//��ǰ����ֵ
	quint64 lvExp;			//��ǰ�������辭��ֵ
	quint32 level;			//�ȼ�
	quint32 hp;				//Ѫ
	quint32 mp;				//��
	quint32 dc1;			//����_����
	quint32 dc2;			//����_����
	quint32 mc1;			//ħ��
	quint32 mc2;			//ħ��
	quint32 sc1;			//����
	quint32 sc2;			//����
	quint32 ac1;			//���
	quint32 ac2;			//���
	quint32 mac1;			//ħ��
	quint32 mac2;			//ħ��
	quint32 intervel;		//�������
	quint32 luck;			//����
	quint32 strength;		//���Ե㣺����
	quint32 wisdom;			//���Ե㣺�ǻ�
	quint32 spirit;			//���Ե㣺����
	quint32 life;			//���Ե㣺����
	quint32 agility;		//���Ե㣺����
	quint32 potential;		//Ǳ�ܵ㡣���ڷ��䵽��Ӧ���Ե㡣
};


#endif //#ifndef _ROLEDEFINE_H

