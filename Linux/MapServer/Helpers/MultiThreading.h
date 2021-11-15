#pragma once

#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>

#include "Assert.h"
#include "Helpers.h"

#include <sched.h>
#include <numa.h>

#include <omp.h>

class ThreadScheduler {

public:
	ThreadScheduler(const std::string strategy, const int nt = omp_get_num_procs()) :
		numLogicalCpus(omp_get_max_threads()),
		numThreads(nt),
		numNumaNodes(1),
		threadToLogicalCpu(numThreads, 0),
		threadToNumaNode(numThreads, 0),
		threadToNumaMaster(numThreads, false),
		numaNodeToLogicalCpus(numNumaNodes) {
		Assert(1 != -1);
		initialize(strategy);
	}

	inline int numThreadsUsed() const {return numThreads;}
	inline int getNumNumaNodesUsed() const {return numNumaNodesUsed;}

	inline int getNumaNodeFromThreadId(const int threadId) const {
		Assert(threadId < threadToNumaNode.size());
		return threadToNumaNode[threadId];
	}

	inline int getLogicalCpuFromThreadId(const int threadId) const {
		Assert(threadId < threadToLogicalCpu.size());
		return threadToLogicalCpu[threadId];
	}

	inline bool isNumaNodeMaster(const int threadId) const {
		Assert(threadId < threadToNumaMaster.size());
		return threadToNumaMaster[threadId];
	}

	inline void pinThread(const int threadId) {
		Assert(threadId < threadToLogicalCpu.size());
		cpu_set_t mask;
		CPU_ZERO(&mask);
		CPU_SET(threadToLogicalCpu[threadId], &mask);
		sched_setaffinity(0, sizeof(mask), &mask);
	}

protected:
	void initialize(const std::string strategy, bool verbose = true) {
		if (verbose) std::cout << "NUMA nodes in the system: " << numNumaNodes << std::endl;
		const int width = ceil(log10(double(numLogicalCpus)));
		for (int n = 0; n < numNumaNodes; ++n) {
			std::cout << "Node " << n << ": " << std::flush;
			bitmask *bmp = numa_allocate_cpumask();
			numa_node_to_cpus(n, bmp);
			for (int l = 0; l < numLogicalCpus; ++l) {
				if (numa_bitmask_isbitset(bmp, l)) {
					numaNodeToLogicalCpus[n].push_back(l);
					std::cout << std::setw(width) << std::right << l << " " << std::flush;
				}
			}
			numa_free_cpumask(bmp);
			std::cout << std::endl;
		}
		if (verbose) std::cout << "Distributing threads according to strategy: " << strategy << std::endl;
		if (strategy == "R") distributeRoundRobin();
		if (strategy == "F") distributeFill();
		numNumaNodesUsed = 0;
		for (int threadId = 0; threadId < numThreads; ++threadId) {
			threadToNumaNode[threadId] = numa_node_of_cpu(threadToLogicalCpu[threadId]);
			numNumaNodesUsed = Variadic::max(numNumaNodesUsed, threadToNumaNode[threadId]);
		}
		numNumaNodesUsed++;

		std::vector<bool> numaAssigned(numNumaNodes, false);
		for (int threadId = 0; threadId < numThreads; ++threadId) {
			int currentNumaNode = threadToNumaNode[threadId];
			if (numaAssigned[currentNumaNode]) continue;
			numaAssigned[currentNumaNode] = true;
			threadToNumaMaster[threadId] = true;
		}

		if (verbose) {
			std::cout << "Logical CPUs (NUMA nodes) being used: " << std::flush;
			for (int t = 0; t < numThreads; ++t) {
				std::cout << threadToLogicalCpu[t] << "(" << threadToNumaNode[t] << ")  " << std::flush;
			}
			std::cout << std::endl;
			std::cout << "# NUMA nodes used in this setting: " << numNumaNodesUsed << std::endl;
		}
		if (verbose) std::cout << std::endl;
	}

	inline void distributeRoundRobin() {
		std::vector<int> currentLogicalCpuIndex(numNumaNodes, 0);
		int currentNumaNode(0);
		for (int t = 0; t < numThreads; ++t) {
			// Skip numa nodes that are 'full'
			while(currentLogicalCpuIndex[currentNumaNode] >= numaNodeToLogicalCpus[currentNumaNode].size())
				currentNumaNode = (currentNumaNode + 1) % numNumaNodes;

			threadToLogicalCpu[t] = numaNodeToLogicalCpus[currentNumaNode][currentLogicalCpuIndex[currentNumaNode]++];

			// Go to next numa node
			currentNumaNode = (currentNumaNode + 1) % numNumaNodes; // Next node
		}
	}

	inline void distributeFill() {
		for (int t = 0; t < numThreads; ++t) {
			threadToLogicalCpu[t] = t;
		}
	}

private:
	int numLogicalCpus;
	int numThreads;
	int numNumaNodes;
	std::vector<int> threadToLogicalCpu;
	std::vector<int> threadToNumaNode;
	std::vector<bool> threadToNumaMaster;
	std::vector<std::vector<int> > numaNodeToLogicalCpus;
	int numNumaNodesUsed;

};

inline void pinThreadToCoreId(const int coreId) {
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(coreId, &mask);
	sched_setaffinity(0, sizeof(mask), &mask);
}
