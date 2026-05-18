#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include <unistd.h>
#include "walib.h"

#ifdef _WIN32
#define WA_CLOSE(fd) closesocket(fd)
#else
#define WA_CLOSE(fd) close(fd)
#endif

int initsock(void){
  int err = 0;
#ifdef _WIN32
  WORD wVersionRequested;
  WSADATA wsaData;
  wVersionRequested = MAKEWORD( 2, 2 );
  err = WSAStartup( wVersionRequested, &wsaData );
#endif
  return err;
}

void finisock(void){
#ifdef _WIN32
  WSACleanup();
#endif
}

int opentcp(char* ip, unsigned short port){
	int clientSocket;
	struct sockaddr_in serverAddr;
	if((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("create socket fail");
		return -1;
	}

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = inet_addr(ip);
	if(connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
	{
		perror("connect fail");
        WA_CLOSE(clientSocket);
		return -1;
	}

  return clientSocket;
}

struct stTcpOption {
	int rcvtimeo;
};

static struct stTcpOption sv;

void wa_settcpopt(int rcvtimeo){
	if (rcvtimeo>0) {
		sv.rcvtimeo = rcvtimeo;
	}
}

static int httpreq(int fd, char* sendbuf, char* recvbuf, int len){
  int iDataNum;
  int truncated = 0;
  int curLen = 0;
  char tmpBuf[4096];

  if (send(fd, sendbuf, strlen(sendbuf), 0)<=0){
    return -1;
  }
  recvbuf[0] = 0;
  while(1){
    fd_set set;
    struct timeval tout;
    tout.tv_sec = sv.rcvtimeo ?sv.rcvtimeo :1;
    tout.tv_usec = 0;
	FD_ZERO(&set);
	FD_SET(fd, &set);
	int sr = select(fd+1, &set, NULL, NULL, &tout);
	if ( 0<sr ) {
	  if ( FD_ISSET(fd, &set) ) {
		iDataNum = recv(fd, tmpBuf, sizeof(tmpBuf)-1, 0);
		//tmpBuf[iDataNum] = 0;
		if ( iDataNum <= 0) {
		  break;
		}
		if (curLen+iDataNum < len) {
		  memcpy(recvbuf+curLen, tmpBuf, iDataNum);
          curLen += iDataNum;
		} else {
          int copylen = len - curLen -1;
          if (copylen > 0) {
		    memcpy(recvbuf+curLen, tmpBuf, copylen);
            curLen += copylen;
          }
          truncated = 1;
		  break;
		}
	  } else {
		break;
	  }
	} else {//sr<=0 is error, either timeout or other error
		return sr-1;
	}
  }
  recvbuf[curLen] = 0;
  return truncated ?-2 :curLen;
}

int http10(char* ip, int port, char* mthurl, char* header, char* body,
  char* recvbuf, int len){
  int fd = opentcp(ip, (unsigned short)port);
  int ret = -1;
  int bodylen = body?strlen(body):0;
  if (fd != -1) {
    int sndlen = strlen(mthurl)+ strlen(ip)+ (header?strlen(header):0) + bodylen;
    char* sendbuf = malloc(sndlen+128);
	/*Some server response but not close sock, so use read timeout
	struct timeval tout;
	tout.tv_sec = 2;
	tout.tv_usec = 0;
	setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (void*)&tout, sizeof(tout) );*/
    sprintf(sendbuf, "%s HTTP/1.0\r\nHost: %s:%d\r\nContent-Length: %d\r\nConnection: Close\r\n%s\r\n%s",
      mthurl, ip, port, bodylen, header?header:"", body?body:"");
    ret = httpreq(fd, sendbuf, recvbuf, len);
    free(sendbuf);
    shutdown(fd, 2);/*windows close not terminate TCP state, use shutdown */
    WA_CLOSE(fd);
  }
  return ret;
}
