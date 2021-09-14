# Simulator of Orchateria
This Simulator is a C++ application that simulates the orders at one orchateria.

# How does it work?
The program is given N amount of orchateros (and waiters, for simplicity one waiter for each orchatero), as the first line of the input file (see data.txt example)
followed by each table  and the customers in each line for the day, one by one. 
Each line represent a table, consecuent number in a line represent the waitinglist for each table or succesive customers for that table

All orchateros move at the same speed.
N number of threads fire off and each worker thread represents a orchatero and a waiter working.
To avoid two or more worker utilizing the same table at the same time I utilized Mutex.

Entering any input from the keyboard would cause the system to exit after the current order is completed.

#Usage: 

add to a dir and run:
make

Then run:
./main data.txt
