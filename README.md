# Remote-Execution-Client-Server
This program(s) is meant to be proof-of-concept low-level remote computing server environment that takes requests from its accompanying client.
The server is designed to be multi-threaded and able to handle a scalable volume of client requests following a designated protocol
specified in [communication.h](/communication.h). Each request type has a different name and can be used to call different jobs
for the server to complete. The server and client interact in a fashion that can easily be scaled
to handle larger tasks and various computations.

## Usage
Both server and client are designed to run exclusively on Unix systems.
Both client and server can be compiled using  
```make```  
or  
```make server```  
```make client```   
for respective programs

### Server
The server can be started using ```./server``` and will run on the port specified in [communication.h](/communication.h). The server is designed to be stopped using SIGINT which
will allow it to properly close down the port it is using and let all threads complete their communication with clients.

### Client
The client can be started using ```./client``` or ```./client IP```. When started without a specified ip the client will opperate using the localhost IP address. Otherwise the client will attempt
to connect to the specified IP. For demonstration purposes the client is able to request two jobs from the server.  
The first is done using ```J1``` and will request the specified job from the server (In this case it just simulates some arbitrary process).   
The second is done using ```J2 int``` for some specified integer and will recieve back that integer multiplied by two from the server (Again, just to simulate some arbitrary calculation).

