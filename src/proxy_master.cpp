#include "proxy_master.h"

ProxyServer :: ProxyServer(int port, int logOption, char *target, int tartPort, int n){
  if(port > 256 && port <= 65535){
    _ServerPort = port;
  }else{
    _ServerPort = DEFAULT_PORTNO;
  }

  memcpy(_ProxyOptions.targetName, target, strlen(target));
  _ProxyOptions.targetPort = tartPort;
  _ProxyOptions.logOption = logOption;
  _ProxyOptions.autoN = n;
  
  _ServerSocket = -1;
}


/*
  startServer

  initializes the server socket then waits for a connection
*/
void ProxyServer :: startServer(){

  _ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
  _ServerAddress.sin_family = AF_INET;
  _ServerAddress.sin_port = htons(_ServerPort);
  _ServerAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
  memset(_ServerAddress.sin_zero, '\0', sizeof _ServerAddress.sin_zero);

  bind(_ServerSocket, (struct sockaddr *) &_ServerAddress, sizeof(_ServerAddress));

  waitForConnection();
}

/*
  waitForConnection

  waitForConnection will listen for a connection on the server socket, but only if the server socket has been
  initialized. Once the client is connected, a message is sent to the client to confirm that the server is connected
  and the server waits for a command.
 */
void ProxyServer :: waitForConnection(){
  char buffer[1024];
  pthread_t worker;
  struct ProxyOptions *proxyOptionsCopy;

  while(_ServerSocket > -1){
    if(listen(_ServerSocket, 1) == 0){
      printf("!! Client connected\n");
    }

    proxyOptionsCopy = (struct ProxyOptions*)malloc(sizeof(_ProxyOptions));
    
    memcpy(_ProxyOptions.targetName, proxyOptionsCopy->targetName, strlen(_ProxyOptions.targetName));
    proxyOptionsCopy->targetPort = _ProxyOptions.targetPort;
    proxyOptionsCopy->logOption = _ProxyOptions.logOption;
    proxyOptionsCopy->autoN = _ProxyOptions.autoN;

    /* accept call creates a new socket for incomming connection */
    _AddrSize = sizeof _ServerStorage;
    
    proxyOptionsCopy->clientSocket = accept(_ServerSocket, (struct sockaddr *) &_ServerStorage, &_AddrSize);

    pthread_create(&worker, NULL, &spawnWorker, (void*)proxyOptionsCopy);
  }
  printf("!! Init server socket first\n");
}




void ProxyServer :: setPort(int port){
  if(port > 256 && port <= 65535){
    _ServerPort = port;
  }
}

int ProxyServer :: getPort(){
  return _ServerPort;
}
