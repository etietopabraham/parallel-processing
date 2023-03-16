'''
9. Write a program which implements master-worker mode:
    1 host must create N worker processes by itself
    2 when worker is created, he writes his rank in the console 
    3 each worker sends his rank to host
    4 host shows the messages received in the console
    For this program you need to write two scripts - host.py and worker.py, each will implement its own logic.
'''

import logging as log
import logging
from mpi4py import MPI as mpi

logging.basicConfig(level=logging.INFO)


comm = mpi.Comm.Get_parent()
rank = comm.Get_rank()
host = 0

# 2 when worker is created, he writes his rank in the console 
# log.error("Host created {} worker!".format(rank))
log.info("Host created {} worker!".format(rank))

#print("Host created {} worker!".format(rank))

# 3 each worker sends his rank to host
comm.send(rank, dest=host)

comm.Disconnect()