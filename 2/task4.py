"""
4. Using the sleep function, write a program in which a worker sends a message to the host and continues to run (outputs some message to the console) 
   while the host is idle and gets the message only when sleep is over, then outputs it to the console. 
"""

"""
P.S.: For some reason, using the print() function blocks the console, and all print() calls within the loop are displayed at once when the process finishes.
For each process, logging is used instead of the print() function.
"""

from mpi4py import MPI as mpi
from time import sleep
import logging as log

# Communicator
comm = mpi.COMM_WORLD
# Process
rank = comm.Get_rank()

if (comm.Get_size() < 2):
    if (rank == 0):
        print("Number of processes must be 4!")
        print("Run program on several processes: mpiexec -n <processes number> python <program name>")
    exit()

# The first process sends messages at its own pace (waits 0.1 seconds after each send) and doesn't wait for anyone else
if rank == 0:
    # That kind of message you should send to calculate
    # time on the worker
    message = "Hello, world "
    for i in range(0, 10):
        # This program will work only with a single worker
        comm.send(message + f"{i}", dest=1, tag=i)
        #print(f"Sender: {i+1} message was sent")
        log.error(f"Sender: {i} message was sent")
        sleep(0.1) # 0.1 second
    print("Sender finished!")

# The second process also works at its own pace: receives a message, displays it, waits for some time (half a second), and starts again
if rank == 1:    
    for i in range(0, 10):
        # Receive the message
        message = comm.recv(source=0, tag=i)
        # Perform additional calculations to determine 
        # the time to transfer the message
        #print(f"Receiver: {i} message was received: {message}")
        log.error(f"Receiver: {i} message was received: {message}")
        # Waiting (performing some work)
        sleep(0.5) # 0.5 second
    print("Receiver finished!")
