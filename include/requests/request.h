#ifndef RequestH
#define RequestH

#include <memory>
#include <string>
#include <hiredis/hiredis.h>
#include <hiredis/adapters/libevent.h>
#include <document.h>
#include <functional>
#include <evhttp.h>

class CRequest;

typedef std::unique_ptr<rapidjson::Document> TDocumentUniquePtr;
typedef std::function<void(CRequest*)> OnRequestProcessingCompleteFunc;

class CRequest {
public:
	CRequest(redisAsyncContext* redisContext, evhttp_request* req, TDocumentUniquePtr document, OnRequestProcessingCompleteFunc onComplete);
	virtual ~CRequest();
	
	virtual void process() = 0;
	virtual std::string getType() = 0;
protected:
	void sendBadJSONError();
	void sendSuccess();
	void sendReply(const std::string& str);
protected:
	redisAsyncContext* redisContext_;
	TDocumentUniquePtr document_;
	OnRequestProcessingCompleteFunc onComplete_;
	evhttp_request* req_;
};

#endif