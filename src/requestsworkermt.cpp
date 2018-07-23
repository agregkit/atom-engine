#include "requestsworkermt.h"

#include "utils.h"

#include <chrono>

CRequestsWorkerMT::CRequestsWorkerMT(TRequestsQueue& requests, std::mutex& requestsQueueMutex, event_base* eventBaseStruct) :
	CRequestsWorker(eventBaseStruct),
	started_(false),
	requests_(requests),
	requestsQueueMutex_(requestsQueueMutex)
{

}

CRequestsWorkerMT::~CRequestsWorkerMT()
{

}

bool CRequestsWorkerMT::start()
{
	if (!init()) {
		return false;
	}

	started_ = true;
	workerThread_ = std::make_unique<std::thread>(&CRequestsWorkerMT::work, this);
	
	return true;
}

void CRequestsWorkerMT::stop()
{
	started_ = false;
}

void CRequestsWorkerMT::work(CRequestsWorkerMT* requestsWorker)
{
	while (requestsWorker->started_) {
		evhttp_request* req = nullptr;
		{
			std::lock_guard<std::mutex> lock(requestsWorker->requestsQueueMutex_);
			if (!requestsWorker->requests_.empty()) {
				req = requestsWorker->requests_.front();
				requestsWorker->requests_.pop();
			}
		}
		if (req) {
			requestsWorker->requestProcess(req);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}