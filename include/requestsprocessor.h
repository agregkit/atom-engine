#ifndef RequestsProcessorH
#define RequestsProcessorH

#include <evhttp.h>
#include <memory>
#include <vector>

class CRequestsWorker;

typedef std::unique_ptr<CRequestsWorker> TRequestsWorkerUniquePtr;
typedef std::vector<TRequestsWorkerUniquePtr> TRequestsWorkers;

class CRequestsProcessor {
public:
	CRequestsProcessor(event_base* eventBaseStruct);
	virtual ~CRequestsProcessor();

	virtual void start(int workersCount);
	void stop();
	virtual void addRequest(evhttp_request* req);
protected:
	TRequestsWorkers workers_;
	event_base* eventBaseStruct_;
};

#endif