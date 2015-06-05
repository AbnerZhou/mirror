#ifndef __MIRRORLOG_H__
#define __MIRRORLOG_H__

#include <iostream>
#include <mutex>
#include <fstream>
#include <sstream>
#include <ctime>

#include "error.h"

//��־���𣬹�4�֣��ֱ�Ϊfatal, error, warning, info
#define LEVEL_FATAL		3
#define LEVEL_ERROR		2
#define LEVEL_WARNING	1
#define LEVEL_INFO		0

//��ȡʱ����������죬ΪGET_TIME_OF_DAT,����ΪTIME��ȡ����ΪGET_TIME_OF_YESTERDAY
#define GET_TIME_OF_YESTERDAY	2
#define GET_TIME_OF_DAY			1
#define TIME					0

class MirrorLog
{
public:
	static class MirrorLog& get_logClass()
	{
		static MirrorLog mirLog;   //�ֲ���̬����  
		return mirLog;
	}

	void init(int _logLevel, std::string _logFileName = "mirror.log");
	void append(int _logLevel, std::string FunctionName, int errorcode);

private:
	MirrorLog() { ; }	//���캯��˽��
	MirrorLog(const MirrorLog &);
	MirrorLog & operator = (const MirrorLog &);

	void delete_oldLog();
	std::string get_time(int flag = TIME);

	std::string m_logfilename;
	int m_loglevel;
	std::mutex lock;
};

#define LogIns MirrorLog::get_logClass()

#endif //__MIRRORLOG_H__
