#ifndef RequestsWorkerH
#define RequestsWorkerH

#include <evhttp.h>
#include <memory>
#include <vector>
#include <mutex>

#include "settings.h"

class redisAsyncContext;
class CRequest;

typedef std::unique_ptr<CRequest> TRequestUniquePtr;
typedef std::vector<TRequestUniquePtr> TProcessingRequests;

class CRequestsWorker {
public:
	CRequestsWorker(event_base* eventBaseStruct, CSettings& settings);
	virtual ~CRequestsWorker();
	virtual bool start();
	virtual void stop();
	void requestProcess(evhttp_request* req);
private:
	void onRequestProcessComplete(CRequest* request);
protected:
	bool init();
private:
	redisAsyncContext* redisContext_;
	event_base* eventBaseStruct_;
	TProcessingRequests processingRequests_;
	std::mutex processingRequestsMutex_;
	CSettings& settings_;
};

#endif