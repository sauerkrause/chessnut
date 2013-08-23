#include "SocketManager.h"

WSAEDESCRIPTION g_WSAErrorDescriptions[]={
  WSAEINTR,
  "WSAEINTR: Interrupted function call",
  WSAEBADF,
  "WSAEBADF: Bad file number",
  WSAEACCES,
  "WSAEACCES: Permission denied",
  WSAEFAULT,
  "WSAEFAULT: Address fault",
  WSAEINVAL,
  "WSAEINVAL: Invalid argument",
  WSAEMFILE,
  "WSAEMFILE: Too many open files",
  WSAEWOULDBLOCK,
  "WSAEWOULDBLOCK: Resource temporarily unavailable (Would block)",
  WSAEINPROGRESS,
  "WSAEINPROGRESS: Operation now in progress",
  WSAEALREADY,
  "WSAEALREADY: Operation already in progress",
  WSAENOTSOCK,
  "WSAENOTSOCK: Socket operation on non-socket",
  WSAEDESTADDRREQ,
  "WSAEDESTADDRREQ: Destination address required",
  WSAEMSGSIZE,
  "WSAEMSGSIZE: Message too long",
  WSAEPROTOTYPE,
  "WSAEPROTOTYPE: Protocol wrong type for socket",
  WSAENOPROTOOPT,
  "WSAENOPROTOOPT: Bad protocol option",
  WSAEPROTONOSUPPORT,
  "WSAEPROTONOSUPPORT: Protocol not supported",
  WSAESOCKTNOSUPPORT,
  "WSAESOCKTNOSUPPORT: Socket type not supported",
  WSAEOPNOTSUPP,
  "WSAEOPNOTSUPP: Operation not supported",
  WSAEPFNOSUPPORT,
  "WSAEPFNOSUPPORT: Protocol family not supported",
  WSAEAFNOSUPPORT,
  "WSAEAFNOSUPPORT: Address family not supported by protocol family",
  WSAEADDRINUSE,
  "WSAEADDRINUSE: Address already in use",
  WSAEADDRNOTAVAIL,
  "WSAEADDRNOTAVAIL: Cannot assign requested address",
  WSAENETDOWN,
  "WSAENETDOWN: Network is down",
  WSAENETUNREACH,
  "WSAENETUNREACH: Network is unreachable",
  WSAENETRESET,
  "WSAENETRESET: Network dropped connection on reset",
  WSAECONNABORTED,
  "WSAECONNABORTED: Software caused connection abort",
  WSAECONNRESET,
  "WSAECONNRESET: Connection reset by peer",
  WSAENOBUFS,
  "WSAENOBUFS: No buffer space available",
  WSAEISCONN,
  "WSAEISCONN: Socket is already connected",
  WSAENOTCONN,
  "WSAENOTCONN: Socket is not connected",
  WSAESHUTDOWN,
  "WSAESHUTDOWN: Cannot send after socket shutdown",
  WSAETOOMANYREFS,
  "WSAETOOMANYREFS: Too many references",
  WSAETIMEDOUT,
  "WSAETIMEDOUT: Connection timed out",
  WSAECONNREFUSED,
  "WSAECONNREFUSED: Connection refused",
  WSAELOOP,
  "WSAELOOP: Loop",
  WSAENAMETOOLONG,
  "WSAENAMETOOLONG: Name too long",
  WSAEHOSTDOWN,
  "WSAEHOSTDOWN: Host is down",
  WSAEHOSTUNREACH,
  "WSAEHOSTUNREACH: No route to host",
  WSAENOTEMPTY,
  "WSAENOTEMPTY: Directory not empty",
  WSAEPROCLIM,
  "WSAEPROCLIM: Too many processes",
  WSAEUSERS,
  "WSAEUSERS: Too many users",
  WSAEDQUOT,
  "WSAEDQUOT: Disk quota",
  WSAESTALE,
  "WSAESTALE: Stale",
  WSAEREMOTE,
  "WSAEREMOTE: Remote",
  WSASYSNOTREADY,
  "WSASYSNOTREADY: Network subsystem is unavailable",
  WSAVERNOTSUPPORTED,
  "WSAVERNOTSUPPORTED: WINSOCK.DLL version out of range",
  WSANOTINITIALISED,
  "WSANOTINITIALISED: Successful WSAStartup() not yet performed",
  WSAEDISCON,
  "WSAEDISCON: Graceful shutdown in progress",
  WSAHOST_NOT_FOUND,
  "WSAHOST_NOT_FOUND: Host not found",
  WSATRY_AGAIN,
  "WSATRY_AGAIN: Non-authoritative host not found",
  WSANO_RECOVERY,
  "WSANO_RECOVERY: Non-recoverable error",
  WSANO_DATA,
  "WSANO_DATA: Valid name, no data record of requested type",
#ifdef _WINSOCK2API_
  WSAENOMORE,
  "WSAENOMORE: ",
  WSAECANCELLED,
  "WSAECANCELLED: Cancelled",
  WSAEINVALIDPROCTABLE,
  "WSAEINVALIDPROCTABLE: Invalid procedure table from service provider",
  WSAEINVALIDPROVIDER,
  "WSAEINVALIDPROVIDER: Invalid service provider version number",
  WSAEPROVIDERFAILEDINIT,
  "WSAEPROVIDERFAILEDINIT: Unable to initialize a service provider",
  WSASYSCALLFAILURE,
  "WSASYSCALLFAILURE: System call failure",
  WSASERVICE_NOT_FOUND,
  "WSASERVICE_NOT_FOUND: Service not found",
  WSATYPE_NOT_FOUND,
  "WSATYPE_NOT_FOUND: Type not found",
  WSA_E_NO_MORE,
  "WSA_E_NO_MORE: ",
  WSA_E_CANCELLED,
  "WSA_E_CANCELLED: ",
  WSAEREFUSED,
  "WSAEREFUSED: Refused",
#endif
};

//Socket Manager implementation

//Constructor opens udp socket to given IP address and socket

SocketManager::SocketManager(char *address, int port, bool isUDP)
{
	WSADATA wsaData;
	m_bInitialized = true;

	//Start winsock
	m_bInitialized = m_bInitialized && (WSAStartup(WINSOCK_VERSION,&wsaData) == 0); //Can it init
	m_bInitialized = m_bInitialized && (wsaData.wVersion == WINSOCK_VERSION); //Do we have the right version?

	//create a UDP/IP Socket
	m_Socket = INVALID_SOCKET;
	if(m_bInitialized)
	{	
		if(isUDP)
			m_Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); //Create UDP socket
		else
			m_Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_TCP); //Create TCP socket
	}
	m_bInitialized = m_bInitialized && m_Socket != INVALID_SOCKET;

	if(m_bInitialized)
	{
		m_saClient.sin_family=AF_INET; //family
		m_saClient.sin_port=htons((u_short)port); //port number
		unsigned int addr = inet_addr(address); //ip address of server
		CopyMemory((char*)&(m_saClient.sin_addr),(char*)&addr,4); //ip address of client
	}
	else
	{
		//We fail and must bail
		WSACleanup();
	}
}	

SocketManager::~SocketManager()
{
	if(m_bInitialized)
	{
		closesocket(m_Socket);
		WSACleanup();
	}
	//else we've already closed it.
}

/// Format an error description
LPCTSTR SocketManager::WinsockErrorDescription(int nErrorCode)
{
	//get the size of the error code.
	const int size = sizeof(g_WSAErrorDescriptions)/sizeof(g_WSAErrorDescriptions[0]);

	for(int i=0; i < size; i++)
	{
		if(g_WSAErrorDescriptions[i].nErrorCode == nErrorCode)
			return((LPCTSTR)g_WSAErrorDescriptions[i].szDescription);
	}
	return ((LPCTSTR)"Unknown error");
}

//Send a packet with message and length of message

bool SocketManager::SendPacket(char *message, int length)
{
	if(!m_bInitialized)
		return false;
	int nRet=sendto(m_Socket, message, length, 0, (LPSOCKADDR)&m_saClient, 
		sizeof(struct sockaddr));
	if(nRet==SOCKET_ERROR)
		return false;
	return true;

}

//Send a packet containing a DWORD in binary.

bool SocketManager::SendBinaryPacket(DWORD data)
{
	char message[4];
	BYTE binMessage[4];
	DWORD dataLocal = data;
	DWORD dataMask = 0x000000ff;
	DWORD tmp;
	//create the string to be used as a packet from the DWORD.
	for(int i = 0; i < 4; i++)
	{
		tmp = dataLocal & dataMask;
		dataMask = ((dataMask) << 8);
		tmp = ((tmp) >> (i * 8));
		binMessage[i] = (BYTE)tmp;
		message[i] = (char)binMessage[i];

	}
	return SendPacket(message, 4);
}