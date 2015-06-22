#include "dlg_detail.h"
#include <QTextEdit>

Dlg_Detail::Dlg_Detail(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	hide();
}

Dlg_Detail::~Dlg_Detail()
{

}

void Dlg_Detail::on_btn_quit_clicked()
{
	hide();
}

QString GenerateEquipAttributeString(quint32 A2, quint32 extra, const QString &AttributeName)
{
	QString strTmp;
	//�����Ƿ��Ǽ�Ʒȷ��������ɫ
	if (extra > 0)
		strTmp = QStringLiteral("`<font color = green>");
	else
		strTmp = QStringLiteral("`<font color = white>");

	//������ʾ�ӳɺ�����ԡ�
	strTmp += AttributeName + QStringLiteral("+%1").arg(A2 + extra);

	//����ʾ��Ʒ����
	if (extra > 0)
	{
		strTmp += QStringLiteral(" (%1)").arg(extra);
	}
	strTmp += QStringLiteral("</font>");

	return strTmp;
}

QString GenerateEquipAttributeString_precent(quint32 A2, quint32 extra, const QString &AttributeName)
{
	QString strTmp;
	//�����Ƿ��Ǽ�Ʒȷ��������ɫ
	if (extra > 0)
		strTmp = QStringLiteral("`<font color = green>");
	else
		strTmp = QStringLiteral("`<font color = white>");

	//������ʾ�ӳɺ�����ԡ�
	strTmp += AttributeName + QStringLiteral("+%1%").arg(A2 * 0.01 + extra * 0.01);

	//����ʾ��Ʒ����
	if (extra > 0)
	{
		strTmp += QStringLiteral(" (%1)").arg(extra);
	}
	strTmp += QStringLiteral("</font>");

	return strTmp;
}
QString GenerateEquipAttributeString(quint32 A1, quint32 A2, quint32 extra, const QString &AttributeName)
{
	QString strTmp;
	//�����Ƿ��Ǽ�Ʒȷ��������ɫ
	if (extra > 0)
		strTmp = QStringLiteral("`<font color = green>");
	else
		strTmp = QStringLiteral("`<font color = white>");

	//������ʾ�ӳɺ�����ԡ�
	strTmp += AttributeName + QStringLiteral(" %1-%2").arg(A1).arg(A2 + extra);

	//����ʾ��Ʒ����
	if (extra > 0)
	{
		strTmp += QStringLiteral(" (%1)").arg(extra);
	}
	strTmp += QStringLiteral("</font>");

	return strTmp;
}

void Dlg_Detail::DisplayEquipInfo(QPoint pos, const Info_basic_equip *BasicInfo, const Info_Equip *Equip, const RoleInfo *roleInfo)
{
	bool bSatisfy;
	QString strTmp;
	qint32 lineCount = 0;
	quint32 nTmp;

	//������·���װ��������Ҫ�����ж�һ�½�ɫ�Ա��Ƿ���װ����Ҫ�Ա�����ϡ�
	strTmp = QStringLiteral("`<font color = yellow>%1</font>").arg(BasicInfo->name);
	int Type = (BasicInfo->ID - g_itemID_start_equip) / 1000;
	if (Type == g_equipType_clothes_m || Type == g_equipType_clothes_f)
	{
		//�Ա𲻷������ɴ�����
		//�˴��жϵļ�Ҫ���ͣ� genderȡֵ(male:1 female:2) Typeȡֵ(male:2 female:3)���ʼ�1����ȼ���
		if (roleInfo->gender != (Type - 1))
			strTmp = QStringLiteral("`<font color = red>%1</font>").arg(BasicInfo->name);
	}
	ui.edit_display->setText(strTmp);

	ui.edit_display->append(QStringLiteral("`<font color = white>����:1 �־�:99/99</font>"));
	ui.edit_display->append(QStringLiteral("`<font color = white>�Ƚף�%1</font>").arg(BasicInfo->lv));

	switch (Equip->extraAmount)
	{
	case 0:
		strTmp = QStringLiteral("`<font color = white>Ʒ�ʣ���ɫ</font>"); break;
	case 1:
	case 2:
		strTmp = QStringLiteral("`<font color = green>Ʒ�ʣ���ɫ</font>"); break;
	case 3:
	case 4:
		strTmp = QStringLiteral("`<font color = blue>Ʒ�ʣ���ɫ</font>"); break;
	case 5:
	case 6:
		strTmp = QStringLiteral("`<font color = magenta>Ʒ�ʣ���ɫ</font>"); break;
	case 7:
	case 8:
		strTmp = QStringLiteral("`<font color = #FEAB0D>Ʒ�ʣ���ɫ</font>"); break;
	default:
		strTmp = QStringLiteral("`<font color = gray>Ʒ�ʣ�δ֪</font>"); 
		break;
	}
	ui.edit_display->append(strTmp);
	ui.edit_display->append("");
	lineCount += 5;

	if (BasicInfo->luck + Equip->extra.luck > 0)
	{
		strTmp = GenerateEquipAttributeString(BasicInfo->luck, Equip->extra.luck, QStringLiteral("����"));
		ui.edit_display->append(strTmp);
		++lineCount;
	}

	if (BasicInfo->acc)
	{
		strTmp = GenerateEquipAttributeString(BasicInfo->acc, 0, QStringLiteral("׼ȷ"));
		ui.edit_display->append(strTmp);
		++lineCount;
	}
	if (BasicInfo->ag)
	{
		strTmp = GenerateEquipAttributeString(BasicInfo->ag, 0, QStringLiteral("����"));
		ui.edit_display->append(strTmp);
		++lineCount;
	}
	if (BasicInfo->spd)
	{
		strTmp = GenerateEquipAttributeString(BasicInfo->spd, 0, QStringLiteral("�����ٶ�"));
		ui.edit_display->append(strTmp);
		++lineCount;
	}
	if (BasicInfo->md > 0)
	{
		strTmp = GenerateEquipAttributeString(BasicInfo->md * 10, 0, QStringLiteral("ħ�����"));
		ui.edit_display->append(strTmp);
		++lineCount;
	}
	if (BasicInfo->ep > 0)
	{
		strTmp = GenerateEquipAttributeString_precent(BasicInfo->ep, 0, QStringLiteral("����"));
		ui.edit_display->append(strTmp);
		++lineCount;
	}
	if (BasicInfo->ed > 0)
	{
		strTmp = GenerateEquipAttributeString(BasicInfo->ed, 0, QStringLiteral("����"));
		ui.edit_display->append(strTmp);
		++lineCount;
	}
	

	if (BasicInfo->ac1 > 0 || (BasicInfo->ac2 + Equip->extra.ac > 0))
	{
		strTmp = GenerateEquipAttributeString(BasicInfo->ac1, BasicInfo->ac2, Equip->extra.ac, QStringLiteral("����"));
		ui.edit_display->append(strTmp);
		++lineCount;
	}

	if (BasicInfo->mac1 > 0 || (BasicInfo->mac2 + Equip->extra.mac > 0))
	{
		strTmp = GenerateEquipAttributeString(BasicInfo->mac1, BasicInfo->mac2, Equip->extra.mac, QStringLiteral("ħ��"));
		ui.edit_display->append(strTmp);
		++lineCount;
	}
	if (BasicInfo->dc1 > 0 || (BasicInfo->dc2 + Equip->extra.dc > 0))
	{
		strTmp = GenerateEquipAttributeString(BasicInfo->dc1, BasicInfo->dc2, Equip->extra.dc, QStringLiteral("����"));
		ui.edit_display->append(strTmp);
		++lineCount;
	}
	if (BasicInfo->mc1 > 0 || (BasicInfo->mc2 + Equip->extra.mc > 0))
	{
		strTmp = GenerateEquipAttributeString(BasicInfo->mc1, BasicInfo->mc2, Equip->extra.mc, QStringLiteral("ħ��"));
		ui.edit_display->append(strTmp);
		++lineCount;
	}
	if (BasicInfo->sc1 > 0 || (BasicInfo->sc2 + Equip->extra.sc > 0))
	{
		strTmp = GenerateEquipAttributeString(BasicInfo->sc1, BasicInfo->sc2, Equip->extra.sc, QStringLiteral("����"));
		ui.edit_display->append(strTmp);
		++lineCount;
	}
	
	//��ѯ��ɫ��ǰ�����Ƿ���������Ҫ����������ϣ�����ʾΪ��ɫ������Ĭ����ɫ��
	switch (BasicInfo->need)
	{
	case 0: 
		bSatisfy = (roleInfo->level >= BasicInfo->needLvl); 
		break;
	case 1: 
		nTmp = roleInfo->dc2_1 << 24 | roleInfo->dc2_2 << 16 | roleInfo->dc2_3 << 8 | roleInfo->dc2_4;
		bSatisfy = (nTmp >= BasicInfo->needLvl); 
		break;
	case 2: 
		nTmp = roleInfo->mc2_1 << 24 | roleInfo->mc2_2 << 16 | roleInfo->mc2_3 << 8 | roleInfo->mc2_4;
		bSatisfy = (nTmp >= BasicInfo->needLvl);
		break;
	case 3: 
		nTmp = roleInfo->sc2_1 << 24 | roleInfo->sc2_2 << 16 | roleInfo->sc2_3 << 8 | roleInfo->sc2_4;
		bSatisfy = (nTmp >= BasicInfo->needLvl);
		break;
	default:
		bSatisfy = false;
		break;
	}

	if (bSatisfy)
	{
		strTmp = QStringLiteral("`<font color = white>");
	}
	else
	{
		strTmp = QStringLiteral("`<font color = red>");
	}
	strTmp += equip_need_info[BasicInfo->need] + QString::number(BasicInfo->needLvl);
	strTmp += QStringLiteral("</font>");

	ui.edit_display->append(strTmp);
	lineCount+= 2;

	this->move(pos);
	this->resize(199, lineCount * 18);
}

void Dlg_Detail::DisplayItemInfo(QPoint pos, const Info_Item *item, quint32 no, quint32 role_voc, quint32 role_lvl)
{
	QString strTmp;
	qint32 lineCount = 10;
	qint32 nTmp;

	strTmp = QStringLiteral("`<font color = yellow>") + item->name + QStringLiteral("</font>");
	ui.edit_display->setText(strTmp);

	nTmp = (item->ID / 1000) % 100;
	QString def_ItemType[100] = { QStringLiteral("δ֪"), QStringLiteral("ҩƷ"), QStringLiteral("����"), QStringLiteral("����"), QStringLiteral("���Ե���") };
	def_ItemType[20] = QStringLiteral("�鼮");
	def_ItemType[99] = QStringLiteral("����");
	ui.edit_display->append(QStringLiteral("`<font color = white>����:%1</font>").arg(def_ItemType[nTmp]));

	//��ѯ��ɫ��ǰ�����Ƿ���������Ҫ����������ϣ�����ʾΪ��ɫ������Ĭ����ɫ��
	bool bSatisfy = (role_lvl >= item->level);
	if (bSatisfy)
		strTmp = QStringLiteral("`<font color = white>�ȼ�:%1</font>").arg(item->level);
	else
		strTmp = QStringLiteral("`<font color = red>�ȼ�:%1</font>").arg(item->level);
	ui.edit_display->append(strTmp);

	if (item->vocation != 0)
	{
		if (item->vocation == role_voc)
			strTmp = QStringLiteral("`<font color = white>ְҵ:%1</font>").arg(def_vocation[item->vocation]);
		else
			strTmp = QStringLiteral("`<font color = red>ְҵ:%1</font>").arg(def_vocation[item->vocation]);

		++lineCount;
		ui.edit_display->append(strTmp);
	}
	ui.edit_display->append(QStringLiteral("`<font color = white>����:1</font>"));
	ui.edit_display->append(" ");//����
	ui.edit_display->append(QStringLiteral("`<font color = white>����:%1</font>").arg(item->coin));
	ui.edit_display->append(QStringLiteral("`<font color = white>����:%1</font>").arg(no));
	ui.edit_display->append(" ");//����
	ui.edit_display->append(QStringLiteral("`<font color = blue>%1</font>").arg(item->descr));

	this->move(pos);
	this->resize(199, lineCount * 18);
}
