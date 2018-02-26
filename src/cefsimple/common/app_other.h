#ifndef CEF_TESTS_CEF_SIMPLE_APP_OTHER_H
#define CEF_TESTS_CEF_SIMPLE_APP_OTHER_H
#pragma once

#include "include/cef_app.h"
#include <Windows.h>

class AppOther : public CefApp {
public:
  AppOther(const std::wstring &str) {
    //::MessageBox(NULL, str.c_str(), L"AppOther", MB_YESNOCANCEL);
  }
  virtual ~AppOther() {}
private:
  IMPLEMENT_REFCOUNTING(AppOther);
  DISALLOW_COPY_AND_ASSIGN(AppOther);
};

#endif // !CEF_TESTS_CEF_SIMPLE_APP_OTHER_H