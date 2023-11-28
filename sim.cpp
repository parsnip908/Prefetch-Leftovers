#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <cstring>

using namespace std;

int pagesize = 12;
int cachesize = 128;
int numNodes;

int globalClk = 0;

enum State {M, E, S, I };

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
	int pageNum, home, lastAccess, evictCause;
	State state;
	vector<int> currNodes;

	PTE(int num): pageNum(num), state(E), lastAccess(globalClk), evictCause(0)
	{
		home = num % numNodes;
		currNodes = vector<int>(1, home);
	}

	PTE(int num, State state_): pageNum(num), state(state_), lastAccess(globalClk), evictCause(0)
	{
		home = num % numNodes;
		currNodes = vector<int>(1, home);
	}


	// bool operator==(const PTE& PTE2) {return pageNum == PTE2.pageNum;}
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

	PTE* getHomePage(int num, int rw)
	{
		for(vector<PTE>::iterator i = homePages.begin(); i < homePages.end(); i++)
		{
			if(i->pageNum == num)
			{
				return &(*i);
			}
		}
		homePages.push_back(PTE(num));
		return &homePages.back();
	}

	PTE* checkCache(int num, int rw)
	{
		for(vector<PTE>::iterator i = cache.begin(); i < cache.end(); i++)
		{
			if(i->pageNum == num)
			{
				return &(*i);
			}
		}
		return NULL;

	}
};

class Node
{
public:
	int pid;
	Prefetcher prefetcher;
	// Metrics metrics;
	PageTable pt;

	int homeAccessCount, externAccessCount;

	Node(int pid_): pid(pid_), pt(pid_), prefetcher(pid_), homeAccessCount(0), externAccessCount(0)
	{}
	
	int serviceRequest(Request& req)
	{
		int addr = req.addr;
		int pageNum = addr >> pagesize;
		int home = pageNum % numNodes;

		PTE* currPage;

		if(home == pid)
		{
			homeAccessCount++;
			currPage = pt.getHomePage(pageNum, req.rw);
			// TODO: 
			// check permissions
			// update access time
			// update metrics
		}
		else // remote page
		{
			externAccessCount++;
			currPage = pt.checkCache(pageNum, req.rw);
			// TODO: 
			// catch null if missing
			// check permissions
			// update access time
			// update metrics

		}


		prefetcher.update(req);
		return 0;
	}

	PTE& serviceExternRequest(Request& req)
	{}

	void printMetrics(void)
	{
		cout << "Node " << pid << ": " << homeAccessCount << ", " << externAccessCount << endl;
		cout << "        " << pt.homePages.size() << endl;
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

	// int clk = 0;
	Request* currRequest = trace.pop_request();
	// cout << currRequest << endl;
	while(currRequest)
	{
		while(currRequest && currRequest->clk <= globalClk)
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
		globalClk++;
		if(globalClk % 100000 == 0)
			cout << "reached clk " << globalClk << " with req " << trace.iter << endl;

	}

	for(int i = 0; i < numNodes; i++)
		nodes[i].printMetrics();

	return 0;
}