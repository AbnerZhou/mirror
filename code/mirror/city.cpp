#include "city.h"

city::city(RoleInfo *roleInfo, MapItem *bag_item)
: myTabFrame(NULL), myRole(roleInfo), m_bag_item(bag_item)
{
	ui.setupUi(this);

	m_drugs = new shop(0, myRole, m_bag_item);
	ui.tabWidget->addTab(m_drugs, QStringLiteral("ҩ��"));

	m_variety = new shop(1, myRole, m_bag_item);
	ui.tabWidget->addTab(m_variety, QStringLiteral("�ӻ���"));

	m_smithy = new smithy(myRole);
	ui.tabWidget->addTab(m_smithy, QStringLiteral("������"));
	tabChanged(0);

	connect(ui.tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
}

city::~city()
{
	delete m_drugs;
	delete m_smithy;
}

void city::updateRoleInfo(void)
{

}

void city::tabChanged(int index)
{
	QString iconPath = ":/city/Resources/city/";
	iconPath += QString::number(index + 1) + ".png";
	ui.lbl_img->setPixmap(QPixmap(iconPath));

	QString strInfo[10] = {
		QStringLiteral("���ֻش�"),
		QStringLiteral("��������"),
		QStringLiteral("װ��ǿ��"),
		QStringLiteral("ת��")
	};
	ui.edit_display->setText(strInfo[index]);
}