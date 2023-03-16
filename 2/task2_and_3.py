"""
2. Complete the code fragment to solve the problem:
    1 create objects: list, class object, numpy array
    2 the host should send messages to three Workers with the 1 - list, 2 - class object and 3 - numpy array.
    3 the workers have to display the obtained objects in the console.
"""

"""
3. Define the time between sending a message from a worker and receiving it at the host. This assumes that multiple workers can be run. 
	The result of the program should be printed to the console in milliseconds.
"""


from mpi4py import MPI as mpi
from sys import exit
from time import perf_counter as getTime
#from time import time as getTime

# Import 'array' object from the numpy library
from numpy import array

# Create a class to pass its data between processes
class Human:

    # Class members are empty by default and set in the constructor
    firstName = ""
    secondName = ""
    age = 0

    # Class constructor accepts a person's first name, last name, and age
    def __init__(self, firstName: str, secondName: str, age: int):
        self.firstName = firstName
        self.secondName = secondName
        self.age = age

    # When displaying an object of this class, the following will be shown
    def __str__(self):
        obj_presentation = self.firstName + " " + self.secondName + ", "
        obj_presentation += str(self.age) + " years old"
        return obj_presentation

# Get the communicator
comm = mpi.COMM_WORLD
# Get the identifier of the process that launched this program
rank = comm.Get_rank()
n_rank = comm.Get_size()

if (n_rank != 4):
    if (rank == 0):
        print("Number of processes must be 4!")
        print("Run program on several processes: mpiexec -n <processes number> python <program name>")
    exit()

'''1 create objects: list, class object, numpy array'''
# Create a list of countries
object1 = ["Russia", "USA", "Denmark", "Latvia"]
# Create an object of the previously created class
object2 = Human("Ivanov", "Ivan", 50)
# Create a numpy array
object3 = array([1, 2, 6, 10, -5], int)

# Place all created objects (list, object, array) in one list
list_of_objects = [object1, object2, object3]

# The first process will send data
if rank == 0:
    '''2 the host should send messages to three Workers with the 1 - list, 2 - class object and 3 - numpy array.'''
    # Send three messages from the first process to each of the other three processes (1,2,3) 
    # Send messages in a loop (three messages - three objects - three iterations - three processes)

    for i in range(0, len(list_of_objects)):
        # Set the object to be transmitted
        obj = list_of_objects[i]
        # Set the process number
        n_process = rank + 1 + i
        # The tag for each message can be the same, as each process receives only one message
        tag = 0
        # Measure and send
        comm.send([obj, getTime()], n_process, tag)
if rank > 0:
    # All other processes (except the first one) will receive messages

    # Sender's number
    n_sender = 0
    # Process receives a message (object, transfer time)
    message = comm.recv(source=n_sender, tag=0)
    # Record the message reception time
    recvTime = getTime()

    """3 the workers have to display the obtained objects in the console."""
    """3. Define the time between sending a message from a worker and receiving it at the host."""
    # Display the received information from the first sender process and the time spent on message transfer
    print(f"Transfer time of message for {rank} process is {recvTime-message[1]:0.4f} sec. Message: {message[0]}")
