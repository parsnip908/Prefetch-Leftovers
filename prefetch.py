#!/bin/python3

import sys

if len(sys.argv) != 3:
    print("USAGE: parse.py trace_path num_procs")
    exit(-1)

trace_path = sys.argv[1]
num_procs = int(sys.argv[2])

print(f"Reading file from: {trace_path} num procs: {num_procs}")

# pagesize - standard 4KB
# wait time for accurate prefetch
# logical cycles upper limit

class Request:
    def __init__(self, ln):
        tk = ln.split(",")
        self.lamport_clk = int(tk[0])
        self.id = int(tk[1])
        self.rw = tk[2]
        self.addr = int(tk[3], 0)
        self.size = int(tk[4])
        self.type = 0

    def __init__(self, lamport_clk_, id_, rw_, addr_, size_, type_):
        self.lamport_clk = lamport_clk_
        self.id = id_
        self.rw = rw_
        self.addr = addr_
        self.size = size_
        self.type = type_

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
        file = open(path, "r")
        file_lines = file.read().splitlines()
        for ln in self.file_lines:
            self.trace.append(Request(ln))
        close(file)

    def pop_request(self):
        pass

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

class Page_table:
    def __init__(self):
        pass

class Proc:
    def __init__(self, id):
        self.id = id
        self.preftcher = Prefetcher()
        self.metric = Metrics()
        self.page_table
        self.request_queue = []    

    def service_request(self, request):
        # Process access
        # See if hit or miss
        # update the prefetcher
        Prefetcher.update(request)
        return 0
    

    def add_request(self, req):
        self.request_queue.append(req)

    def handle_request(self):


trace = Trace(trace_path)

procs = []
for i in range(num_procs):
    procs.append(Proc(i))

# core loop   
curr_request = trace.pop_request()
for clk in range(upper_limit): #need upper limit for cycles as a safety
    # check if request for this cycle
        # send request to processor
        # deal with its effects?
        # load new request and loop
    # run prefetcher