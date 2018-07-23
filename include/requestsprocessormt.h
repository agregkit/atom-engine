#ifndef RequestsProcessorMTH
#define RequestsProcessorMTH

#include "requestsprocessor.h"

#include <queue>
#include <mutex>
#include <vector>
#include <memory>
#include <evhttp.h>

typedef std::queue<evhttp_request*> TRequestsQueue;

class CRequestsProcessorMT : public CRequestsProcessor
{
public:
	CRequestsProcessorMT(event_base* eventBaseStruct);
	virtual ~CRequestsProcessorMT();

	virtual void start(int workersCount) override;
	virtual void addRequest(evhttp_request* req) override;
private:
	TRequestsQueue requests_;
	std::mutex requestsQueueMutex_;
};

#endif