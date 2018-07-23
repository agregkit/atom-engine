#ifndef RequestsWorkerMTH
#define RequestsWorkerMTH

#include "requestsworker.h"

#include <queue>
#include <mutex>
#include <thread>
#include <evhttp.h>
#include <hiredis/hiredis.h>

typedef std::queue<evhttp_request*> TRequestsQueue;
typedef std::unique_ptr<std::thread> TThreadUniquePtr;

class CRequestsWorkerMT : public CRequestsWorker
{
public:
	CRequestsWorkerMT(TRequestsQueue& requests, std::mutex& requestsQueueMutex, event_base* eventBaseStruct);
	virtual ~CRequestsWorkerMT();
	virtual bool start() override;
	virtual void stop() override;
private:
	static void work(CRequestsWorkerMT* requestsWorker);
private:
	volatile bool started_;
	TRequestsQueue& requests_;
	std::mutex& requestsQueueMutex_;
	TThreadUniquePtr workerThread_;
};

#endif