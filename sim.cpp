#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <cstring>

using namespace std;

int pagesize = 12;
int numNodes;

class Request
{
public:
	int clk, pid, rw, addr, size, type;
	
	Request():
		clk(-1), pid(-1), rw(-1), 
		addr(-1), size(-1), type(-1) {}

	Request(int clk_, int id_, int rw_, 
			int addr_, int size_, int type_):
		clk(clk_), pid(id_), rw(rw_), addr(addr_), 
		size(size_), type(type_) {}
	
	Request(char* ln)
	{
		char* tk = strtok(ln, ",");
		clk = atoi(tk);
		tk = strtok(NULL, ",");
		pid = atoi(tk);
		tk = strtok(NULL, ",");
		if(strcmp(tk, "r"))
			rw = 1;
		else rw = 0;
		tk = strtok(NULL, ",");
		addr = stol(string(tk), NULL, 0);
		tk = strtok(NULL, ",");
		size = atoi(tk);
		type = 0;
	}

	void print(void)
	{
		cout << "Request:" << endl <<
				"clk:  " << clk << endl <<
				"pid:  " << pid << endl <<
				"rw:   " << rw << endl <<
				"addr: " << addr << endl <<
				"size: " << size << endl <<
				"type: " << type << endl;
	}

	bool operator<(const Request& r2)
	{
		if(clk == r2.clk)
			return pid < r2.pid;
		return clk < r2.clk;
	}
};

class Trace
{
public:
	vector<Request> trace;
	int count;
	int iter;

	Trace(const char* fileName)
	{
		count = 0;
		iter = 0;
		ifstream infile(fileName);
		cout << "generating trace\n";
		char line[64];
		trace = vector<Request>();
		while(infile.getline(line, 64))
		{
			trace.push_back(Request(line));
			count++;
		}
		cout << "sorting\n";
		sort(trace.begin(), trace.end());
	}

	Request* pop_request()
	{
		if(iter < count)
			return &(trace[iter++]);
		return NULL;
	}
};

class Prefetcher
{
public:
	int pid;
	Prefetcher(int pid_): pid(pid_) {}

	void update(Request req) {}

	Request* getRequest(Request& req)
	{
		return NULL;
	}
};

class PTE
{
public:
	int pageNum, home, state, evictCause;
	vector<int> currNodes;

	PTE(int num): pageNum(num), state(0), evictCause(0)
	{
		home = num % numNodes;
		currNodes = vector<int>(1, home);
	}
};

class PageTable
{
public:
	int pid;
	vector<PTE> homePages;
	vector<PTE> cache;

	PageTable(int pid_): pid(pid_)
	{
		homePages = vector<PTE>();
		cache = vector<PTE>();
	}	
};

class Node
{
public:
	int pid;
	Prefetcher prefetcher;
	// Metrics metrics;
	PageTable pt;

	Node(int pid_): pid(pid_), pt(pid_), prefetcher(pid_)
	{}
	
	int serviceRequest(Request& req)
	{
		int addr = req.addr;
		int pageNum = addr >> pagesize;
		int home = pageNum % numNodes;
		return 0;
	}
};

int main(int argc, char const *argv[])
{
	if(argc != 3)
		cout << "USAGE: sim.exe trace_path num_procs";
	
	numNodes = atoi(argv[2]);

	Trace trace(argv[1]);
	cout << trace.count << endl;
	trace.trace[0].print();

	vector<Node> nodes;

	for(int i = 0; i < numNodes; i++)
		nodes.push_back(Node(i));

	cout << "core loop" << endl;

	int clk = 0;
	Request* currRequest = trace.pop_request();
	cout << currRequest;
	while(currRequest)
	{
		while(currRequest && currRequest->clk <= clk)
		{
			nodes[currRequest->pid].serviceRequest(*currRequest);
			//deal with effects?
			currRequest = trace.pop_request();
		}

		if(!currRequest) break;

		for(int i = 0; i<numNodes; i++)
		{
			Request prefetchReq;
			if(nodes[i].prefetcher.getRequest(prefetchReq))
				nodes[i].serviceRequest(prefetchReq);
		}
		clk++;
		if(clk % 100000 == 0)
			cout << "reached clk " << clk << " with req " << trace.iter << endl;

	}

	return 0;
}