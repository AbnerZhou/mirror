#ifndef MIRROR_H
#define MIRROR_H

#include <QtWidgets/QMainWindow>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QSystemTrayIcon>
#include <QtWinExtras/QtWinExtras>
#include "ui_mirror.h"

#include "role.h"
#include "fight.h"
#include "city.h"

class mirror : public QMainWindow
{
	Q_OBJECT

public:
	quint8 markCount = 60;

public:
	mirror(QWidget *parent = 0);
	~mirror();

	//�ӽ�ɫһЩ���ߣ��˺�����Ҫ�������Ե��ߡ�
	void GiveSomeItem(void);

protected:
	void closeEvent(QCloseEvent *event);
	void changeEvent(QEvent *e);

private:
	//
	bool LoadVerify();
	//���ش浵
	bool LoadRole();
	//����ְҵ�ӳ���Ϣ���ݿ�
	bool LoadJobSet();
	//���ؼ�������
	bool LoadSkill();
	bool LoadBuff();

	//���ص�����Ϣ���ݿ�
	bool LoadItemList();
	//����װ����Ϣ���ݿ�
	bool LoadEquipList();
	//��������װ���б�
	bool LoadStateEquip();

	//���ع���ֲ���Ϣ���ݿ�
	bool LoadDistribute();
	//������ͨ����Ϣ���ݿ�
	bool LoadMonster();
	//����BOSS����Ϣ���ݿ�
	bool LoadBoss();
	//���ع��ﱩ���趨���ݿ�
	bool LoadDropSet();

	//������������
	bool LoadTaskSet();

	bool LoadFormula();

	void initUi(void);
	void initMarkByte(void);
	bool verifyRoleInfo();
	bool verifyXSpeed(QDateTime time_c);
	bool silentSave();
	bool updateSaveFileVersion();

	void timerEvent(QTimerEvent *event);

private slots:
	void on_btn_fight_clicked(void);
	void on_btn_role_clicked(void);
	void on_btn_city_clicked(void);

	void on_btn_skill_clicked();
	void on_btn_task_clicked();

	void on_action_limit(bool checked);
	void on_action_about(bool checked);
	void on_action_help(bool checked);
	
	void on_mirror_save();
	void enable_bkSound(bool);

	void iconActivated(QSystemTrayIcon::ActivationReason);

private:
	Ui::mirrorClass ui;
	bool bFirstMinimum;

	RoleInfo roleInfo;			//��ɫ������Ϣ
	MapRoleSkill m_skill_study;	//��ɫ��ѧ��ļ���
	MapItem m_bag_item;			//���߱���
	MapItem m_storage_item;		//���ֿ߲�
	ListEquip m_bag_equip;		//װ������
	ListEquip m_storage_equip;	//װ���ֿ�

	role *m_tab_role;
	fight *m_tab_fight;
	city *m_tab_city;

	quint32 nSaveTimer;

	HANDLE hVerify;

	QMediaPlayer *bgAudio;
	QMediaPlaylist *bgAudioList;
	QSystemTrayIcon *trayIcon;

	QMenu *popMenu;
	QAction *action_limit, *action_about, *action_help;
};

#endif // MIRROR_H
