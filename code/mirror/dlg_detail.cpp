#include "dlg_detail.h"
#include <QTextEdit>

Dlg_Detail::Dlg_Detail(QWidget *parent, CPlayer *w_player)
	: QDialog(parent), player(w_player)
{
	ui.setupUi(this);
	hide();

	QFont f = ui.edit_display->font();
	QFontMetrics fm(f);
	lineSpacing = fm.lineSpacing();
}

Dlg_Detail::~Dlg_Detail()
{

}

void Dlg_Detail::on_btn_quit_clicked()
{
	hide();
}

QString GenerateEquipAttributeString(quint32 A2, const QString &AttributeName)
{
	QString strTmp;
	strTmp = QStringLiteral("`<font color = white>");

	//������ʾ�ӳɺ�����ԡ�
	strTmp += AttributeName + QStringLiteral(" +%1 </font>").arg(A2);

	return strTmp;
}

QString GenerateEquipAttributeString(quint32 A1, quint32 A2, const QString &AttributeName)
{
	QString strTmp;
	strTmp = QStringLiteral("`<font color = white>");

	//������ʾ�ӳɺ�����ԡ�
	strTmp += AttributeName + QStringLiteral(" %1-%2</font>").arg(A1).arg(A2);

	return strTmp;
}

void Dlg_Detail::DisplayEquipInfo(QPoint pos, const Info_basic_equip *BasicInfo, const Info_Equip *Equip)
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
		if (player->get_gender() != (Type - 1))
			strTmp = QStringLiteral("`<font color = red>%1</font>").arg(BasicInfo->name);
	}
	ui.edit_display->setText(strTmp);

	ui.edit_display->append(QStringLiteral("`<font color = white>������1 �־�:99/99</font>"));
	ui.edit_display->append(QStringLiteral("`<font color = white>�Ƚף�%1</font>").arg(BasicInfo->lv));

	switch (Equip->extraAmount)
	{
	case 0:
		strTmp = QStringLiteral("`<font color = white>Ʒ�ʣ���ͨ</font>"); break;
	case 1:
		strTmp = QStringLiteral("`<font color = green>Ʒ�ʣ�����</font>"); break;
	case 2:
		strTmp = QStringLiteral("`<font color = blue>Ʒ�ʣ�ϡ��</font>"); break;
	case 3:
		strTmp = QStringLiteral("`<font color = magenta>Ʒ�ʣ�׿Խ</font>"); break;
	case 4:
		strTmp = QStringLiteral("`<font color = #FEAB0D>Ʒ�ʣ�����</font>"); break;		
	default:
		strTmp = QStringLiteral("`<font color = gray>Ʒ�ʣ�δ֪</font>"); 
		break;
	}
	ui.edit_display->append(strTmp);
	ui.edit_display->append("");
	ui.edit_display->append(QStringLiteral("<font color = yellow>��������</font>"));
	lineCount += 6;
	
	if (BasicInfo->luck > 0)
	{
		strTmp = GenerateEquipAttributeString(BasicInfo->luck, QStringLiteral("����"));
		ui.edit_display->append(strTmp);
		++lineCount;
	}
	if (BasicInfo->spd > 0)
	{
		strTmp = GenerateEquipAttributeString(BasicInfo->spd, QStringLiteral("�����ٶ�"));
		ui.edit_display->append(strTmp);
		++lineCount;
	}

	if (BasicInfo->hp > 0)
	{
		strTmp = GenerateEquipAttributeString(BasicInfo->hp, QStringLiteral("����"));
		ui.edit_display->append(strTmp);
		++lineCount;
	}
	if (BasicInfo->mp > 0)
	{
		strTmp = GenerateEquipAttributeString(BasicInfo->mp, QStringLiteral("����"));
		ui.edit_display->append(strTmp);
		++lineCount;
	}
	if (BasicInfo->ac > 0)
	{
		strTmp = GenerateEquipAttributeString(BasicInfo->ac, QStringLiteral("����"));
		ui.edit_display->append(strTmp);
		++lineCount;
	}

	if (BasicInfo->mac > 0)
	{
		strTmp = GenerateEquipAttributeString(BasicInfo->mac, QStringLiteral("ħ��"));
		ui.edit_display->append(strTmp);
		++lineCount;
	}
	if (BasicInfo->dc1 > 0 || (BasicInfo->dc2 > 0))
	{
		strTmp = GenerateEquipAttributeString(BasicInfo->dc1, BasicInfo->dc2, QStringLiteral("����"));
		ui.edit_display->append(strTmp);
		++lineCount;
	}
	if (BasicInfo->mc1 > 0 || (BasicInfo->mc2 > 0))
	{
		strTmp = GenerateEquipAttributeString(BasicInfo->mc1, BasicInfo->mc2, QStringLiteral("ħ��"));
		ui.edit_display->append(strTmp);
		++lineCount;
	}
	if (BasicInfo->sc1 > 0 || (BasicInfo->sc2 > 0))
	{
		strTmp = GenerateEquipAttributeString(BasicInfo->sc1, BasicInfo->sc2, QStringLiteral("����"));
		ui.edit_display->append(strTmp);
		++lineCount;
	}

	if (Equip->extraAmount > 0)
	{
		ui.edit_display->append("");
		ui.edit_display->append(QStringLiteral("<font color = yellow>��Ʒ����</font>"));
		lineCount += 2;

		for (int i = 0; i < Equip->extraAmount; i++)
		{
			strTmp = "";
			const EquipExtra2 &extra = Equip->extra[i];
			switch (extra.eet)
			{
			case eet_fixed_hp: strTmp = QStringLiteral("���� +%1").arg(extra.value); break;
			case eet_fixed_mp: strTmp = QStringLiteral("���� +%1").arg(extra.value); break;
			case eet_fixed_hpr: strTmp = QStringLiteral("�����ָ� +%1").arg(extra.value); break;
			case eet_fixed_mpr: strTmp = QStringLiteral("�����ָ� +%1").arg(extra.value); break;
			case eet_fixed_dc: strTmp = QStringLiteral("���� +%1").arg(extra.value); break;
			case eet_fixed_mc: strTmp = QStringLiteral("ħ�� +%1").arg(extra.value); break;
			case eet_fixed_sc: strTmp = QStringLiteral("���� +%1").arg(extra.value); break;
			case eet_fixed_ac: strTmp = QStringLiteral("���� +%1").arg(extra.value); break;
			case eet_fixed_mac: strTmp = QStringLiteral("ħ�� +%1").arg(extra.value); break;
			case eet_fixed_spd: strTmp = QStringLiteral("�����ٶ� +%1").arg(extra.value); break;
			case eet_fixed_luck:strTmp = QStringLiteral("���� +%1").arg(extra.value); break;
			case eet_percent_hp: strTmp = QStringLiteral("���� +%1%").arg(extra.value); break;
			case eet_percent_mp: strTmp = QStringLiteral("���� +%1%").arg(extra.value); break;
			case eet_percent_hpr: strTmp = QStringLiteral("�����ָ� +%1%").arg(extra.value); break;
			case eet_percent_mpr: strTmp = QStringLiteral("�����ָ� +%1%").arg(extra.value); break;
			case eet_percent_dc: strTmp = QStringLiteral("���� +%1%").arg(extra.value); break;
			case eet_percent_mc: strTmp = QStringLiteral("ħ�� +%1%").arg(extra.value); break;
			case eet_percent_sc: strTmp = QStringLiteral("���� +%1%").arg(extra.value); break;
			case eet_percent_ac: strTmp = QStringLiteral("���� +%1%").arg(extra.value); break;
			case eet_percent_mac: strTmp = QStringLiteral("ħ�� +%1%").arg(extra.value); break;
			default:
				break;
			}

			if (!strTmp.isEmpty())
			{
				ui.edit_display->append(QString("`<font color = white>%1</font>").arg(strTmp));
				lineCount ++;
			}
		}
	}
	
	//��ѯ��ɫ��ǰ�����Ƿ���������Ҫ����������ϣ�����ʾΪ��ɫ������Ĭ����ɫ��
	switch (BasicInfo->need)
	{
	case 0: 
		bSatisfy = (player->get_lv() >= BasicInfo->needLvl);
		break;
	case 1: 
		bSatisfy = (player->get_dc2() >= BasicInfo->needLvl); 
		break;
	case 2: 
		bSatisfy = (player->get_mc2() >= BasicInfo->needLvl);
		break;
	case 3: 
		bSatisfy = (player->get_sc2() >= BasicInfo->needLvl);
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
	this->resize(150, lineCount * lineSpacing + 12);
}

void Dlg_Detail::DisplayItemInfo(QPoint pos, const Info_Item *item, quint32 no)
{
	QString strTmp;
	qint32 lineCount = 11;
	qint32 nTmp;

	strTmp = QStringLiteral("`<font color = yellow>") + item->name + QStringLiteral("</font>");
	ui.edit_display->setText(strTmp);

	nTmp = (item->ID / 1000) % 100;
	QString def_ItemType[100] = { QStringLiteral("δ֪"), QStringLiteral("ҩƷ"), QStringLiteral("����"), QStringLiteral("����"), QStringLiteral("���Ե���") };
	def_ItemType[20] = QStringLiteral("�鼮");
	def_ItemType[99] = QStringLiteral("����");
	ui.edit_display->append(QStringLiteral("`<font color = white>����:%1</font>").arg(def_ItemType[nTmp]));

	//��ѯ��ɫ��ǰ�����Ƿ���������Ҫ����������ϣ�����ʾΪ��ɫ������Ĭ����ɫ��
	bool bSatisfy = (player->get_lv() >= item->level);
	if (bSatisfy)
		strTmp = QStringLiteral("`<font color = white>�ȼ�:%1</font>").arg(item->level);
	else
		strTmp = QStringLiteral("`<font color = red>�ȼ�:%1</font>").arg(item->level);
	ui.edit_display->append(strTmp);

	if (item->vocation != 0)
	{
		if (item->vocation == player->get_voc())
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
	ui.edit_display->append(QStringLiteral("`<font color = cyan>%1</font>").arg(item->descr));

	this->move(pos);
	this->resize(199, lineCount * lineSpacing + 12);
}
