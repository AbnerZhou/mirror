#include "cryptography.h"

cryptography::cryptography(QObject *parent)
	: QObject(parent)
{

}

cryptography::~cryptography()
{

}

/*
a��������Ч�浵��MD5�롣
b����a�Ľ������base64���룬�õ�44�ֽ��ı���ÿ4�ֽڱ�Ϊһ�飬��11�顣
c���������һ��2000�ֽڵ������ı���ascii 65-122)��
d��ȡc�ı��ĵ�100*n�ֽڵ�ascii�� - 65��Ϊ��ʼλ�ã�����д��c����ĵ�n�鵽c�С�
e����д���c�����2000�ֽ���Ϊ�浵����ʼͷ��
*/
bool cryptography::Encrypt(QByteArray &dst, const QByteArray &src)
{
	dst.clear();

	QByteArray MD5arr = QCryptographicHash::hash(src, QCryptographicHash::Md5).toHex();
	QByteArray TmpArr1,TmpArr2;
	QString strTmp;
	char ch;
	int nIndex;

 	TmpArr1 = MD5arr.toBase64();
 
	for (qint32 i = 0; i < 2000; i++)
	{
		ch = qrand() % (122 - 65) + 65;
		TmpArr2.append(ch);
	}

	for (qint32 i = 0; i < 11; i++)
	{
		nIndex = TmpArr2.at(i * 100) - 60 + i * 100;
		TmpArr2.replace(nIndex, 4, TmpArr1.mid(i * 4, 4));
	}

	dst = TmpArr2;
	return true;
}
bool cryptography::Decrypt(QByteArray &dst, const QByteArray &src)
{
	if (src.size() < 2000)
	{
		return false;
	}
	dst.clear();

	QByteArray MD5arr;
	QByteArray TmpArr1, TmpArr2;
	QString strTmp;
	int nIndex;

	for (qint32 i = 0; i < 11; i++)
	{
		nIndex = src.at(i * 100) - 60 + i * 100;
		TmpArr2.append(src.mid(nIndex, 4));
	}

	TmpArr1 = QByteArray::fromBase64(TmpArr2);

	MD5arr = TmpArr1;
	dst = MD5arr;

	return true;
}
