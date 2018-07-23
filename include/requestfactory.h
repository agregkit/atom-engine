#ifndef RequestFactoryH
#define RequestFactoryH

#include <memory>
#include <evhttp.h>
#include <hiredis/hiredis.h>
#include <hiredis/adapters/libevent.h>
#include <functional>

class CRequest;
typedef std::unique_ptr<CRequest> TRequestUniquePtr;
typedef std::function<void(CRequest*)> OnRequestProcessingCompleteFunc;

class CRequestFactory {
public:
	static TRequestUniquePtr createRequest(redisAsyncContext* redisContext, evhttp_request* req, OnRequestProcessingCompleteFunc onComplete);
};

#endif