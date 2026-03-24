#include "stdafx.h"
#include "InputOutputStream.h"
#include "SocketAddress.h"
#include "Socket.h"
#include "ThreadName.h"
#include "..\Minecraft.Client\ServerConnection.h"
#include <algorithm>
#include "..\Minecraft.Client\PS3\PS3Extras\ShutdownManager.h"

// This current socket implementation is for the creation of a single local link. 2 sockets can be created, one for either end of this local
// link, the end (0 or 1) is passed as a parameter to the ctor.

CRITICAL_SECTION Socket::s_hostQueueLock[2]; 
std::queue<byte> Socket::s_hostQueue[2];
Socket::SocketOutputStreamLocal *Socket::s_hostOutStream[2];
Socket::SocketInputStreamLocal *Socket::s_hostInStream[2];
ServerConnection *Socket::s_serverConnection = NULL;

void Socket::Initialise(ServerConnection *serverConnection)
{
	s_serverConnection = serverConnection;

	// Only initialise everything else once - just setting up static data, one time xrnm things, thread for ticking sockets
	static bool init = false;
	if( init )
	{
		for( int i = 0; i < 2; i++ )
		{
			if(TryEnterCriticalSection(&s_hostQueueLock[i]))
			{
				// Clear the queue
				std::queue<byte> empty;
				std::swap( s_hostQueue[i], empty );
				LeaveCriticalSection(&s_hostQueueLock[i]);
			}
			s_hostOutStream[i]->m_streamOpen = true;
			s_hostInStream[i]->m_streamOpen = true;
		}
#ifdef _DEDICATED_SERVER
		// Second call: server connection is now set, start TCP listener
		if (serverConnection != nullptr)
		{
			StartTCPListener();
		}
#endif
		return;
	}
	init = true;

	for( int i = 0; i < 2; i++ )
	{
		InitializeCriticalSection(&Socket::s_hostQueueLock[i]);
		s_hostOutStream[i] = new SocketOutputStreamLocal(i);
		s_hostInStream[i] = new SocketInputStreamLocal(i);
	}
}

Socket::Socket(bool response)
{
	m_hostServerConnection = true;
	m_hostLocal = true;
#ifdef _DEDICATED_SERVER
	m_isTCP = false;
	m_tcpSocket = (ULONG_PTR)INVALID_SOCKET;
#endif
	if( response )
	{
		m_end = SOCKET_SERVER_END;
	}
	else
	{
		m_end = SOCKET_CLIENT_END;
		Socket *socket = new Socket(true);
		s_serverConnection->NewIncomingSocket(socket);
	}

	for( int i = 0; i < 2; i++ )
	{
		m_endClosed[i] = false;
	}
	m_socketClosedEvent = NULL;
	createdOk = true;
	networkPlayerSmallId = g_NetworkManager.GetHostPlayer()->GetSmallId();
}

Socket::Socket(INetworkPlayer *player, bool response /* = false*/, bool hostLocal /*= false*/)
{
	m_hostServerConnection = false;
	m_hostLocal = hostLocal;
#ifdef _DEDICATED_SERVER
	m_isTCP = false;
	m_tcpSocket = (ULONG_PTR)INVALID_SOCKET;
#endif

	for( int i = 0; i < 2; i++ )
	{
		InitializeCriticalSection(&m_queueLockNetwork[i]);		
		m_inputStream[i] = NULL;
		m_outputStream[i] = NULL;		
		m_endClosed[i] = false;
	}
	
	if(!response || hostLocal)
	{
		m_inputStream[0] = new SocketInputStreamNetwork(this,0);
		m_outputStream[0] = new SocketOutputStreamNetwork(this,0);
		m_end = SOCKET_CLIENT_END;
	}
	if(response || hostLocal)
	{
		m_inputStream[1] = new SocketInputStreamNetwork(this,1);
		m_outputStream[1] = new SocketOutputStreamNetwork(this,1);
		m_end = SOCKET_SERVER_END;
	}
	m_socketClosedEvent = new C4JThread::Event;
	//printf("New socket made %s\n", player->GetGamertag() );
	networkPlayerSmallId = player->GetSmallId();
	createdOk = true;
}

SocketAddress *Socket::getRemoteSocketAddress()
{
	return NULL;
}

INetworkPlayer *Socket::getPlayer()
{
	return g_NetworkManager.GetPlayerBySmallId(networkPlayerSmallId);
}

void Socket::setPlayer(INetworkPlayer *player)
{
	if(player!=NULL)
	{
		networkPlayerSmallId = player->GetSmallId();
	}
	else
	{
		networkPlayerSmallId = 0;
	}
}

void Socket::pushDataToQueue(const BYTE * pbData, DWORD dwDataSize, bool fromHost /*= true*/)
{
	int queueIdx = SOCKET_CLIENT_END;
	if(!fromHost)
		queueIdx = SOCKET_SERVER_END;

	if( queueIdx != m_end && !m_hostLocal )
	{
		app.DebugPrintf("SOCKET: Error pushing data to queue. End is %d but queue idx id %d\n", m_end, queueIdx);
		return;
	}

	EnterCriticalSection(&m_queueLockNetwork[queueIdx]);
	for( unsigned int i = 0; i < dwDataSize; i++ )
	{
		m_queueNetwork[queueIdx].push(*pbData++);
	}
	LeaveCriticalSection(&m_queueLockNetwork[queueIdx]);
}

void Socket::addIncomingSocket(Socket *socket)
{
	if( s_serverConnection != NULL )
	{
		s_serverConnection->NewIncomingSocket(socket);
	}
}

InputStream *Socket::getInputStream(bool isServerConnection)
{
	if( !m_hostServerConnection )
	{
		if( m_hostLocal )
		{
			if( isServerConnection )
			{
				return m_inputStream[SOCKET_SERVER_END];
			}
			else
			{
				return m_inputStream[SOCKET_CLIENT_END];
			}
		}
		else
		{
			return m_inputStream[m_end];
		}
	}
	else
	{
		return s_hostInStream[m_end];
	}
}

void Socket::setSoTimeout(int a )
{
}

void Socket::setTrafficClass( int a )
{
}

Socket::SocketOutputStream *Socket::getOutputStream(bool isServerConnection)
{
	if( !m_hostServerConnection )
	{
		if( m_hostLocal )
		{
			if( isServerConnection )
			{
				return m_outputStream[SOCKET_SERVER_END];
			}
			else
			{
				return m_outputStream[SOCKET_CLIENT_END];
			}
		}
		else
		{
			return m_outputStream[m_end];
		}
	}
	else
	{
		return s_hostOutStream[ 1 - m_end ];
	}
}

bool Socket::close(bool isServerConnection)
{
	bool allClosed = false;
	if( m_hostLocal )
	{
		if( isServerConnection )
		{
			m_endClosed[SOCKET_SERVER_END] = true;
			if(m_endClosed[SOCKET_CLIENT_END])
			{
				allClosed = true;
			}
		}
		else
		{
			m_endClosed[SOCKET_CLIENT_END] = true;
			if(m_endClosed[SOCKET_SERVER_END])
			{
				allClosed = true;
			}
		}
	}
	else
	{
		allClosed = true;
		m_endClosed[m_end] = true;
	}
	if( allClosed && m_socketClosedEvent != NULL )
	{
		m_socketClosedEvent->Set();
	}
	if(allClosed) createdOk = false;
	return allClosed;
}

/////////////////////////////////// Socket for input, on local connection ////////////////////

Socket::SocketInputStreamLocal::SocketInputStreamLocal(int queueIdx)
{
	m_streamOpen = true;
	m_queueIdx = queueIdx;
}

// Try and get an input byte, blocking until one is available
int Socket::SocketInputStreamLocal::read()
{
	while(m_streamOpen && ShutdownManager::ShouldRun(ShutdownManager::eConnectionReadThreads))
	{
		if(TryEnterCriticalSection(&s_hostQueueLock[m_queueIdx]))
		{
			if( s_hostQueue[m_queueIdx].size() )
			{
				byte retval = s_hostQueue[m_queueIdx].front();
				s_hostQueue[m_queueIdx].pop();
				LeaveCriticalSection(&s_hostQueueLock[m_queueIdx]);
				return retval;
			}
			LeaveCriticalSection(&s_hostQueueLock[m_queueIdx]);
		}
		Sleep(1);
	}
	return -1;
}

// Try and get an input array of bytes, blocking until enough bytes are available
int Socket::SocketInputStreamLocal::read(byteArray b)
{
	return read(b, 0, b.length);
}

// Try and get an input range of bytes, blocking until enough bytes are available
int Socket::SocketInputStreamLocal::read(byteArray b, unsigned int offset, unsigned int length)
{
	while(m_streamOpen)
	{
		if(TryEnterCriticalSection(&s_hostQueueLock[m_queueIdx]))
		{
			if( s_hostQueue[m_queueIdx].size() >= length )
			{
				for( unsigned int i = 0; i < length; i++ )
				{
					b[i+offset] = s_hostQueue[m_queueIdx].front();
					s_hostQueue[m_queueIdx].pop();
				}
				LeaveCriticalSection(&s_hostQueueLock[m_queueIdx]);
				return length;
			}
			LeaveCriticalSection(&s_hostQueueLock[m_queueIdx]);
		}
		Sleep(1);
	}
	return -1;
}

void Socket::SocketInputStreamLocal::close()
{
	m_streamOpen = false;
	EnterCriticalSection(&s_hostQueueLock[m_queueIdx]);
	s_hostQueue[m_queueIdx].empty();
	LeaveCriticalSection(&s_hostQueueLock[m_queueIdx]);
}

/////////////////////////////////// Socket for output, on local connection ////////////////////

Socket::SocketOutputStreamLocal::SocketOutputStreamLocal(int queueIdx)
{
	m_streamOpen = true;
	m_queueIdx = queueIdx;
}

void Socket::SocketOutputStreamLocal::write(unsigned int b)
{
	if( m_streamOpen != true )
	{
		return;
	}
	EnterCriticalSection(&s_hostQueueLock[m_queueIdx]);
	s_hostQueue[m_queueIdx].push((byte)b);
	LeaveCriticalSection(&s_hostQueueLock[m_queueIdx]);
}

void Socket::SocketOutputStreamLocal::write(byteArray b)
{
	write(b, 0, b.length);
}

void Socket::SocketOutputStreamLocal::write(byteArray b, unsigned int offset, unsigned int length)
{
	if( m_streamOpen != true )
	{
		return;
	}
	MemSect(12);
	EnterCriticalSection(&s_hostQueueLock[m_queueIdx]);
	for( unsigned int i = 0; i < length; i++ )
	{
		s_hostQueue[m_queueIdx].push(b[offset+i]);
	}
	LeaveCriticalSection(&s_hostQueueLock[m_queueIdx]);
	MemSect(0);
}

void Socket::SocketOutputStreamLocal::close()
{
	m_streamOpen = false;
	EnterCriticalSection(&s_hostQueueLock[m_queueIdx]);
	s_hostQueue[m_queueIdx].empty();
	LeaveCriticalSection(&s_hostQueueLock[m_queueIdx]);
}

/////////////////////////////////// Socket for input, on network connection ////////////////////

Socket::SocketInputStreamNetwork::SocketInputStreamNetwork(Socket *socket, int queueIdx)
{
	m_streamOpen = true;
	m_queueIdx = queueIdx;
	m_socket = socket;
}

// Try and get an input byte, blocking until one is available
int Socket::SocketInputStreamNetwork::read()
{
	while(m_streamOpen && ShutdownManager::ShouldRun(ShutdownManager::eConnectionReadThreads))
	{
		if(TryEnterCriticalSection(&m_socket->m_queueLockNetwork[m_queueIdx]))
		{
			if( m_socket->m_queueNetwork[m_queueIdx].size() )
			{
				byte retval = m_socket->m_queueNetwork[m_queueIdx].front();
				m_socket->m_queueNetwork[m_queueIdx].pop();
				LeaveCriticalSection(&m_socket->m_queueLockNetwork[m_queueIdx]);
				return retval;
			}
			LeaveCriticalSection(&m_socket->m_queueLockNetwork[m_queueIdx]);
		}
		Sleep(1);
	}
	return -1;
}

// Try and get an input array of bytes, blocking until enough bytes are available
int Socket::SocketInputStreamNetwork::read(byteArray b)
{
	return read(b, 0, b.length);
}

// Try and get an input range of bytes, blocking until enough bytes are available
int Socket::SocketInputStreamNetwork::read(byteArray b, unsigned int offset, unsigned int length)
{
	while(m_streamOpen)
	{
		if(TryEnterCriticalSection(&m_socket->m_queueLockNetwork[m_queueIdx]))
		{
			if( m_socket->m_queueNetwork[m_queueIdx].size() >= length )
			{
				for( unsigned int i = 0; i < length; i++ )
				{
					b[i+offset] = m_socket->m_queueNetwork[m_queueIdx].front();
					m_socket->m_queueNetwork[m_queueIdx].pop();
				}
				LeaveCriticalSection(&m_socket->m_queueLockNetwork[m_queueIdx]);
				return length;
			}
			LeaveCriticalSection(&m_socket->m_queueLockNetwork[m_queueIdx]);
		}
		Sleep(1);
	}
	return -1;
}

void Socket::SocketInputStreamNetwork::close()
{
	m_streamOpen = false;
}

/////////////////////////////////// Socket for output, on network connection ////////////////////

Socket::SocketOutputStreamNetwork::SocketOutputStreamNetwork(Socket *socket, int queueIdx)
{
	m_queueIdx = queueIdx;
	m_socket = socket;
	m_streamOpen = true;
}

void Socket::SocketOutputStreamNetwork::write(unsigned int b)
{
	if( m_streamOpen != true ) return;
	byteArray barray;
	byte bb;
	bb = (byte)b;
	barray.data = &bb;
	barray.length = 1;
	write(barray, 0, 1);

}

void Socket::SocketOutputStreamNetwork::write(byteArray b)
{
	write(b, 0, b.length);
}

void Socket::SocketOutputStreamNetwork::write(byteArray b, unsigned int offset, unsigned int length)
{	
	writeWithFlags(b, offset, length, 0);
}

void Socket::SocketOutputStreamNetwork::writeWithFlags(byteArray b, unsigned int offset, unsigned int length, int flags)
{
	if( m_streamOpen != true ) return;
	if( length == 0 ) return;

	// If this is a local connection, don't bother going through QNet as it just delivers it straight anyway
	if( m_socket->m_hostLocal )
	{
		// We want to write to the queue for the other end of this socket stream
		int queueIdx = m_queueIdx;
		if(queueIdx == SOCKET_CLIENT_END)
			queueIdx = SOCKET_SERVER_END;
		else
			queueIdx = SOCKET_CLIENT_END;

		EnterCriticalSection(&m_socket->m_queueLockNetwork[queueIdx]);
		for( unsigned int i = 0; i < length; i++ )
		{
			m_socket->m_queueNetwork[queueIdx].push(b[offset+i]);
		}
		LeaveCriticalSection(&m_socket->m_queueLockNetwork[queueIdx]);
	}
	else
	{
		XRNM_SEND_BUFFER buffer;
		buffer.pbyData = &b[offset];
		buffer.dwDataSize = length;

		INetworkPlayer *hostPlayer = g_NetworkManager.GetHostPlayer();
		if(hostPlayer == NULL)
		{
			app.DebugPrintf("Trying to write to network, but the hostPlayer is NULL\n");
			return;
		}
		INetworkPlayer *socketPlayer = m_socket->getPlayer();
		if(socketPlayer == NULL)
		{
			app.DebugPrintf("Trying to write to network, but the socketPlayer is NULL\n");
			return;
		}

#ifdef _XBOX
		bool lowPriority = ( ( flags & QNET_SENDDATA_LOW_PRIORITY )		== QNET_SENDDATA_LOW_PRIORITY );
		bool requireAck = lowPriority;
#else
		bool lowPriority = false;
		bool requireAck =  ( ( flags & NON_QNET_SENDDATA_ACK_REQUIRED ) == NON_QNET_SENDDATA_ACK_REQUIRED );
#endif

		if( m_queueIdx == SOCKET_SERVER_END )
		{
			//printf( "Sent %u bytes of data from \"%ls\" to \"%ls\"\n",
			//buffer.dwDataSize,
			//hostPlayer->GetGamertag(),
			//m_socket->networkPlayer->GetGamertag());

			hostPlayer->SendData(socketPlayer, buffer.pbyData, buffer.dwDataSize, lowPriority, requireAck);

	// 		DWORD queueSize = hostPlayer->GetSendQueueSize( NULL, QNET_GETSENDQUEUESIZE_BYTES  );
	// 		if( queueSize > 24000 )
	// 		{
	// 			//printf("Queue size is: %d, forcing doWork()\n",queueSize);
	// 			g_NetworkManager.DoWork();
	// 		}
		}
		else
		{
			//printf( "Sent %u bytes of data from \"%ls\" to \"%ls\"\n",
			//buffer.dwDataSize,
			//m_socket->networkPlayer->GetGamertag(),
			//hostPlayer->GetGamertag());

			socketPlayer->SendData(hostPlayer, buffer.pbyData, buffer.dwDataSize, lowPriority, requireAck);
		}
	}
}

void Socket::SocketOutputStreamNetwork::close()
{
	m_streamOpen = false;
}

#ifdef _DEDICATED_SERVER
/////////////////////////////////// TCP socket support for dedicated server ////////////////////

// Static member definitions
C4JThread* Socket::s_tcpListenerThread = nullptr;
int Socket::s_tcpPort = 25565;
ULONG_PTR Socket::s_tcpListenSocket = (ULONG_PTR)INVALID_SOCKET;
BYTE Socket::s_nextSmallId = 1; // 0 is reserved for host

// TCP Socket constructor — uses queue-based input (recv thread feeds the queue)
// and framed TCP output (adds [4-byte len] header to every write)
Socket::Socket(ULONG_PTR tcpSocket)
{
	m_hostServerConnection = false;
	m_hostLocal = false;
	m_end = SOCKET_SERVER_END;
	m_tcpSocket = tcpSocket;
	m_isTCP = true;

	for (int i = 0; i < 2; i++)
	{
		InitializeCriticalSection(&m_queueLockNetwork[i]);
		m_inputStream[i] = nullptr;
		m_outputStream[i] = nullptr;
		m_endClosed[i] = false;
	}

	// Queue-based input: TCPRecvThread reads [4-byte len][payload] frames from TCP
	// and pushes payload bytes into the queue for Connection to read
	m_inputStream[SOCKET_SERVER_END] = new SocketInputStreamNetwork(this, SOCKET_SERVER_END);
	// Framed TCP output: adds [4-byte len] header before each write
	m_outputStream[SOCKET_SERVER_END] = new SocketOutputStreamTCP(tcpSocket);
	m_socketClosedEvent = new C4JThread::Event;
	networkPlayerSmallId = 0;
	createdOk = true;

	// Spawn per-client recv thread to read WinsockNetLayer-framed data
	TCPRecvThreadData* threadData = new TCPRecvThreadData;
	threadData->tcpSock = tcpSocket;
	threadData->socket = this;
	C4JThread* recvThread = new C4JThread(TCPRecvThread, threadData, "TCP Recv", 64*1024);
	recvThread->Run();
}

void Socket::SetTCPPort(int port)
{
	s_tcpPort = port;
}

void Socket::StartTCPListener()
{
	if (s_tcpListenerThread != nullptr) return;

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		fprintf(stderr, "[TCP] WSAStartup failed\n");
		return;
	}

	SOCKET listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenSock == INVALID_SOCKET)
	{
		fprintf(stderr, "[TCP] Failed to create listen socket: %d\n", WSAGetLastError());
		return;
	}

	int optval = 1;
	setsockopt(listenSock, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval));

	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons((u_short)s_tcpPort);

	if (::bind(listenSock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		fprintf(stderr, "[TCP] Bind to port %d failed: %d\n", s_tcpPort, WSAGetLastError());
		closesocket(listenSock);
		return;
	}

	if (listen(listenSock, SOMAXCONN) == SOCKET_ERROR)
	{
		fprintf(stderr, "[TCP] Listen failed: %d\n", WSAGetLastError());
		closesocket(listenSock);
		return;
	}

	s_tcpListenSocket = (ULONG_PTR)listenSock;
	fprintf(stderr, "[TCP] Server listening on port %d\n", s_tcpPort);

	s_tcpListenerThread = new C4JThread(TCPListenerThread, nullptr, "TCP Listener", 64*1024);
	s_tcpListenerThread->Run();
}

int Socket::TCPListenerThread(void* param)
{
	SOCKET listenSock = (SOCKET)s_tcpListenSocket;

	while (s_serverConnection != nullptr && listenSock != INVALID_SOCKET)
	{
		fd_set readfds;
		FD_ZERO(&readfds);
		FD_SET(listenSock, &readfds);

		struct timeval timeout;
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;

		int result = select(0, &readfds, nullptr, nullptr, &timeout);
		if (result <= 0) continue;

		SOCKET clientSock = accept(listenSock, nullptr, nullptr);
		if (clientSock == INVALID_SOCKET) continue;

		fprintf(stderr, "[TCP] New client connection accepted\n");

		// Disable Nagle's algorithm for low latency (client also sets TCP_NODELAY)
		int noDelay = 1;
		setsockopt(clientSock, IPPROTO_TCP, TCP_NODELAY, (char*)&noDelay, sizeof(noDelay));

		// Send small ID to client (WinsockNetLayer::JoinGame expects this 1-byte handshake)
		BYTE assignedSmallId = s_nextSmallId++;
		if (s_nextSmallId >= 0xFF) s_nextSmallId = 1; // wrap around, 0xFF is reject sentinel
		BYTE assignBuf[1] = { assignedSmallId };
		int sent = send(clientSock, (const char*)assignBuf, 1, 0);
		if (sent != 1)
		{
			fprintf(stderr, "[TCP] Failed to send small ID to client\n");
			closesocket(clientSock);
			continue;
		}
		fprintf(stderr, "[TCP] Assigned small ID %d to client\n", assignedSmallId);

		Socket* tcpSocket = new Socket((ULONG_PTR)clientSock);
		tcpSocket->networkPlayerSmallId = assignedSmallId;
		if (s_serverConnection != nullptr)
		{
			s_serverConnection->NewIncomingSocket(tcpSocket);
		}
		else
		{
			delete tcpSocket;
			closesocket(clientSock);
		}
	}
	return 0;
}

/////////////////////////////////// TCP Recv Thread ////////////////////
// Reads [4-byte big-endian length][payload] frames from TCP (WinsockNetLayer format)
// and pushes the payload into the Socket's queue for Connection to read.
// This bridges the gap between WinsockNetLayer's framed protocol and the
// Connection/Packet system that expects raw packet bytes from a stream.

static bool RecvExact(SOCKET sock, BYTE* buf, int len)
{
	int total = 0;
	while (total < len)
	{
		int r = recv(sock, (char*)(buf + total), len - total, 0);
		if (r <= 0) return false;
		total += r;
	}
	return true;
}

int Socket::TCPRecvThread(void* param)
{
	TCPRecvThreadData* data = (TCPRecvThreadData*)param;
	SOCKET sock = (SOCKET)data->tcpSock;
	Socket* socket = data->socket;
	delete data;

	fprintf(stderr, "[TCP] Recv thread started for small ID %d\n", socket->getSmallId());

	while (socket->createdOk && !socket->isClosing())
	{
		// Read 4-byte big-endian length header
		BYTE header[4];
		if (!RecvExact(sock, header, 4))
			break;

		int packetSize = (header[0] << 24) | (header[1] << 16) | (header[2] << 8) | header[3];
		if (packetSize <= 0 || packetSize > 4 * 1024 * 1024)
		{
			printf("[TCP] Invalid packet size %d from small ID %d\n", packetSize, socket->getSmallId());
			break;
		}

		// Read payload
		BYTE* payload = new BYTE[packetSize];
		if (!RecvExact(sock, payload, packetSize))
		{
			delete[] payload;
			break;
		}

		fprintf(stderr, "[TCP] Recv %d bytes from small ID %d (first byte=0x%02x)\n", packetSize, socket->getSmallId(), payload[0]);
		// Push payload into socket queue for Connection to read
		// fromHost=false → pushes to SERVER_END queue (matches m_end for this socket)
		socket->pushDataToQueue(payload, packetSize, false);
		delete[] payload;
	}

	fprintf(stderr, "[TCP] Recv thread ending for small ID %d\n", socket->getSmallId());
	socket->close(true);
	return 0;
}

/////////////////////////////////// TCP InputStream (kept for reference but no longer used by default) ////////////////////

int Socket::SocketInputStreamTCP::read()
{
	if (!m_streamOpen) return -1;
	byte b;
	int received = recv((SOCKET)m_tcpSock, (char*)&b, 1, 0);
	if (received <= 0)
	{
		m_streamOpen = false;
		return -1;
	}
	return (int)(unsigned char)b;
}

int Socket::SocketInputStreamTCP::read(byteArray b, unsigned int offset, unsigned int length)
{
	if (!m_streamOpen || length == 0) return -1;
	unsigned int total = 0;
	while (total < length && m_streamOpen)
	{
		int received = recv((SOCKET)m_tcpSock, (char*)(b.data + offset + total), (int)(length - total), 0);
		if (received <= 0)
		{
			m_streamOpen = false;
			return total > 0 ? (int)total : -1;
		}
		total += (unsigned int)received;
	}
	return (int)total;
}

void Socket::SocketInputStreamTCP::close()
{
	m_streamOpen = false;
	// Signal recv() to unblock (shutdown RD=0)
	shutdown((SOCKET)m_tcpSock, 0);
}

/////////////////////////////////// TCP OutputStream (framed) ////////////////////
// Wraps every write with a [4-byte big-endian length] header so the client's
// WinsockNetLayer::ClientRecvThreadProc can read it as [len][payload] frames.

void Socket::SocketOutputStreamTCP::write(unsigned int b)
{
	if (!m_streamOpen) return;
	// Frame a single byte: [00 00 00 01][byte]
	BYTE frame[5] = { 0, 0, 0, 1, (BYTE)b };
	int sent = 0;
	while (sent < 5)
	{
		int result = send((SOCKET)m_tcpSock, (char*)(frame + sent), 5 - sent, 0);
		if (result <= 0) { m_streamOpen = false; return; }
		sent += result;
	}
}

void Socket::SocketOutputStreamTCP::write(byteArray b, unsigned int offset, unsigned int length)
{
	if (!m_streamOpen || length == 0) return;
	fprintf(stderr, "[TCP] Sending %u bytes to client\n", length);

	// Send 4-byte big-endian length header
	BYTE header[4];
	header[0] = (BYTE)((length >> 24) & 0xFF);
	header[1] = (BYTE)((length >> 16) & 0xFF);
	header[2] = (BYTE)((length >> 8) & 0xFF);
	header[3] = (BYTE)(length & 0xFF);

	unsigned int sent = 0;
	while (sent < 4)
	{
		int result = send((SOCKET)m_tcpSock, (char*)(header + sent), 4 - sent, 0);
		if (result <= 0) { m_streamOpen = false; return; }
		sent += (unsigned int)result;
	}

	// Send payload
	sent = 0;
	while (sent < length)
	{
		int result = send((SOCKET)m_tcpSock, (char*)(b.data + offset + sent), (int)(length - sent), 0);
		if (result <= 0) { m_streamOpen = false; return; }
		sent += (unsigned int)result;
	}
}

void Socket::SocketOutputStreamTCP::close()
{
	m_streamOpen = false;
	shutdown((SOCKET)m_tcpSock, 1);
}

#endif // _DEDICATED_SERVER