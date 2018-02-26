#ifndef CEF_TESTS_CEF_SIMPLE_APP_BROWSER_H_
#define CEF_TESTS_CEF_SIMPLE_APP_BROWSER_H_
#pragma once

#include "include/cef_app.h"
#include "include/cef_browser_process_handler.h"

class AppBrowser : public CefApp, public CefBrowserProcessHandler
{
public:
	AppBrowser();
	virtual ~AppBrowser();
public:
	// CefApp methods.
	CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE {
		return this;
	}

	// CefBrowserProcessHandler methods.
	void OnContextInitialized() OVERRIDE;
private:
  IMPLEMENT_REFCOUNTING(AppBrowser);
  DISALLOW_COPY_AND_ASSIGN(AppBrowser);
};



#endif // !CEF_TESTS_CEF_SIMPLE_APP_BROWSER_H_
