#pragma once

#include <winsock2.h>
#include <string>
#include <vector>

typedef void (*NetThread)(void *para);



class CUDPConnection
{
public:
	CUDPConnection(void);
	~CUDPConnection(void);

	int m_sockfd;
	SOCKADDR_IN sever_addr ;
	
	char m_szSendBuff[2048];

	bool m_bSendInit;
	bool m_bReceivedInit;
	

	void InitConnection(const char *szServerIP,DWORD dwServerPort,DWORD dwClientPort = 0);
	void CloseConnection();
	void RunThread(NetThread thread,void *para);//һ�����ڼ����߳�

	void  CUDPConnection::Send(std::string strContent);
	
	void Recv_Test();	//���ն����ݽ���ʾ��
	void InitSkelNames();
	
};
