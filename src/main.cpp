#include <vector>
#include "Server.hpp"
#include "ServerController.hpp"
     
#define TRUE   1 
#define FALSE  0 
#define PORT 8888 

static int count;

int main(int argc , char *argv[])  
{  
    std::vector<Port> ports;
    ports.push_back(PORT);
    ports.push_back(PORT + 1);

    ServerController serverController(ports);
    serverController.run();
    return 0;
}  