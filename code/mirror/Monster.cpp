#include "Monster.h"


CMonster::CMonster()
	:COrganisms("unKnow", 0)
{
	skill.type = 0;
	skill.damage = 100;
	skill.basic = 0;
	skill.times = 1;
	skill.name = QStringLiteral("���﹥��");
}


CMonster::~CMonster()
{
}

bool CMonster::ReplaceSoul(const MonsterInfo &info, bool boss)
{
	bBoss = boss;
	id = info.ID;
	name = info.name;
	set_head(info.Head);
	set_Lv(info.level);
	set_exp(info.exp);
	set_hp_m(info.hp);
	set_mp_m(info.mp);
	set_dc(info.DC1, info.DC2);
	set_mc(info.MC1, info.MC2);
	set_sc(0, 0);
	set_ac(info.AC, info.AC);
	set_mac(info.MAC, info.MAC);
	set_intervel(info.interval);

	QString str[10] = { QStringLiteral("�˻�"), QStringLiteral("��ײ"), QStringLiteral("����"), QStringLiteral("�ɻ�"), QStringLiteral("Ӱ��"),
		QStringLiteral("����"), QStringLiteral("������"), QStringLiteral("����"), QStringLiteral("����"), QStringLiteral("��̤") };
	skill.name = str[qrand() % 10];
	return true;
}
