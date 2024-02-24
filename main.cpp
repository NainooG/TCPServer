#include <iostream>
#include <sys/socket.h>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>

using namespace std;

int main() 
{
    // Create a socket
    int listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == -1) 
    {
        cerr << "Cannot create a socket" << endl;
        return -1;
    }

    // Bind the socket to a IP / port
    // htons() host to network short
    // anytime we interact with port number, flip bits using htons()
    // also ntohs does reverse
    sockaddr_in hint;
    hint.sin_family = AF_INET; 
    hint.sin_port = htons(54000);
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr); 

    int binding = bind(listening, (sockaddr*)&hint, sizeof(hint));
    if (binding == -1) 
    {
        cerr << "Cannot bind to IP/port" << endl;
        return -2;
    }

    // Mark the socket for listening in 
    if (listen(listening, SOMAXCONN) == -1) 
    {
        cerr << "Cannot listen!";
        return -3;
    }

    // Accept a call 
    sockaddr_in client;
    socklen_t clientSize = sizeof(client);
    char host[NI_MAXHOST]; // buffers
    char service[NI_MAXSERV];

    int clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
    if (clientSocket == -1) 
    {
        cerr << "Problem with client connecting!";
        return -4;
    }

    // Close the listening socket
    close(listening);

    memset(host, 0, NI_MAXHOST); // cleans up buffers
    memset(service, 0, NI_MAXSERV);

    int result = getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0);

    if (result)
    {
        cout << host << " connected on " << service << endl;
    }
    else 
    {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        cout << host << " connected on " << ntohs(client.sin_port) << endl;
    }
    
    // While receiving - display message, echo message
    char buf[4096];
    while (true)
    {
        // clear buffer
        memset(buf, 0, 4096);
        // wait for a message, receive from client socket
        int bytesReceived = recv(clientSocket, buf, 4096, 0);
        if (bytesReceived == -1) 
        {
            cerr << "There was a connection issue" << endl;
            break;
        }

        if (bytesReceived == 0)
        {
            cout << "The client disconnected" << endl;
            break;
        }

        // display message
        cout << "Received: " << string(buf, 0, bytesReceived) << endl;

        // resend message
        send(clientSocket, buf, bytesReceived + 1, 0);
        
    }

    // Close the socket
    close(clientSocket);

    return 0;
}
