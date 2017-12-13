#pragma once

#include "prt/API.h"
#include "prt/LogHandler.h"

#include <string>
#include <vector>
#include <iterator>
#include <ostream>
#include <sstream>
#include <iostream>
#include <memory>


namespace p4h_log {

struct Logger { };

const std::string LEVELS[] = { "trace", "debug", "info", "warning", "error", "fatal" };
const std::wstring WLEVELS[] = { L"trace", L"debug", L"info", L"warning", L"error", L"fatal" };

// log to std streams
template<prt::LogLevel L> struct StreamLogger : Logger {
	StreamLogger(std::wostream& out = std::wcout) : Logger(), mOut(out) { mOut << prefix(); }
	virtual ~StreamLogger() { mOut << std::endl; }
	StreamLogger<L>& operator<<(std::wostream&(*x)(std::wostream&)) { mOut << x; return *this; }
	StreamLogger<L>& operator<<(const std::string& x) { std::copy(x.begin(), x.end(), std::ostream_iterator<char, wchar_t>(mOut)); return *this; }
	template<typename T> StreamLogger<L>& operator<<(const T& x) { mOut << x; return *this; }
	static std::wstring prefix() { return L"[" + WLEVELS[L] + L"] "; }
	std::wostream& mOut;
};

// log through the prt logger
template<prt::LogLevel L> struct PRTLogger : Logger {
	PRTLogger() : Logger() { }
	virtual ~PRTLogger() { prt::log(wstr.str().c_str(), L); }
	PRTLogger<L>& operator<<(std::wostream&(*x)(std::wostream&)) { wstr << x;  return *this; }
	PRTLogger<L>& operator<<(const std::string& x) {
		std::copy(x.begin(), x.end(), std::ostream_iterator<char, wchar_t>(wstr));
		return *this;
	}
	template<typename T> PRTLogger<L>& operator<<(const std::vector<T>& v) {
		wstr << L"[ ";
		for (const T& x: v) {
			wstr << x << L" ";
		}
		wstr << L"]";
		return *this;
	}
	template<typename T> PRTLogger<L>& operator<<(const T& x) { wstr << x; return *this; }
	std::wostringstream wstr;
};

class LogHandler : public prt::LogHandler {
public:
	LogHandler(const std::wstring& name) : mName(name) {
	}

	virtual void handleLogEvent(const wchar_t* msg, prt::LogLevel level) override {
		// probably not the best idea - is there a houdini logging framework?
		std::wcout << L"[" << mName << L"] " << msg << std::endl;
	}

	virtual const prt::LogLevel* getLevels(size_t* count) override {
		*count = prt::LogHandler::ALL_COUNT;
		return prt::LogHandler::ALL;
	}

	virtual void getFormat(bool* dateTime, bool* level) override {
		*dateTime = true;
		*level = true;
	}

	void setName(const std::wstring& n) {
		mName = n;
	}

private:
	std::wstring mName;
};

using LogHandlerPtr = std::unique_ptr<LogHandler>;

} // namespace log


// switch logger here
#define LT p4h_log::PRTLogger

using _LOG_DBG = LT<prt::LOG_DEBUG>;
using _LOG_INF = LT<prt::LOG_INFO>;
using _LOG_WRN = LT<prt::LOG_WARNING>;
using _LOG_ERR = LT<prt::LOG_ERROR>;

// convenience shortcuts in global namespace
#define LOG_DBG _LOG_DBG()
#define LOG_INF _LOG_INF()
#define LOG_WRN _LOG_WRN()
#define LOG_ERR _LOG_ERR()