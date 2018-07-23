#include "requestsprocessormt.h"

#include "requestsworker.h"
#include "requestsworkermt.h"
#include "utils.h"

CRequestsProcessorMT::CRequestsProcessorMT(event_base* eventBaseStruct) :
	CRequestsProcessor(eventBaseStruct)
{

}

CRequestsProcessorMT::~CRequestsProcessorMT()
{

}

void CRequestsProcessorMT::start(int workersCount)
{
	for (int i = 0; i < workersCount; ++i) {
		TRequestsWorkerUniquePtr worker = std::make_unique<CRequestsWorkerMT>(requests_, requestsQueueMutex_, eventBaseStruct_);
		if (worker->start()) {
			workers_.push_back(std::move(worker));
		}
	}
}

void CRequestsProcessorMT::addRequest(evhttp_request* req)
{
	LOG_INFO << "Add new request" << std::endl;
	std::lock_guard<std::mutex> lock(requestsQueueMutex_);
	requests_.push(req);
}