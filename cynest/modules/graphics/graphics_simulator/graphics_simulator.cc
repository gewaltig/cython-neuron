// c++ test.cc `sdl-config --cflags --libs` -lGLU -lSDL -o test


#include <iostream>
#include <cmath>
#include <vector>
#include <memory.h>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <ctime>
#include <sstream>
#include <string>

#if defined (_WIN32)
    #include <winsock.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <sys/types.h>
#endif

#include <GL/glu.h>
#include <SDL/SDL.h>

using namespace std;




class Socket {
private:
	#if _WIN32
		int sock;
		int csock;
		int recsize;
		int crecsize;
	#else
		socklen_t sock;
		socklen_t csock;
		socklen_t recsize;
		socklen_t crecsize;
	#endif

		int port;
		sockaddr_in ssin;

		sockaddr_in csin;
		int listener;


	void init(int port_, char* host_) {
		#if _WIN32
			WSADATA wsaData;
			WORD version;
			version = MAKEWORD( 2, 0 );
			WSAStartup( version, &wsaData );
			WSACleanup();
			recsize = sizeof(ssin);
			crecsize = sizeof(csin);
		#else
			recsize = sizeof(ssin);
			crecsize = sizeof(csin);
		#endif
		port = port_;
		sock = socket(AF_INET, SOCK_STREAM, 0);
		ssin.sin_addr.s_addr = htonl(INADDR_ANY);
		ssin.sin_family = AF_INET;
		ssin.sin_port = htons(port_);
	}
	
public:
	void acceptConnection(int port_, char* host_) {
		init(port_, host_);
		bind(sock, (sockaddr*)&ssin, recsize);
		listen(sock, 1);
		csock = accept(sock, (sockaddr*)&csin, &crecsize);
		listener = 1;
	}
	
	void initiateConnection(int port_, char* host_) {
		init(port_, host_);
		connect(sock, (sockaddr*)&ssin, recsize);
		listener = 0;
	}


	void sendMsg(char* msg, int length) {
		send(sock, msg, length, 0);
	}
	
	void receiveMsg(char* buffer, int length) {
		recv(csock, buffer, length, 0);
	}

	void destroy(){
		close(sock);
		if(listener = 1){
			close(csock);
		}
	}
};







int main() {
	Socket listener = Socket();
	Socket sender = Socket();
	
	
	sender.initiateConnection(50001, NULL);
	listener.acceptConnection(50002, NULL);
	
	sender.sendMsg("ready", 5);

	listener.destroy();
	sender.destroy();
	

	return 0;
}









