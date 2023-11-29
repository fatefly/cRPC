#ifndef CRPC_COMMON_LOG_H
#define CRPC_COMMON_LOG_H

#include<string>
#include<memory>
#include<sstream>
#include<stdint.h>
#include<fstream>
#include<stdarg.h>
#include<vector>
#include<map>
#include<list>

namespace cRPC{

class Logger;
class LoggerManager;

/* 日志级别 */
class LogLevel{
public:
    enum Level{
        UNKNOW = 0,
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5
    };
    /* 将日志级别转换为文本输出 */
    static const char* ToString(LogLevel::Level Level);
    /* 将文本转换为日志级别 */
    static LogLevel::Level FromString(const std::string& str);
}；

class LogEvent{
public:
    typedef std::shared_ptr<LogEvent> ptr;

    LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level Level
            , const char* file, int32_t line, uint32_t elapse
            , uint32_t thread_id, uint32_t fiber_id, uint64_t time
            , const std::string& thread_name);

    const char* getFile() const { return m_file;}

    int32_t getLine() const{ return m_line;}

    uint32_t getElapse() const{ return m_elapse;}

    uint32_t getThreadID() const{ return m_threadId;}

    uint32_t getFiberId() const{ return m_fiberId;}

    uint64_t getTime() const{ return m_time;}

    const std::string& getThreadName() const{ return m_threadName;}

    std::string getContent() const{ return m_ss.str();}

    std::shared_ptr<Logger> getLogger() const{ return m_logger;}

    LogLevel::Level getLevel() const{ return m_level;}

    std::stringstream& getSS(){ return m_ss;}

    void format(const char* fmt, ...);

    void format(const char* fmt, va_list al);
private:
    // 文件名
    const char* m_file = nullptr;
    // 行号
    int32_t m_line = 0;
    // 程序启动到现在的毫秒数
    uint32_t m_elapse = 0;
    // 线程ID
    uint32_t m_threadId = 0;
    // 协程ID
    uint32_t m_fiberId = 0;
    // 时间戳
    uint64_t m_time = 0;
    // 线程名称
    std::string m_threadName;
    // 日志内容流
    std::stringstream m_ss;
    // 日志器
    std::shared_ptr<Logger> m_logger;
    // 日志级别
    LogLevel::Level m_level;
}；

class LogEventWrap{
public:
    LogEventWrap(LogEvent::ptr e);

    ~LogEventWrap();

    LogEvent::ptr getEvent() const{ return m_event;}

    std::stringstream& getSS();
private:
    LogEvent::ptr m_event;
};

class LogFormatter{
public:
    typedef std::shared_ptr<LogFormatter> ptr;

    LogFormatter(const std::string& pattern);

    std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
    std::ostream& format(std::ostream& ofs, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
public:
    class FormatItem{
    public:
        typedef std::shared_ptr<FormatItem> ptr;

        virtual ~FormatItem(){}

        virtual void format(std::ostream& ofs, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
    };

    void init();

    bool isError() const{ return m_error;}
private:
    // 日志格式模版
    std::string m_pattern;
    // 日志解析后格式
    std::vector<FormatItem::ptr> m_items;
    // 是否有错误
    bool m_error = false;
}；

class LogAppender{
friend class Logger;
public:
    typedef std::shared_ptr<LogAppender> ptr;
    typedef Spinlock MutexType;

    virtual ~LogAppender(){}

    virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;

    virtual std::string toYamlString() = 0;

    void setFormatter(LogFormatter::ptr val);

    LogFormatter::ptr getFormatter();

    LogLevel::Level getLevel() const{ return m_level};

    void setLevel(LogLevel::Level val){ m_level = val;}
private:
    //日志级别
    LogLevel::Level m_level = LogLevel::DEBUG;
    // 是否有自己的日志格式器
    bool m_hasFormatter = false;
    // mutex
    MutexType m_mutex;
    // 日志格式器
    LogFormatter::ptr m_formatter;
};



} //namespace cRPC

#endif  //CRPC_COMMON_LOG_H