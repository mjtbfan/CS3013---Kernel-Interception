Nicholas Delli Carpini - Project 2

--- Part 1 ---
Part 1 works as expected, where when the module is installed, all open/close/read instructions done by the user are 
printed out to the syslog. 

--- Part 2 ---
Part 2 works as expected on the grading virtual machine; however, it did not work on my kernel. If you are looking through
the syslog, I would recommend searching for "Targeted", since all of my printk start with that word. procAncestry takes a
single PID for an argument and prints out all the info from the syslog to the terminal. procAncestrytest was just a quick program
written to test that the info printed to the commandline was accurate. This code was then repurposed to build procAncestry.

--- Miscellaneous Notes ---
All of the module parts use a slightly edited makefile from project 0
All of the executables use a very basic makefile
Much of the testing for procAncestry was done using pre-existing processes and comparing the results to the command ps -axf

--- procAncestrytest Output ---

PID1 2458
PID2 2459
PID3 2460
Targeted Task PID: 2459

Children

Siblings
Targeted Sibling 1 PID: 2458
Targeted Sibling 2 PID: 2460

Ancestors
Targeted Parent 1 PID: 2457
Targeted Parent 2 PID: 2055
Targeted Parent 3 PID: 2048
Targeted Parent 4 PID: 1100
Targeted Parent 5 PID: 1084
Targeted Parent 6 PID: 876
Targeted Parent 7 PID: 1