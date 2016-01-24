#ifndef _DEF_SKILL_H
#define _DEF_SKILL_H

#include <QPixmap>
#include "def_DataType.h"

struct Info_SkillBasic {
	skillID ID;
	QString name;
	QPixmap icon;
	qint32 level;		//�����б��б�ʾ���ȼ�����ɫ�����д���ǰ�ȼ�
//	qint32 spell_basic;
//	qint32 spell_add;
	qint32 cd[4];
	qint32 type;
	qint32 no;
	QString descr;
};

struct Info_SkillDamage
{
	qint32 id;
	qint32 type;
	qint32 targets;
	qint32 times;
	qint32 extra;
	qint32 basic;
	qint32 add;
};
enum BufferType
{
	et_DamageEnhance = 1,
	et_DamageSave,
	et_ac_fixed,	//��ʿ�ӷ��༼�ܣ�����������һ�����ֵ������ʥս��
	et_mac_fixed,	//��ʿ�ӷ��༼�ܣ�����������һ��ħ��ֵ���������
	et_ac_percent,	//սʿ�����༼�ܣ��ٷֱ������������
	et_mac_percent,	//��ʦ�����༼�ܣ��ٷֱ���������ħ��
	et_speed,
	et_rhp
};
struct Info_SkillBuff
{
	qint32 id;
	qint32 time;
	qint32 targets;
	BufferType et;
	qint32 basic;
	qint32 add;
};
struct Info_SkillSummon
{
	qint32 id;
	qint32 photo;
	qint32 type;
	qint32 hp;
	qint32 damage1;
	qint32 damage2;
	qint32 defense;
};
struct Info_SkillTreat
{
	qint32 id;
	qint32 targets;
	qint32 hpr_basic;
	qint32 hpr_add;
};

struct SkillStudy
{
	skillID id;
	qint32 level;
	qint32 usdIndex;
};

struct SkillFight
{
	skillID id;
	QString name;
	QPixmap icon;
	qint32 level;		//�����б��б�ʾ���ȼ�����ɫ�����д���ǰ�ȼ�
	qint32 spell;
	qint32 cd;
	qint32 cd_c;
	qint32 type;
	qint32 no;

	SkillFight() { 0; }
	SkillFight(const Info_SkillBasic &skillInfo, qint32 studyLv)
	{
		id = skillInfo.ID;
		name = skillInfo.name;
		icon = skillInfo.icon;
		level = studyLv;
		spell = 0; // skillInfo.spell_basic + skillInfo.spell_add * level;
		cd = skillInfo.cd[studyLv-1];
		cd_c = 0;
		type = skillInfo.type;
		no = skillInfo.no;
	}
};

typedef QMap<skillID, SkillStudy> MapSkillStudy;

#endif	//#ifndef _DEF_SKILL_H