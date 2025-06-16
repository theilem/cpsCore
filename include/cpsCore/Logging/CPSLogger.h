/*
 * CPSLogger.h
 *
 *  Created on: Jun 23, 2017
 *      Author: mircot
 */

#ifndef UAVAP_CORE_LOGGING_CPSLOGGER_H_
#define UAVAP_CORE_LOGGING_CPSLOGGER_H_

#include <ostream>
#include <string>
#include <memory>

class ITimeProvider;

enum class LogLevel
{
	TRACE = 0, DEBUG = 1, WARN = 2, ERROR = 3, NONE = 4
};

class CPSLogger
{
public:

	struct LogLevelScope
	{
		explicit LogLevelScope(LogLevel setLevel);

		~LogLevelScope();

		LogLevel oldLogLevel_;
	};

	struct FlushScope
	{
		~FlushScope();
	};

	static CPSLogger*
	instance();

	~CPSLogger();

	void
	setLogLevel(LogLevel level);

	LogLevel
	getLogLevel() const;

	void
	setSink(std::ostream& sink);

	void
	setModuleName(const std::string& name);

	std::ostream&
	log(LogLevel level);

	std::ostream&
	log(LogLevel level, const std::string& module);

	void
	flush();

	bool
	isFlushed() const;

	void
	setTimeProvider(std::shared_ptr<ITimeProvider> timeProvider);

private:

	static CPSLogger* instance_;

	LogLevel setLevel_;
	std::ostream sink_;
	std::ostream emptySink_;

	std::string moduleName_;

	bool isFlushed_ = true;

	std::weak_ptr<ITimeProvider> timeProvider_;

	CPSLogger();

	class CGuard
	{
	public:
		~CGuard();
	};
};

class RAIILogStream {
public:
	explicit RAIILogStream(LogLevel level)
		: scope_(), stream_(CPSLogger::instance()->log(level)) {}

	std::ostream& stream() { return stream_; }

private:
	CPSLogger::FlushScope scope_;
	std::ostream& stream_;
};


#define CPSLOG(level) (RAIILogStream(level).stream())
#ifdef NO_LOGGING
#warning "Logging Disabled"
#define CPSLOG_ERROR if (0) CPSLOG(::LogLevel::ERROR)
#define CPSLOG_WARN if (0) CPSLOG(::LogLevel::WARN)
#define CPSLOG_DEBUG if (0) CPSLOG(::LogLevel::DEBUG)
#define CPSLOG_TRACE if (0) CPSLOG(::LogLevel::TRACE)

#define MODULE_LOG_ERROR(module) if (0) MODULE_LOG(::LogLevel::ERROR, module)
#define MODULE_LOG_WARN(module) if (0) MODULE_LOG(::LogLevel::WARN, module)
#define MODULE_LOG_DEBUG(module) if (0) MODULE_LOG(::LogLevel::DEBUG, module)
#define MODULE_LOG_TRACE(module) if (0) MODULE_LOG(::LogLevel::TRACE, module)

#else

#define CPSLOG_ERROR CPSLOG(::LogLevel::ERROR) << " [ERROR] " << "[" << __FILE__ << ":" << __LINE__ <<"] "
#define CPSLOG_WARN CPSLOG(::LogLevel::WARN) << " [WARNING] " << "[" << __FILE__ << ":" << __LINE__ <<"] "

#ifdef NODEBUG
#define CPSLOG_DEBUG if (0) CPSLOG(::LogLevel::DEBUG)
#define CPSLOG_TRACE if (0) CPSLOG(::LogLevel::TRACE)
#else
#define CPSLOG_DEBUG CPSLOG(::LogLevel::DEBUG) << " [DEBUG] " << "[" << __FILE__ << ":" << __LINE__ <<"] "
#define CPSLOG_TRACE CPSLOG(::LogLevel::TRACE) << " [TRACE] " << "[" << __FILE__ << ":" << __LINE__ <<"] "
#endif

#define MODULE_LOG(level, module) (CPSLOGger::instance()->log(level, module))
#define MODULE_LOG_ERROR(module) (MODULE_LOG(::LogLevel::ERROR, module) << " [ERROR] " << "[" << __FILE__ << ":" << __LINE__ <<"] ")
#define MODULE_LOG_WARN(module) (MODULE_LOG(::LogLevel::WARN, module) << " [WARNING] " << "[" << __FILE__ << ":" << __LINE__ <<"] ")

#ifdef NODEBUG
#define MODULE_LOG_DEBUG(module) if (0) MODULE_LOG(::LogLevel::DEBUG, module)
#define MODULE_LOG_TRACE(module) if (0) MODULE_LOG(::LogLevel::TRACE, module)
#else
#define MODULE_LOG_DEBUG(module) (MODULE_LOG(::LogLevel::DEBUG, module) << " [DEBUG] " << "[" << __FILE__ << ":" << __LINE__ <<"] ")
#define MODULE_LOG_TRACE(module) (MODULE_LOG(::LogLevel::TRACE, module) << " [TRACE] " << "[" << __FILE__ << ":" << __LINE__ <<"] ")
#endif

#endif //NO_LOGGING

#endif /* UAVAP_CORE_LOGGING_CPSLOGGER_H_ */
