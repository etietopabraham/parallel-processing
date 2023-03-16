from mpi4py import MPI

# The code is designed to be executed by two processes:
# The first process sends a message "Hello world" using the send() function,
# and the second process receives this message using the recv() function.

# Create a communicator (a group of processes)
comm = MPI.COMM_WORLD

# Since we are using the MPI library (mpi4py) for parallelizing the program,
# this code is executed by multiple processes, as specified when launching the program, for example:
# mpiexec -n 2 python task1.py
# The program will run on two processes.

# Each process runs this code, and the 'rank' object will contain its own value
# - the process identifier. The first process will have rank = 0, the second process - rank = 1, etc.
rank = comm.Get_rank()

# The first process (rank = 0) creates the message and sends it to the second process (rank = 1)
# This is specified in the send() function through the 'dest' (destination) parameter.
if rank == 0:
    # Define the message
    message = "Hello, world!"
    
    # The sender (rank = 0) sends the message with tag 0 (message identifier).
    comm.send(message, dest=1, tag=0)  

# If the program is running on more than one process, the second process (rank = 1)
# receives the message from the first process (rank = 0).
if rank == 1:
    # The second process receives the data using the recv() function, specifying that it receives data
    # from a source with rank = 0 (source=0) and with identifier = 0 (tag=0).
    message = comm.recv(source=0, tag=0)
    print(f"Rank {rank} received the message: {message}")
