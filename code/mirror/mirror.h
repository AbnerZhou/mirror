#ifndef MIRROR_H
#define MIRROR_H

#include <QtWidgets/QMainWindow>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include "ui_mirror.h"

#include "role.h"
#include "fight.h"
#include "city.h"

class mirror : public QMainWindow
{
	Q_OBJECT

public:
	mirror(QWidget *parent = 0);
	~mirror();

	//�ӽ�ɫһЩ���ߣ��˺�����Ҫ�������Ե��ߡ�
	void GiveSomeItem(void);

protected:
	void closeEvent(QCloseEvent *event);

private:
	//���ش浵
	bool LoadRole();
	//����ְҵ�ӳ���Ϣ���ݿ�
	bool LoadJobSet();
	//������������������Ϣ���ݿ�
	bool LoadExpSetting();
	//���ؼ�������
	bool LoadSkill();

	//���ص�����Ϣ���ݿ�
	bool LoadItemList();
	//����װ����Ϣ���ݿ�
	bool LoadEquipList();

	//���ع���ֲ���Ϣ���ݿ�
	bool LoadDistribute();
	//������ͨ����Ϣ���ݿ�
	bool LoadMonster();
	//����BOSS����Ϣ���ݿ�
	bool LoadBoss();
	//���ع��ﱩ���趨���ݿ�
	bool LoadDropSet();

	bool silentSave();
	bool updateSaveFileVersion();

	void timerEvent(QTimerEvent *event);

private slots:
	void tabChanged(int index);

	void on_mirror_save();
	void enable_autoSave(bool);
	void enable_bkSound(bool);

private:
	Ui::mirrorClass ui;

	RoleInfo roleInfo;			//��ɫ������Ϣ
	VecRoleSkill m_skill_study;	//��ɫ��ѧ��ļ���
	MapItem m_bag_item;			//���߱���
	MapItem m_storage_item;		//���ֿ߲�
	ListEquip m_bag_equip;		//װ������
	ListEquip m_storage_equip;	//װ���ֿ�

	role *m_tab_role;
	fight *m_tab_fight;
	city *m_tab_city;

	quint32 nSaveTimer;

	QMediaPlayer *bgAudio;
	QMediaPlaylist *bgAudioList;
};

#endif // MIRROR_H
