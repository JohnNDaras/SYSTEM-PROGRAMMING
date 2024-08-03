**<p align = center>Process Management and Communication in Unix Environment** 


This project guides us through the process of creating processes using system calls,
establishing communication between processes with pipes and named pipes, performing low-level
I/O operations, handling signals, and crafting shell scripts. <br>
The fundamental entities in this task are the listener, the manager, and the workers. <br>
Listener: The inotifywait command sends notifications about changes to the contents of a file
system directory. By using inotifywait, we monitor changes in the files of a directory.
inotifywait is executed within a listener process. <br>
Manager: It is the central entity of the system that communicates with the listener process
through a pipe. The listener informs the manager of each new file it detects in the monitored
directory. Therefore, before running inotifywait, we pipe the output of the process. <br>
Workers: The manager communicates with the workers through named pipes. For each filename
received from the listener, the manager will notify or create (if non-existent) a worker process to act
on that specific file. At startup, the manager creates as many workers as there are files it knows
exist. The manager maintains information about available workers in a queue and only creates new
workers if none are available. <br>
Mode: <br>
If a worker is stopped, the manager restarts it (using a SIGCONT signal, as available workers will
be in a "stopped" state). The manager also sends information to the worker about which file to
process. <br>
The purpose of the worker is to open the file and search for URLs via low-level I/O. The files are
text files containing plain text and URLs. The search is limited to URLs using the HTTP protocol,
i.e., of the form http://.... Each URL starts with http:// .
For each detected URL, it is required to extract information about its location, excluding the www.
Refer to the following link: URL Definition for different parts of a URL. We are specifically
interested in finding its location without the www. For example, for the department website URL
http://www.di.uoa.gr, the location is "di.uoa.gr." (Other possible URLs could be i.e. https://www.youtube.com, http://192.168.1.1/index.html, http://www.122.128.1.1:8080/index.html/, http://www.10.1.0.1:3000/dashboard/, http://www.example.com:8080/home/, http://www.122.422.189.123:9090/path/to/resource/)
While reading the file, the worker creates a new file in which it records all detected locations along
with their occurrence number. For instance, if 3 URLs with the location "di.uoa.gr" appear in the
added file, the worker's output file will contain the line "di.uoa.gr 3," and similarly, a line for every
other location. <br>
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

**<p align = center>Figure 1**
<br>

**Note**: Static strings are utilized in this project with an understanding of memory allocation. However, the primary focus remains on mastering Process Management and Communication in the Unix Environment.
<br>
<br>
Also the Bash script summarize.sh processes files in the current directory to sum numbers associated with strings that end with a user-provided substring. It starts by checking if the user has provided a required argument, which is the substring to match. If the argument is missing, it prompts the user for correct usage and exits. <br>

The script then initializes an associative array to store sums of numbers indexed by strings. It iterates over all files in the directory, and for each file, it reads each line expecting a comma-separated string and number. Leading and trailing whitespace are removed from both the string and the number. It checks if the string ends with the specified substring provided by the user. If it does, the script adds the corresponding number to the sum for that string in the associative array. <br>

After processing all files, the script prints each string that matched the criteria along with its total sum and calculates the overall sum of all matching strings. Finally, it prints the total sum of numbers for strings that end with the provided substring. <br>

The script expects one argument, which is the substring to match. You run the script by providing this argument on the command line: <br>

./summurize.sh <string_to_match>
