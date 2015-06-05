#include "mirrorlog.h"

void MirrorLog::init(int _logLevel, std::string _logFileName)
{
	m_loglevel = _logLevel;
	m_logfilename = _logFileName;
	delete_oldLog();
}

void MirrorLog::append(int _logLevel, std::string FunctionName, int errorcode)
{	
	//д��־���ж����м���
	if (_logLevel < m_loglevel)
	{
		return;
	}

	lock.lock();
	std::fstream out(m_logfilename.c_str(), std::ios::out | std::ios::app);
	out << get_time(TIME);
	out << std::string(" ") << _logLevel << std::string(" ") << FunctionName << std::string("\t");
	out << errorcode << std::string("\t") << error2str(errorcode) << std::endl;

	out.close();
	lock.unlock();
}

void MirrorLog::delete_oldLog()
{
	std::ifstream in(m_logfilename.c_str()); // ����־�ļ�
	std::stringstream ss;
	std::string s, sTmp;//�����־�ļ����������ݺ�ɾ��ǰһ��֮ǰ����־�������
	std::size_t w;//��λǰһ�����־λ��

	ss << in.rdbuf(); // ���ļ�����buffer�ŵ�stringstream��
	s = ss.str(); // ��ss������Ϊһ��string����s���ļ��������ݾ���s��

	in.close();

	// ... ����s
	if (!s.size())
		return;

	w = s.find(get_time(GET_TIME_OF_DAY));
	if (w == std::string::npos)
	{
		w = s.find(get_time(GET_TIME_OF_YESTERDAY));
		if (w == std::string::npos)
		{
			sTmp = "";
		}
		else
		{
			sTmp = s.substr(w);
		}
	}
	else
	{
		sTmp = s.substr(w);
	}

	// ��s����д���ļ�
	std::ofstream out(m_logfilename.c_str());
	out << sTmp;
	out.close();
}

std::string MirrorLog::get_time(int flag)
{
	char tmp[64];
	tm tm;
	
	if (flag == TIME)
	{
		time_t t = time(0);
		localtime_s(&tm, &t);
		strftime(tmp, sizeof(tmp), "%m-%d %H:%M:%S", &tm);
	}
	else if (flag == GET_TIME_OF_DAY)
	{
		time_t t = time(0);
		localtime_s(&tm, &t);
		strftime(tmp, sizeof(tmp), "%m-%d", &tm);
	}
	else if (flag == GET_TIME_OF_YESTERDAY)
	{
		time_t t = time(0) - 24 * 3600;
		localtime_s(&tm, &t);
		strftime(tmp, sizeof(tmp), "%m-%d", &tm);
	}
	else
	{
		time_t t = time(0);
		localtime_s(&tm, &t);
		strftime(tmp, sizeof(tmp), "%m-%d %H:%M:%S", &tm);
	}

	return std::string(tmp);
}


