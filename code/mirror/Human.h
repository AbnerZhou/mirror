#pragma once
#include "Organisms.h"
#include "def_DataType.h"
#include "RoleDefine.h"
#include "ItemDefine.h"

class CHuman :
	public COrganisms
{
public:
	CHuman();
	~CHuman();

	//���װ�������������λ�á�
	int32_t wearEquip(uint32_t index);

	//ȡ��ָ��λ�õ�װ����
	int32_t takeoffEquip(uint32_t location);

	//
	Info_Equip *get_onBodyEquip_point(void) { return onWearEquip; }
	bool HasEquip(qint32 index) {
		if (index >= 0 && index < MaxEquipCountForRole)
			return onWearEquip[index].ID != 0;
		else
			return false;
	}

	void add_exp(int32_t no)
	{ 
		int32_t n = get_exp() + no;
		if (n > lvExp) {
			set_levelInfo(get_lv()+1, 0);
			updateParameter();
		} else {
			set_levelInfo(get_lv(), n);
		}
	}

	void set_resver1(int32_t r1, int32_t r2, int32_t r3, int32_t r4, int32_t r5) 
	{ 
		resver1 = r1, resver2 = r2, resver3 = r3, resver4 = r4, resver5 = r5; 
	}
	void set_resver2(int32_t r6, int32_t r7, int32_t r8, int32_t r9, int32_t r10)
	{
		resver1 = r6, resver2 = r7, resver3 = r8, resver4 = r9, resver5 = r10;
	}

	void updateEquipInfo();
	void updateParameter();

private:
	Info_Equip onWearEquip[MaxEquipCountForRole];	//���ϴ�����װ��
	Info_basic_equip equip_basic;					//ͳ�Ƶ�װ���������Լӳ�

	int32_t lvExp;
	
	qint32 resver1, resver2, resver3, resver4, resver5;
	qint32 resver6, resver7, resver8, resver9, resver10;

	//װ����չ���Լӳ�
	qint32 fixed_hp, fixed_mp, fixed_hpr, fixed_mpr, fixed_dc, fixed_mc, fixed_sc, fixed_ac, fixed_mac, fixed_spd, fixed_luck;
	qint32 percent_hp, percent_mp, percent_hpr, percent_mpr, percent_dc, percent_mc, percent_sc, percent_ac, percent_mac;
};

