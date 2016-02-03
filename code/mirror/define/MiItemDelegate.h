#ifndef MI_ITEM_DELEGATE_H
#define MI_ITEM_DELEGATE_H

#include <QItemDelegate>

//����װ������Ԫ�񱳾�Ϊװ��Ʒ�ʿ�Ȼ����ʾ������ʾװ�������½���ʾ���������Ͻ���ʾǿ������
class MiItemDelegate : public QItemDelegate
{
	Q_OBJECT
public:
	MiItemDelegate(QObject *parent = 0) : QItemDelegate(parent) { }
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex & index) const;
	void drawDisplay(QPainter *painter, const QPoint &pos, const QSize &ItemGridSize, QVariant value) const;
};

#endif // MI_ITEM_DELEGATE_H
