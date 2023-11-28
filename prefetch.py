#!/bin/python3

import sys
import math

if len(sys.argv) != 3:
    print("USAGE: parse.py trace_path num_procs")
    exit(-1)

trace_path = sys.argv[1]
num_procs = int(sys.argv[2])

print(f"Reading file from: {trace_path} num procs: {num_procs}")

page_size = 12 # standard 4KB
# number of frames in memory
# wait time for accurate prefetch
# logical cycles upper limit
nodes = []

class Request:
    # def __init__(self, ln):
    #     tk = ln.split(",")
    #     self.lamport_clk = int(tk[0])
    #     self.id = int(tk[1])
    #     self.rw = tk[2]
    #     self.addr = int(tk[3], 0)
    #     self.size = int(tk[4])
    #     self.type = 0

    def __init__(self, lamport_clk_, id_, rw_, addr_, size_, type_):
        self.lamport_clk = lamport_clk_
        self.id = id_
        self.rw = rw_
        self.addr = addr_
        self.size = size_
        self.type = type_

    def __str__(self):
        return "Request:" + \
                "\nclk:  " + str(self.lamport_clk) + \
                "\nid:   " + str(self.id) + \
                "\nrw:   " + str(self.rw) + \
                "\naddr: " + hex(self.addr) + \
                "\nsize: " + str(self.size) + \
                "\ntype: " + str(self.type)

    def get_all(self):
        return {"clk": self.lamport_clk, 
                "id": self.id,
                "rw": self.rw,
                "addr": self.addr,
                "size": self.size,
                "type": self.type # meaning real access
                }

    def get_lamport_clk(self):
        return self.lamport_clk

    def get_id(self):
        return self.id
    
    def get_rw(self):
        return self.rw
    
    def get_addr(self):
        return self.addr
    
    def get_size(self):
        return self.size

class Trace:
    def __init__(self, path) -> None:
        self.trace = []
        self.count = 0
        file = open(path, "r")
        file_lines = file.read().splitlines()
        print('generating trace')
        for ln in file_lines:
            tk = ln.split(",")
            self.trace.append(Request(int(tk[0]), int(tk[1]), tk[2], int(tk[3], 0), int(tk[4]), 0))
            self.count += 1
        file.close()

    def pop_request(self):
        if(self.count):
            self.count -= 1
            return self.trace.pop(0)
        return None

    def get_count(self):
        return self.count

    def get_request(self, pid, clk):
        pass

class Metrics:
    def __init__(self):
        pass

class Prefetcher:
    def __init__(self):
        pass
    
    def update(self, request): # update records based on the request made
        pass

    def get_prefetch(self):
        return None

class PTE:
    def __init__(self, num):
        self.page_num = num
        self.home_node = num % num_procs
        self.curr_nodes = [home_node]
        self.state = 0
        self.evict_cause = None


class Page_table:
    def __init__(self, pid_):
        self.pid = pid_
        self.home_pages = []
        self.cache = []

        pass

class Node:
    def __init__(self, id_):
        self.id = id_
        self.prefetcher = Prefetcher()
        self.metric = Metrics()
        self.page_table = Page_table(self.id)
        self.request_queue = []    

    def service_request(self, request):
        addr = request.get_addr()
        page_num = addr >> page_size
        home_node = page_num % num_procs
        # Process access
        # See if hit or miss
        # update the prefetcher
        self.prefetcher.update(request)
        return 0

    def add_request(self, req):
        self.request_queue.append(req)


trace = Trace(trace_path)
print(trace.get_count())
print(trace.trace[0])
original_count = trace.count

for i in range(num_procs):
    nodes.append(Node(i))

# core loop
print('core loop')
clk = 0
curr_request = trace.pop_request()
while trace.get_count() > 0: # need upper limit for cycles as a safety
    # deal with real requests for this cycle
    while curr_request.get_lamport_clk() <= clk:
        nodes[curr_request.get_id()].service_request(curr_request)
        # deal with its effects?
        del curr_request
        curr_request = trace.pop_request()
        if(curr_request == None):
            break

    if(curr_request == None):
        break

    # run prefetcher and service any requests
    for i in range(num_procs):
        prefetch_req = nodes[i].prefetcher.get_prefetch()
        if(prefetch_req != None):
            nodes[i].service_request(prefetch_req)

    clk += 1
    if clk % 5000 == 0:
        print("reached clk " + str(clk) + " with count " + str(trace.get_count()))
