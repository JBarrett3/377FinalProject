# 377FinalProject

This project extends Project 3, the scheduler. It does so by implementing MLFQ and a visualizer for MLFQ, along with a scheduler of my own.

It is best to start with an explanation of the user functionality. In the main_scheduling.cpp file, I've implemented two more cases in the if/else chain. One takes in the string "mlfq" and the other "modMlfq". These correspond to the mlfq scheduler and the scheduler I came up with on my own. Each of these requires extra parameters, so I've included arguments in the command line according. It now has the requirement of 4 additional integers that are formatted as "mlfq workload_file levels timeSlice boost interactive". The original functionality, "[fifo|sjf|stcf|rr] workload_file" is of course still preserved. Considering each new parameter, levels correspond to the number of queues (or levels) in the MLFQ, timeSlice to how long each job should stay in one queue before being demoted to a lower one, boost to how many seconds between jobs should be waited until the jobs are all returned to the first (highest priority) queue, and interactive to whether or not some of these jobs should be considered interactive. An example would then be "mlfq workloads/workload_03.txt 3 5 25 0" which would mean that there are 3 levels, each job stays in one queue for 5 seconds of work before being demoted, and every 25 seconds all the jobs go back to the first queue, and none of the jobs are interactive. Furthermore, due to space limits in the EdLab, the total time is limited to 200 seconds and the levels to 8. Interactivity is binary as well, so we request a re-entry of inputs if the levels are more than 8 or the interactivity is inputted as anything except 0 or 1. With this understanding of how the code works for a user, specifying the workload, levels, timeSlice, boost, and interactivity, now we can consider the code as a developer.

After parsing the user parameters, the mlfq function is called with those parameters. If the job is marked interactive, then every other file is selected to be interactive and the job struct is marked accordingly. Then, when jobs are selected as running, occasionally interactive jobs will pass and not run at all. The rest of the jobs are marked intensive by default and never pass. Then, the queues are constructed to the size of the levels. Before entering any processing, the 2D array of queues called snapShots is initialized. This is an object used to keep track of the status of the queues at any time. This is done by calling updateSnapShots every time, which copies the states of each queue into the snapShot queues at that time. Then, the function begins work on the jobs. As long as the queues are not empty, the function iterates through the queues from 0 to the highest level, checking if there are jobs in each queue. If it finds jobs, it runs that job (or passes if interactive) and pushes that job onto either the back of that queue (if it has been in the queue for less than its timeSlice) or the next lower queue (if it has been in the queue for all of its time timeSlice). From there, the jobs are updated and a snapShot is taken to the snapShots array using updateSnapShots. Finally, after all the jobs are completed (the queues are empty), the snapShots are exported to a text file using exportSnapShots. 

Now, addressing my scheduling program, I've modified the MLFQ scheduler and called it modMlfq. This program is a modification of mlfq that reassesses the initial priority of incoming jobs. In experimenting with the processes, I noticed that intensive jobs were moved to the lower queues while newer jobs that came in while the others were in lower levels led to lower jobs starving. The solution given in class for this was to occasionally boost all the jobs, which is successful, but not until the boost takes effect. I thought to defeat this by varying the priorities and inputting new jobs at the level of as many jobs in the queues. Moreover, I imagined that this would help balance the new and older jobs immediately as they came in. This was implemented similarly to mlfq and users can run it almost identically using: "modMLFQ workload_file levels timeSlice boost interactive". 

I invite you to test this yourself and see how the turnaround time and response time vary using these two new scheduling protocols. And to watch my presentation about the project here : https://youtu.be/ZOtNMf6F-iE.
