#include "error.h"

const int errorCount = 4;

info_error errrDefine[errorCount] =
{
	{ mirErr_Unknow, "δ֪����" },
	{ mirErr_XSpeed, "��⵽�������" },
	{ mirErr_Modify, "�浵���޸�" },
	{ mirErr_MD5, "MD5У��δͨ��" }
};

std::string & error2str(int Ecode)
{
	for (int i = 0; i < errorCount; i++)
	{
		if (errrDefine[i].id == Ecode)
		{
			return errrDefine[i].msg;
		}
	}
	return errrDefine[0].msg;
}