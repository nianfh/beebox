
#ifndef __HTTP_CONNECTION_H__
#define __HTTP_CONNECTION_H__

#include "Socket.h"
#include "http_parser/http_parser.h"

#include <string>
#include <map>

using namespace std;

#define CRLF "\r\n"

/* Informational 1xx */
#define HTTP_STATUS_100 "100 Continue"
#define HTTP_STATUS_101 "101 Switching Protocols"
#define HTTP_STATUS_102 "102 Processing"
/* Successful 2xx */
#define HTTP_STATUS_200 "200 OK"
#define HTTP_STATUS_201 "201 Created"
#define HTTP_STATUS_202 "202 Accepted"
#define HTTP_STATUS_203 "203 Non-Authoritative Information"
#define HTTP_STATUS_204 "204 No Content"
#define HTTP_STATUS_205 "205 Reset Content"
#define HTTP_STATUS_206 "206 Partial Content"
#define HTTP_STATUS_207 "207 Multi-Status"
/* Redirection 3xx */
#define HTTP_STATUS_300 "300 Multiple Choices"
#define HTTP_STATUS_301 "301 Moved Permanently"
#define HTTP_STATUS_302 "302 Moved Temporarily"
#define HTTP_STATUS_303 "303 See Other"
#define HTTP_STATUS_304 "304 Not Modified"
#define HTTP_STATUS_305 "305 Use Proxy"
#define HTTP_STATUS_307 "307 Temporary Redirect"
/* Client Error 4xx */
#define HTTP_STATUS_400 "400 Bad Request"
#define HTTP_STATUS_401 "401 Unauthorized"
#define HTTP_STATUS_402 "402 Payment Required"
#define HTTP_STATUS_403 "403 Forbidden"
#define HTTP_STATUS_404 "404 Not Found"
#define HTTP_STATUS_405 "405 Method Not Allowed"
#define HTTP_STATUS_406 "406 Not Acceptable"
#define HTTP_STATUS_407 "407 Proxy Authentication Required"
#define HTTP_STATUS_408 "408 Request Time-out"
#define HTTP_STATUS_409 "409 Conflict"
#define HTTP_STATUS_410 "410 Gone"
#define HTTP_STATUS_411 "411 Length Required"
#define HTTP_STATUS_412 "412 Precondition Failed"
#define HTTP_STATUS_413 "413 Request Entity Too Large"
#define HTTP_STATUS_414 "414 Request-URI Too Large"
#define HTTP_STATUS_415 "415 Unsupported Media Type"
#define HTTP_STATUS_416 "416 Requested Range Not Satisfiable"
#define HTTP_STATUS_417 "417 Expectation Failed"
#define HTTP_STATUS_418 "418 I'm a teapot"
#define HTTP_STATUS_422 "422 Unprocessable Entity"
#define HTTP_STATUS_423 "423 Locked"
#define HTTP_STATUS_424 "424 Failed Dependency"
#define HTTP_STATUS_425 "425 Unordered Collection"
#define HTTP_STATUS_426 "426 Upgrade Required"
#define HTTP_STATUS_428 "428 Precondition Required"
#define HTTP_STATUS_429 "429 Too Many Requests"
#define HTTP_STATUS_431 "431 Request Header Fields Too Large"
/* Server Error 5xx */
#define HTTP_STATUS_500 "500 Internal Server Error"
#define HTTP_STATUS_501 "501 Not Implemented"
#define HTTP_STATUS_502 "502 Bad Gateway"
#define HTTP_STATUS_503 "503 Service Unavailable"
#define HTTP_STATUS_504 "504 Gateway Time-out"
#define HTTP_STATUS_505 "505 HTTP Version Not Supported"
#define HTTP_STATUS_506 "506 Variant Also Negotiates"
#define HTTP_STATUS_507 "507 Insufficient Storage"
#define HTTP_STATUS_509 "509 Bandwidth Limit Exceeded"
#define HTTP_STATUS_510 "510 Not Extended"
#define HTTP_STATUS_511 "511 Network Authentication Required"

namespace beebox
{

class CFile;

class CHttpConnection : public sigslot::has_slots<>
{
public:
	CHttpConnection(CSocket* socket);
	~CHttpConnection();

	void start();
	void stop();

	/************** Request method **************/
	string getRequestMethod();
	string getRequestPath();
	string getRequestQuery();
	string getRequestBody();
	string getRequestHeaderFieldValue(string fieldName);

	/************** Response method **************/
	void setResponseStatus(string status);
	void setResponseHeader(string fieldName, string fieldValue);
	bool isResponseKeepAlive();
	void resetResponseHeader();
	void sendResponseSimple(string body);
	void sendResponseSimple(char* bodyBuffer, int bodySize);
	void sendResponse(char* bodyBuffer, int bodySize, bool withEnd = true);
	void sendResponseDirct(string content);
	void sendResponseDirct(char* buffer, int size);
	bool haveResponded();

	/************** Download method **************/
	bool sendFile(string filePath); // don't support Resume From Breakpoint yet

	/************** Upload method **************/
	void setUploadingCacheFileDir(string dirPath);
	string getUploadingCacheFilePath();

	/************** Static method **************/
	static string getContentType(string fileExtension);

	/************** signal **************/
	sigslot::signal1<CHttpConnection*> doRoute; // emit when http frame is completed
	sigslot::signal1<CHttpConnection*> doDisconnect; // emit when disconnect

	/************** Private method **************/
	// called by static c function
	void onRead(int length);
	void onWrite(bool isOK);
	void onClosed(CSocket* socket);

	// request
	void setRequestPath(string path);
	void addRequestBody(string body);
	void clearRequestBody();
	void setUploading(string fileName);
	void setRequestQuery(string query);
	void setRequestLastHeaderFieldName(string fieldName);
	void setRequestHeaderFieldValue(string fieldValue);

	// upload file
	bool isUploading();
	void enableUploadLogic(string boundary);
	void writeCacheFile(const char* buffer, int length);
	void uploadEnd();

private:
	bool parseRequest(char* buffer, int length);

	CSocket* m_socket;
	char* m_recvBuffer;

	typedef struct http_parser_settings HttpSettings;
	typedef struct http_parser HttpParser;
	HttpSettings m_httpSettings;
	HttpParser m_httpParser;

	/************** Request **************/
	string m_path;
	string m_query;
	string m_body;

	string m_lastHeaderField;
	map<string, string> m_headerMap;

	/************** Response **************/
	bool m_haveResponded;

	string m_status;
	string m_contentType;
	string m_connectionType;
	string m_accessControlAllowOrigin;
	map<string, string> m_resposeHeaderMap;

	/************** Uploading **************/
	bool m_uploading;
	string m_boundary;
	string m_uploadingFilePath;
	string m_uploadingFileDir;
	CFile* m_uploadingFile;
};

}

#endif // __HTTP_SESSTION_H__
