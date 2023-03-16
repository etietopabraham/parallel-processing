'''
9. Write a program which implements master-worker mode:
    1 host must create N worker processes by itself
    2 when worker is created, he writes his rank in the console 
    3 each worker sends his rank to host
    4 host shows the messages received in the console
    For this program you need to write two scripts - host.py and worker.py, each will implement its own logic.
'''

# program must be ran like this:
# mpiexec -n 1 python host.py worker.py

import logging as log
import logging
import sys
from mpi4py import MPI as mpi

logging.basicConfig(level=logging.INFO)


# checking
if (mpi.COMM_WORLD.Get_size() != 1):
    if (mpi.COMM_WORLD.Get_rank() == 1):
        print("Must be only one Host!")
else:
    # 1 host must create N worker processes by itself
    N = 3
    comm = mpi.COMM_WORLD.Spawn(sys.executable, args=['worker.py'], maxprocs=N)

    for i in range(N):
        # 4 host shows the messages received in the console
        message = comm.recv() # from any and any tag
        # log.error("Rank {}, message: {}".format(comm.Get_rank(), message))
        log.info("Rank {}, message: {}".format(comm.Get_rank(), message))
        #print("Rank {}, message: {}".format(comm.Get_rank(), message))

    comm.Disconnect()