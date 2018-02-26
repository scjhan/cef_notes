#ifndef CEF_TESTS_CEF_SIMPLE_APP_RENDER_H_
#define CEF_TESTS_CEF_SIMPLE_APP_RENDER_H_
#pragma once

#include "include/cef_app.h"
#include "include/cef_render_process_handler.h"
#include "include/wrapper/cef_message_router.h"

class AppRenderer : public CefApp, public CefRenderProcessHandler
{
public:
	AppRenderer();
	virtual ~AppRenderer();
public:
  // CefApp methods:
  virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() 
      OVERRIDE {
    return this;
  }

  // CefRenderProcessHandler methods:
  virtual void OnWebKitInitialized() OVERRIDE;
  virtual void OnContextCreated(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                CefRefPtr<CefV8Context> context) OVERRIDE;

  virtual void OnContextReleased(CefRefPtr<CefBrowser> browser,
                                 CefRefPtr<CefFrame> frame,
                                 CefRefPtr<CefV8Context> context) OVERRIDE;
  virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                        CefProcessId source_process,
                                        CefRefPtr<CefProcessMessage> message) OVERRIDE;
private:
  CefRefPtr<CefMessageRouterRendererSide> message_router_;
private:
  IMPLEMENT_REFCOUNTING(AppRenderer);
  DISALLOW_COPY_AND_ASSIGN(AppRenderer);
};


#endif // !CEF_TESTS_CEF_SIMPLE_APP_RENDERER_H_
