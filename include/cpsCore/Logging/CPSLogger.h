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

	static CPSLogger*
	instance();

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

private:

	static CPSLogger* instance_;

	LogLevel setLevel_;
	std::ostream sink_;
	std::ostream emptySink_;

	std::string moduleName_;

	bool isFlushed_ = true;

	CPSLogger();

	class CGuard
	{
	public:
		~CGuard();
	};
};

#define CPSLOG(level) (CPSLogger::instance()->log(level))
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

#define CPSLOG_ERROR (CPSLOG(::LogLevel::ERROR) << __FILE__ << ":" << __LINE__ << " [ERROR] ")
#define CPSLOG_WARN (CPSLOG(::LogLevel::WARN) << __FILE__ << ":" << __LINE__ << " [WARNING] ")

#ifdef NODEBUG
#define CPSLOG_DEBUG if (0) CPSLOG(::LogLevel::DEBUG)
#define CPSLOG_TRACE if (0) CPSLOG(::LogLevel::TRACE)
#else
#define CPSLOG_DEBUG (CPSLOG(::LogLevel::DEBUG) << __FILE__ << ":" << __LINE__ << " [DEBUG] ")
#define CPSLOG_TRACE (CPSLOG(::LogLevel::TRACE) << __FILE__ << ":" << __LINE__ << " [TRACE] ")
#endif

#define MODULE_LOG(level, module) (CPSLOGger::instance()->log(level, module))
#define MODULE_LOG_ERROR(module) (MODULE_LOG(::LogLevel::ERROR, module) << __FILE__ << ":" << __LINE__ << " [ERROR] ")
#define MODULE_LOG_WARN(module) (MODULE_LOG(::LogLevel::WARN, module) << __FILE__ << ":" << __LINE__ << " [WARNING] ")

#ifdef NODEBUG
#define MODULE_LOG_DEBUG(module) if (0) MODULE_LOG(::LogLevel::DEBUG, module)
#define MODULE_LOG_TRACE(module) if (0) MODULE_LOG(::LogLevel::TRACE, module)
#else
#define MODULE_LOG_DEBUG(module) (MODULE_LOG(::LogLevel::DEBUG, module) << __FILE__ << ":" << __LINE__ << " [DEBUG] ")
#define MODULE_LOG_TRACE(module) (MODULE_LOG(::LogLevel::TRACE, module) << __FILE__ << ":" << __LINE__ << " [TRACE] ")
#endif

#endif //NO_LOGGING

#endif /* UAVAP_CORE_LOGGING_CPSLOGGER_H_ */
