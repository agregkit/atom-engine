#include "requestsprocessor.h"

#include "requestsworker.h"
#include "utils.h"

CRequestsProcessor::CRequestsProcessor(event_base* eventBaseStruct) :
	eventBaseStruct_(eventBaseStruct)
{

}

CRequestsProcessor::~CRequestsProcessor()
{

}

void CRequestsProcessor::start(int workersCount)
{
	TRequestsWorkerUniquePtr worker = std::make_unique<CRequestsWorker>(eventBaseStruct_);
	if (worker->start()) {
		workers_.push_back(std::move(worker));
	}
}

void CRequestsProcessor::stop()
{
	auto it = workers_.begin();
	while (it != workers_.end()) {
		(*it)->stop();
		it = workers_.erase(it);
	}
}

void CRequestsProcessor::addRequest(evhttp_request* req)
{
	LOG_INFO << "Add new request" << std::endl;
	if (!workers_.empty()) {
		auto it = workers_.begin();
		(*it)->requestProcess(req);
	}
}