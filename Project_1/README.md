**<p align = center>Process Management and Communication in Unix Environment** 


This project guides you through the process of creating processes using fork/exec system calls,
establishing communication between processes with pipes and named pipes, performing low-level
I/O operations, handling signals, and crafting shell scripts.
The fundamental entities in this task are the listener, the manager, and the workers.
Listener: The inotifywait command sends notifications about changes to the contents of a file
system directory. By using inotifywait, you will monitor changes in the files of a directory.
inotifywait will be executed (via the exec family) within a listener process.
Manager: It is the central entity of the system that communicates with the listener process
through a pipe. The listener informs the manager of each new file it detects in the monitored
directory. Therefore, before running inotifywait, you should pipe the output of the process.
Workers: The manager communicates with the workers through named pipes. For each filename
received from the listener, the manager will notify or create (if non-existent) a worker process to act
on that specific file. At startup, the manager creates as many workers as there are files it knows
exist. The manager maintains information about available workers in a queue and only creates new
workers if none are available.
Mode:
If a worker is stopped, the manager restarts it (using a SIGCONT signal, as available workers will
be in a "stopped" state). The manager also sends information to the worker about which file to
process.
The purpose of the worker is to open the file and search for URLs via low-level I/O. The files are
text files containing plain text and URLs. The search is limited to URLs using the HTTP protocol,
i.e., of the form http://.... Each URL starts with http:// and ends with a blank character.
For each detected URL, it is required to extract information about its location, excluding the www.
Refer to the following link: URL Definition for different parts of a URL. We are specifically
interested in finding its location without the www. For example, for the department website URL
http://www.di.uoa.gr/, the location is "di.uoa.gr."
While reading the file, the worker creates a new file in which it records all detected locations along
with their occurrence number. For instance, if 3 URLs with the location "di.uoa.gr" appear in the
added file, the worker's output file will contain the line "di.uoa.gr 3," and similarly, a line for every
other location.
If the file read by the worker is named <filename>, the file it creates is named <filename>.out.
The worker then notifies the manager that it has finished its work and is available for the next
command. The worker sends itself a SIGSTOP signal to enter the "stopped" state. The manager,
being the parent of the worker process, receives a SIGCHLD signal and determines which child
changed state using waitpid().
Processes do not terminate by themselves; they must be stopped by the user. Termination of the
manager is done with Control+C (SIGINT signal), and before terminating, it must kill all other
processes (listener and workers).
The structure of the process hierarchy is summarized in Figure 1.






![Screenshot from 2024-02-05 18-48-24~2](https://github.com/JohnNDaras/SYSTEM-PROGRAMMING/assets/117290033/dab11f88-c352-4795-b50c-d9cd78a374f2)

<b>
<p align = center>Figure 1
