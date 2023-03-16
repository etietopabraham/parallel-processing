"""
5. A program to calculate the dot product in distributed mode: two vectors of size 100,000 must be initialized on the host with values 1 (first vector) 
    and 2 (second vector). Each worker must receive their fragment of both vectors, perform element-by-element multiplication, and sum the result. 
    The host must then collect the results from all vectors and calculate the final result by displaying it on the console.
"""

from mpi4py import MPI as mpi
import numpy
import logging as log
from sys import exit

def workForHost():
    comm = mpi.COMM_WORLD
    size = comm.Get_size()
    rank = comm.Get_rank()

    # Size of the generated vectors
    arraySize = 100000

    # Generate two vectors of size 100,000
    v1 = numpy.ones(arraySize, dtype=int)
    
    v2 = v1.copy()
    for i in range(len(v2)):
        v2[i] += 1

    # Each worker will receive an equal number of elements from both vectors
    equalPart = arraySize // (size-1)
    
    # The last worker will have to take the remaining elements (if any)
    remainder = arraySize % (size-1)

    elementsCount = equalPart

    requests1 = []
    requests2 = []

    # Send a fragment of each vector to each worker
    for i in range(1, size):
        end = elementsCount * i

        # If the process gets the last fragment of the vector, it will take the remainder
        if (i == size-1):
            end += remainder

        # For each worker, we will take a new starting point in the vector and measure the required number of elements
        start = (i - 1) * elementsCount

        # Send each worker the corresponding fragment of each vector
        req = comm.isend(v1[start:end], dest=i, tag=1)
        requests1.append(req)
        req = comm.isend(v2[start:end], dest=i, tag=2)
        requests2.append(req)

    # Wait for data transfer to each worker
    for i in range(0, size-1):
        requests1[i].wait()
        requests2[i].wait()

    # Inform the console that the host has sent tasks to all workers - to calculate the sum of their elements
    log.error(f"Host gave tasks to all workers.")

    total = 0
    for i in range(1, size):
        total += comm.recv()

    log.error("Host sum = {}".format(total))

def work():
    comm = mpi.COMM_WORLD
    size = comm.Get_size()
    rank = comm.Get_rank()

    # Each process receives its fragment of the vector
    array1 = comm.recv(source=0, tag=1)
    array2 = comm.recv(source=0, tag=2)

    # Calculate the sum
    sum = 0
    for i in range(0, len(array1)):
        sum += (array1[i] * array2[i])

    # Show which process calculated which sum
    log.error(f"{rank} rank calculated sum = {sum}")

    comm.send(sum, dest=0)

def main():
    N = mpi.COMM_WORLD.Get_size()
    if (N < 2):
        if (mpi.COMM_WORLD.rank == 0):
            print("Must be 1 host and 1 worker at least!")
        return

    rank = mpi.COMM_WORLD.Get_rank()

    if (rank == 0):
        workForHost()
    else:
        work()


if __name__ == "__main__":
    main()