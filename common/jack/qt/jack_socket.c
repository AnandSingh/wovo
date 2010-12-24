#include <winsock2.h>
#include <ws2tcpip.h>
#include "osal_socket.h"
#include "osal_string.h"
#include "osal_stdio.h"
#include "osal_memory.h"



/*---------------------------------------------------------------------------*/

unsigned long Evinet_addr (const char * cp);
char * Evinet_ntoa (unsigned long in);

# ifndef ___constant_swab16
#  define ___constant_swab16(x) \
		((unsigned short)( \
		(((unsigned short)(x) & (unsigned short)0x00ffU) << 8) | \
		(((unsigned short)(x) & (unsigned short)0xff00U) >> 8) ))
# endif

	
# ifndef ___constant_swab32
#  define ___constant_swab32(x) \
		((unsigned long)( \
		(((unsigned long)(x) & (unsigned long)0x000000ffUL) << 24) | \
		(((unsigned long)(x) & (unsigned long)0x0000ff00UL) <<  8) | \
		(((unsigned long)(x) & (unsigned long)0x00ff0000UL) >>  8) | \
		(((unsigned long)(x) & (unsigned long)0xff000000UL) >> 24) ))
# endif

void EvsockaddrToNative(Evsockaddr *pEvSockAddrIn, void* pOut);
void NativeToEvsockaddr(void* pIn, Evsockaddr *pEvSockAddrOut);

/*---------------------------------------------------------------------------*/

int EvSocketInit(void)
{
	int err = 0;
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD( 2, 2 );

	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 )
	{
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		Evperror("EvSocketInit: Unable to initialize Winsock DLL (step 1)\n");
		return -1;
	}
	/* Confirm that the WinSock DLL supports 2.2.*/
	/* Note that if the DLL supports versions greater    */
	/* than 2.2 in addition to 2.2, it will still return */
	/* 2.2 in wVersion since that is the version we      */
	/* requested.                                        */
	if( LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2 )
	{
		/* Tell the user that we could not find a usable */
		/* WinSock DLL. */
		Evperror("EvSocketInit: Unable to initialize Winsock DLL (step 2)");
        WSACleanup( );
		return -1;
	}
	return 0;
}

/*---------------------------------------------------------------------------*/

/* deinitializes socket library */
int EvSocketUninit(void)
{
	return WSACleanup( );
}

/*---------------------------------------------------------------------------*/

EvSOCKET Evsocket(int af, int type, int protocol)
{
	switch(af)
	{
	case EV_AF_INET:
		af = AF_INET;
		break;
	
	case EV_AF_INET6:
		af = AF_INET6;
		break;
	
	default:
		Evprintf("Unknown address family %d\n", af);
		return -1;
	}

	switch(type)
	{
	case EV_SOCK_DGRAM:
		type = SOCK_DGRAM;
		break;
    case EV_SOCK_STREAM:
        type = SOCK_STREAM;
        break;
	default:
		Evprintf("Unknown socket type %d\n", type);
		return -1;
	}

	switch(protocol)
	{
	case EV_IPPROTO_IP:
		protocol = IPPROTO_IP;
		break;

	case EV_IPPROTO_IPV6:
		protocol = IPPROTO_IPV6;
		break;

	case EV_IPPROTO_UDP:
		protocol = IPPROTO_UDP;
		break;
	
	default:
		Evprintf("Unknown protocol %d\n", protocol);
		return -1;
	}

	return (EvSOCKET)socket(af, type, protocol);
}

/*---------------------------------------------------------------------------*/

int Evclosesocket ( EvSOCKET s)
{
	return closesocket(s);
}

/*---------------------------------------------------------------------------*/

int Evgethostbyname (Evsockaddr_in *addr, int addrlen, const char * name )
{
  struct hostent * he;
  unsigned long int ip;

  Evmemset(addr, 0, addrlen);

  ip = Evinet_addr(name);
  if(ip == INADDR_NONE)
  {
	  he = gethostbyname(name);
	  if(!he)
		return -1;
	  Evmemcpy(&(addr->sin_addr), he->h_addr, sizeof(addr->sin_addr));
  }
  else
  {
	  addr->sin_addr = ip;
  }

  addr->sin_family = EV_AF_INET;
  
  return 0;
}

/*---------------------------------------------------------------------------*/

int Evrecv (EvSOCKET s, void * buf, int len)
{
    return recv(s, buf, len, 0);
}

/*---------------------------------------------------------------------------*/

int Evsend (EvSOCKET s, const void * buf, int len)
{
    return send(s, buf, len, 0);
}

/*---------------------------------------------------------------------------*/

int Evconnect (EvSOCKET s, Evsockaddr *addr, int addrlen)
{
	if(addr->sa_family == EV_AF_INET)
	{
		struct sockaddr BSDSockAddr;	
		EvsockaddrToNative(addr, &BSDSockAddr);	
		return connect(s, &BSDSockAddr, addrlen);
	}
	else if(addr->sa_family == EV_AF_INET6)
	{
		struct sockaddr_in6 BSDSockAddr;	
		EvsockaddrToNative(addr, &BSDSockAddr);	
		return connect(s, (struct sockaddr*)&BSDSockAddr, addrlen);
	}
    else
    {
      return -1;
    }
}

/*---------------------------------------------------------------------------*/

int Evbind (EvSOCKET s, Evsockaddr *addr, int namelen)
{
	if(addr->sa_family == EV_AF_INET)
	{
		struct sockaddr BSDSockAddr;	
		EvsockaddrToNative(addr, &BSDSockAddr);	
		return bind(s, &BSDSockAddr, namelen);
	}
	else if(addr->sa_family == EV_AF_INET6)
	{
		struct sockaddr_in6 BSDSockAddr;	
		EvsockaddrToNative(addr, &BSDSockAddr);	
		return bind(s, (struct sockaddr*)&BSDSockAddr, namelen);
	}
    else
    {
      return -1;
    }
}

/*---------------------------------------------------------------------------*/

int Evlisten (EvSOCKET s, int backlog)
{
	return listen (s, backlog);
}

/*---------------------------------------------------------------------------*/

EvSOCKET Evaccept(EvSOCKET s, struct Evsockaddr* addr, int *addrlen)
{
	return accept (s, addr, addrlen);
}

/*---------------------------------------------------------------------------*/

int Evsetsockopt (EvSOCKET s, int level, int optname, const char  * optval, int optlen)
{
	/* map level, optname to native constants */
	switch(level)
	{
	case EV_SOL_SOCKET:
		level = SOL_SOCKET;
		switch(optname)
		{
		case EV_SO_REUSEPORT:
			return 0;
		case EV_SO_REUSEADDR:
			optname = SO_REUSEADDR;
			break;		
		default:
			return -1;
		}
		break;
	
	case EV_IPPROTO_IP:
		level = IPPROTO_IP;		
		switch(optname)
		{
		case EV_IP_ADD_MEMBERSHIP:
			optname = IP_ADD_MEMBERSHIP;
			break;		
		case EV_IP_DROP_MEMBERSHIP:
			optname = IP_DROP_MEMBERSHIP;
			break;
        case EV_IP_MULTICAST_IF:
			optname = IP_MULTICAST_IF;
			break;
		case EV_IP_MULTICAST_TTL:
			optname = IP_MULTICAST_TTL;
			break;
		case EV_IP_MULTICAST_LOOP:
			optname = IP_MULTICAST_LOOP;
			break;
		default:
			return -1;
		}
		break;

	case EV_IPPROTO_IPV6:
		level = IPPROTO_IPV6;		
		switch(optname)
		{
		case EV_IPV6_ADD_MEMBERSHIP:
			optname = IPV6_ADD_MEMBERSHIP;
			break;		
		case EV_IPV6_DROP_MEMBERSHIP:
			optname = IPV6_DROP_MEMBERSHIP;
			break;
        case EV_IPV6_MULTICAST_IF:
			optname = IPV6_MULTICAST_IF;
			break;
		case EV_IPV6_MULTICAST_HOPS:
			optname = IPV6_MULTICAST_HOPS;
			break;
		case EV_IPV6_MULTICAST_LOOP:
			optname = IPV6_MULTICAST_LOOP;
			break;
		default:
			return -1;
		}
		break;

	default:
		return -1;
	}

	return setsockopt(s, level, optname, optval, optlen);
}

/*---------------------------------------------------------------------------*/

int Evgetsockopt (EvSOCKET s, int level, int optname, const char * optval, int *optlen)
{
	return getsockopt (s, level, optname, optval, optlen);
}

/*---------------------------------------------------------------------------*/

int Evrecvfrom (EvSOCKET s, char * buf, int len, int flags,
                struct Evsockaddr *from, int *fromlen)
{
	return recvfrom(s, buf, len, flags, (struct sockaddr *)from, fromlen);
}

/*---------------------------------------------------------------------------*/

int EvSetSocketNonBlocking(EvSOCKET s, unsigned long set_non_blocking)

{
	if (ioctlsocket(s, FIONBIO, (&set_non_blocking)) == SOCKET_ERROR ) 
	{
		Evperror("ERROR, fcntl failed");
		return -1;
	}
	return 0;
}

/*---------------------------------------------------------------------------*/

void EvsockaddrToNative(Evsockaddr *pEvSockAddrIn, void* pOut)
{
	if(pEvSockAddrIn->sa_family == EV_AF_INET)
	{
		Evmemcpy(pOut, pEvSockAddrIn, sizeof(struct sockaddr));
		((struct sockaddr_in*)pOut)->sin_family = AF_INET;
	}
	else if(pEvSockAddrIn->sa_family == EV_AF_INET6)
	{
		Evmemcpy(pOut, pEvSockAddrIn, sizeof(struct sockaddr_in6));
		((struct sockaddr_in6*)pOut)->sin6_family = AF_INET6;
	}
}

/*---------------------------------------------------------------------------*/

void NativeToEvsockaddr(void* pIn, Evsockaddr *pEvSockAddrOut)
{
	if(((struct sockaddr_in*)pIn)->sin_family == AF_INET)
	{
		Evmemcpy(pEvSockAddrOut, pIn, sizeof(Evsockaddr));	
		pEvSockAddrOut->sa_family = EV_AF_INET;
	}
	else if(((struct sockaddr_in*)pIn)->sin_family == AF_INET6)
	{
		Evmemcpy(pEvSockAddrOut, pIn, sizeof(Evsockaddr_in6));	
		((struct Evsockaddr_in6*)pEvSockAddrOut)->sin6_family = EV_AF_INET6;
	}
}

/*---------------------------------------------------------------------------*/

int Evinet_pton(int af, const char *src, void *dst)
{
	if(!src || !dst || !(af == EV_AF_INET || af == EV_AF_INET6))
		return 0;

	if(af == EV_AF_INET)
	{
		*(unsigned long*)dst = Evinet_addr(src);

		if(*(unsigned long*)dst == EV_INADDR_NONE)
			return 0;
		
		return 1;
	}
	else if(af == EV_AF_INET6)
	{
		struct sockaddr_in6	sa6 = {0};
		int len = sizeof(sa6);
		
		sa6.sin6_family = AF_INET6;

	/* TLE : The following code does not compile for WM5 : not useful according to msdn */
	/*#ifdef _WIN32_WCE
		USES_CONVERSION;
		if (WSAStringToAddress(A2W(src), AF_INET6, NULL,
					(struct sockaddr*) &sa6, &len) != 0) 
		{
			return 0;
		}
	#else*/

		if (WSAStringToAddress(src, AF_INET6, NULL,
					(struct sockaddr*) &sa6, &len) != 0) 
		{
			return 0;
		}
	/* #endif */

		Evmemcpy(dst, &sa6.sin6_addr, sizeof(sa6.sin6_addr));
		return 1;
	}

	return 0;
}

/*---------------------------------------------------------------------------*/

char *Evinet_ntop(int af, const void *src, char *dst, unsigned long size)
{
	if(!src || !dst || size<=1 || !(af == EV_AF_INET || af == EV_AF_INET6))
		return 0;

	if(af == EV_AF_INET)
	{
		char *res;
		
		res = Evinet_ntoa(*(unsigned long*)src);
		if(!res)
			return 0;

		if((size - 1) < Evstrlen(res))
			return 0;

		Evstrcpy(dst, res);
		
		return dst;
	}
	else if(af == EV_AF_INET6)
	{
		/* TLE */
		const unsigned short *pwsrc = src;

		if (size < 40) return 0;

		Evsprintf(dst,"%X:%X:%X:%X:%X:%X:%X:%X",
		        ___constant_swab16(pwsrc[0]),
		        ___constant_swab16(pwsrc[1]),
		        ___constant_swab16(pwsrc[2]),
		        ___constant_swab16(pwsrc[3]),
		        ___constant_swab16(pwsrc[4]),
		        ___constant_swab16(pwsrc[5]),
		        ___constant_swab16(pwsrc[6]),
		        ___constant_swab16(pwsrc[7]));

		return dst;
	}

	return 0;
}

/*---------------------------------------------------------------------------*/

unsigned long Evinet_addr (const char * cp)
{
	return inet_addr(cp);
}

/*---------------------------------------------------------------------------*/

char * Evinet_ntoa (unsigned long addr)
{
	return inet_ntoa(*((struct in_addr*)&addr));
}

/*---------------------------------------------------------------------------*/

unsigned long Evhtonl(unsigned long in)
{
	return htonl(in);
}

/*---------------------------------------------------------------------------*/

unsigned short Evhtons(unsigned short in)
{
	return htons(in);
}

/*---------------------------------------------------------------------------*/

unsigned long Evntohl(unsigned long in)
{
	return ntohl(in);
}

/*---------------------------------------------------------------------------*/

unsigned short Evntohs(unsigned short in)
{
	return ntohs(in);
}
