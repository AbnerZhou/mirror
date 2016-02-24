#ifndef _ROLEDEFINE_H
#define _ROLEDEFINE_H

const QString def_vocation[4] = {
	QStringLiteral("��ְҵ"),
	QStringLiteral("սʿ"),
	QStringLiteral("��ʦ"),
	QStringLiteral("��ʿ")
};

const QString def_xiulian[12] = {
	QStringLiteral("����"),
	QStringLiteral("����"),
	QStringLiteral("�ᵤ"),
	QStringLiteral("ԪӤ"),
	QStringLiteral("����"),
	QStringLiteral("Ӥ��"),
	QStringLiteral("�ʶ�"),
	QStringLiteral("����"),
	QStringLiteral("��ʵ"),
	QStringLiteral("����"),
	QStringLiteral("����"),
	QStringLiteral("����")
};

struct Info_jobAdd {
	qint32 level;		//�ȼ�
	qint32 exp;			//�ȼ����辭��
	qint32 hp;			//Ѫ
	qint32 mp;			//��
	qint32 dc1;			//����_����
	qint32 dc2;			//����_����
	qint32 mc1;			//ħ��
	qint32 mc2;			//ħ��
	qint32 sc1;			//����
	qint32 sc2;			//����
	qint32 ac;			//���
	qint32 mac;			//ħ��
};
typedef QMap<qint32, QVector<Info_jobAdd>> mapJobAdd;

struct Info_Chenhao {
	qint32 level;
	QString name;
	qint32 need;
	qint32 dc1;
	qint32 dc2;
	qint32 mc1;
	qint32 mc2;
	qint32 sc1;
	qint32 sc2;
};

#endif //#ifndef _ROLEDEFINE_H

