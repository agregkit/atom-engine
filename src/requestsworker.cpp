#include "requestsworker.h"

#include "utils.h"
#include "request.h"
#include "requestfactory.h"

#include <hiredis/hiredis.h>
#include <hiredis/adapters/libevent.h>

CRequestsWorker::CRequestsWorker(event_base* eventBaseStruct) :
	eventBaseStruct_(eventBaseStruct)
{

}

CRequestsWorker::~CRequestsWorker()
{

}

bool CRequestsWorker::start()
{
	if (!init()) {
		return false;
	}

	return true;
}

void CRequestsWorker::stop()
{

}

bool CRequestsWorker::init()
{
	redisContext_ = redisAsyncConnect("127.0.0.1", 6379);
	if (redisContext_->err) {
		LOG_ERROR << "Connect to redis failed error = " << redisContext_->errstr << std::endl;
		return false;
	}

	redisLibeventAttach(redisContext_, eventBaseStruct_);

	return true;
}

void CRequestsWorker::requestProcess(evhttp_request* req)
{
	TRequestUniquePtr request = CRequestFactory::createRequest(redisContext_, req, std::bind(&CRequestsWorker::onRequestProcessComplete, this, std::placeholders::_1));
	if (request) {
		LOG_INFO << "Request process type = " << request->getType() << std::endl;
		TProcessingRequests::iterator curRequestIt;
		{
			std::lock_guard<std::mutex> lock(processingRequestsMutex_);
			processingRequests_.push_back(std::move(request));
			curRequestIt = processingRequests_.end() - 1;
		}
		(*curRequestIt)->process();
	}
}

void CRequestsWorker::onRequestProcessComplete(CRequest* request)
{
	std::lock_guard<std::mutex> lock(processingRequestsMutex_);
	for (auto it = processingRequests_.begin(); it != processingRequests_.end(); ++it) {
		if ((*it).get() == request) {
			processingRequests_.erase(it);
			break;
		}
	}
}