#include "error.h"

info_error errrDefine[errorCount] =
{
	{ mirErr_Unknow, "δ֪����" },
	{ mirErr_XSpeed, "��⵽�������" }
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