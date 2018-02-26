#ifndef CEF_TESTS_CEF_SIMPLE_BROWSER_CIENT_H_
#define CEF_TESTS_CEF_SIMPLE_BROWSER_CIENT_H_
#pragma once

#include "include/cef_client.h"
#include "include/cef_life_span_handler.h"
#include "include/cef_request_handler.h"
#include "include/wrapper/cef_message_router.h"

class BrowserClient : public CefClient,
											public CefLifeSpanHandler,
											public CefRequestHandler {
public:
	BrowserClient();
	virtual ~BrowserClient();
public:
	class RouterHandler : public CefMessageRouterBrowserSide::Handler {
	public:
		// CefMessageRouterBrowserSide::Handler methods:
		virtual bool OnQuery(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			int64 query_id,
			const CefString& request,
			bool persistent,
			CefRefPtr<Callback> callback) OVERRIDE;
	};
public:
	// CefClient methods:
	virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() { return this; }
	virtual CefRefPtr<CefRequestHandler> GetRequestHandler() { return this; }
	virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                        CefProcessId source_process,
		                                    CefRefPtr<CefProcessMessage> message) OVERRIDE;

  // CefLifeSpanHandler methods:
  virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
  virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

  // CefRequestHandler methods:
  virtual bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
                              CefRefPtr<CefFrame> frame,
                              CefRefPtr<CefRequest> request,
                              bool is_redirect) OVERRIDE;
  virtual void OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser,
                                         TerminationStatus status) OVERRIDE;
private:
  RouterHandler router_handler_;
  CefRefPtr<CefMessageRouterBrowserSide> message_router_;
private:
  IMPLEMENT_REFCOUNTING(BrowserClient);
  DISALLOW_COPY_AND_ASSIGN(BrowserClient);
};



#endif // !CEF_TESTS_CEF_SIMPLE_BROWSER_CIENT_H_
