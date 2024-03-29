**<p align = center>Concurrent Directory Copying in Unix: Efficient Server-Client Communication with Threaded Processing**


This project pertains to the domain of system programming within a Unix environment, specifically
with thread creation and network communication. It’s a program that recursively copies all contents
of a directory from a server to the local file system of a client. The server handles requests from
clients concurrently and process each request in parallel, breaking it into independent file
copy operations. <br><br>
Entities and Communication: The main entities in the design are the server and the client. Multiple
clients can connect simultaneously to a server, and communication between them is achieved using
sockets. <br><br>
Server: The server, named dataServer, waits for connections from clients on a predefined port
provided as an argument during startup. The server creates two types of threads: communication
threads and worker threads. The communication thread handles client connections and reads the
directory name to copy recursively. The server maintains a common execution queue for all threads,
where each file in the hierarchy is placed. The server has a thread pool with worker threads, and the
pool size is specified as an argument during execution. Worker threads are responsible for reading
file contents and sending them to the client in blocks, with block size specified as a command-line
argument. The server ensures that each client's socket is written to by only one worker thread at a
time. <br><br>
Client: The client, named remoteClient, connects to the server on a specified port and provides the
directory name to copy. For each file in the directory, the client receives the file name, file metadata,
and file contents from the server. The client creates the necessary directories and subdirectories
locally. If a file already exists in the local file system, it is deleted and recreated from scratch.  <br> <br>
**Conventions:** <br>
• The client is assumed to know the hierarchy of the server's file system and can choose any 
directory for copying. <br>
• The server's file system does not contain empty directories. <br> <br>

**Command Line Arguments:** <br> The server is invoked with the following command-line format:

    ./dataServer -p <port> -s <thread_pool_size> -q <queue_size> -b <block_size>



Where: 
<br>
	- port: The port on which the server listens for external connections. <br>
	- thread pool size: The number of worker threads in the thread pool. <br>
	- queue size: The number of positions in the execution queue. <br>
	- block size: The size of file blocks in bytes sent by worker threads. <br>

    
<br>

The client is invoked with the following command-line format:

    ./remoteClient -i <server_ip> -p <server_port> -d <directory>

Where: <br> 
        - server ip: The IP address used by the server. <br>
        - server port: The port on which the server listens for external connections. <br>
        - directory: The directory to copy (a relative path). <br> <br>
Both command-line arguments are mandatory for both executable programs. Argument pairs can be
provided in a different order than specified in this description.
Example Execution: In this paragraph, we describe an illustrative example of execution. The server
has the following hierarchy: <br> <br>
![Screenshot from 2024-02-05 19-59-15](https://github.com/JohnNDaras/SYSTEM-PROGRAMMING/assets/117290033/89373135-4021-41bc-99d1-23f25c594151)

<br>

If we execute a client request to copy the entire hierarchy, we observe that different worker threads
read and send file data to the client:  <br> <br> <br>
<em>thread_pool_size: 2  <br>
queue_size: 2  <br>
Block_size: 512  <br>
Server was successfully initialized...  <br>
Listening for connections to port 12500  <br>
Accepted connection from localhost  <br>
[Thread: 140069174429440]: About to scan directory Server  <br>
[Thread: 140069174429440]: Adding file Server/file1 to the queue…  <br>
[Thread: 140069174429440]: Adding file Server/Folder1/file5 to the queue...  <br>
[Thread: 140069174429440]: Adding file Server/Folder1/file4 to the queue...  <br>
[Thread: 140069193361152]: Received task: <Server/file1, 4>  <br>
[Thread: 140069193361152]: About to read file Server/file1  <br>
[Thread: 140069184968448]: Received task: <Server/Folder1/file5, 4>  <br>
[Thread: 140069184968448]: About to read file Server/Folder1/file5  <br>
[Thread: 140069174429440]: Adding file Server/file2 to the queue...  <br>
[Thread: 140069174429440]: Adding file Server/file3 to the queue...  <br>
[Thread: 140069193361152]: Received task: <Server/Folder1/file4, 4>  <br>
[Thread: 140069193361152]: About to read file Server/Folder1/file4  <br>
[Thread: 140069193361152]: Received task: <Server/file2, 4>  <br>
[Thread: 140069193361152]: About to read file Server/file2  <br>
[Thread: 140069193361152]: Received task: <Server/file3, 4>  <br>
[Thread: 140069193361152]: About to read file Server/file3  <br>
Client’s parameters are:  <br>
serverIP: 127.0.0.1  <br>
port: 12500  <br>
directory: Server  <br>
Connecting to 127.0.0.1 on port 12500  <br>
Received: Server/file1  <br>
Received: Server/Folder1/file4  <br>
Received: Server/file2  <br>
Received: Server/file3  <br>
Received: Server/Folder1/file5  <br>
</em>

