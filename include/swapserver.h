#ifndef SwapServerH
#define SwapServerH

#include <memory>
#include <evhttp.h>

#include "settings.h"

class CRequestsProcessor;
class CSettings;

typedef std::unique_ptr<CRequestsProcessor> TRequestsProcessorUniquePtr;

class CSwapServer {
public:
	CSwapServer();
	~CSwapServer();

	bool run();
private:
	static void onRequest(evhttp_request *req, void *arg);
private:
	bool started_;
	TRequestsProcessorUniquePtr requestsProcessor_;
	CSettings settings_;
};

#endif