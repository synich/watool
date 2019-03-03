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

int initsock(){
  int err = 0;
#ifdef _WIN32
  WORD wVersionRequested;
  WSADATA wsaData;
  wVersionRequested = MAKEWORD( 2, 2 );
  err = WSAStartup( wVersionRequested, &wsaData );
#endif
  return err;
}

void finisock(){
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
  int iTotalNum = 0;
  char tmpBuf[256];

  send(fd, sendbuf, strlen(sendbuf), 0);
  recvbuf[0] = 0;
  fd_set set;
  struct timeval tout;
  tout.tv_sec = 1;
  tout.tv_usec = 0;
  if (sv.rcvtimeo != 0) {
	tout.tv_sec = sv.rcvtimeo;
  }
  while(1){
	FD_ZERO(&set);
	FD_SET(fd, &set);
	int sr = select(fd+1, &set, NULL, NULL, &tout);
	if ( 0<sr ) {
	  if ( FD_ISSET(fd, &set) ) {
		iDataNum = recv(fd, tmpBuf, sizeof(tmpBuf)-1, 0);
		tmpBuf[iDataNum] = 0;
		iTotalNum += iDataNum;
		if ( iDataNum <= 0) {
		  break;
		}
		if (iTotalNum < len) {
		  strncat(recvbuf, tmpBuf, iDataNum);
		} else {
		  strncat(recvbuf, tmpBuf, len-strlen(recvbuf)-1);
		  break;
		}
	  } else {
		break;
	  }
	} else {//sr<=0 is error, either timeout or other error
		return sr-1;
	}
  }
  return strlen(recvbuf);
}

int http10(char* ip, int port, char* mthurl, char* header, char* body,
  char* recvbuf, int len){
  int fd = opentcp(ip, (unsigned short)port);
  int ret = -1;
  int bodylen = body?strlen(body):0;
  if (fd != -1) {
    int sndlen = strlen(mthurl)+ (header?strlen(header):0) + bodylen;
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
    close(fd);
  }
  return ret;
}
