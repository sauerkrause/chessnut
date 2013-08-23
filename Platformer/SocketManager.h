#ifndef __SOCKET_MANAGER_H_INCLUDED__
#define __SOCKET_MANAGER_H_INCLUDED__

#include <winsock.h>

//Specify version of winsock used.
#define WINSOCK_VERSION MAKEWORD(1,1)



//winsock error reporting

struct WSAEDESCRIPTION
{
  int nErrorCode;
  char *szDescription;
};


class SocketManager
{

public:
	SocketManager(char *addr, int port, bool isUDP); //Constructor
	virtual ~SocketManager(); //Destructor
	bool SendPacket(char *message, int length); //Send packet
	bool SendBinaryPacket(DWORD data);
	LPCTSTR WinsockErrorDescription(int nErrorCode); //Describes Winsock error codes.

private:
	bool m_bInitialized; //True if socket is initialized.
	SOCKADDR_IN m_saClient; //Client socket info
	SOCKET m_Socket; //The socket

};

#endif