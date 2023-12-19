#include<map>
#include"log.h"

namespace cRPC{

const char* LogLevel::ToString(LogLevel::Level Level){
    switch(level){
#define XX(name) \
    case LogLevel::name:  \
        return #name;  \
        break;

    XX(DEBUG);
    XX(INFO);
    XX(WARN);
    XX(ERROR);
    XX(FATAL);
#undef XX
    default:
        return "UNKNOW";
    }
    return "UNKNOW";
}

LogLevel::Level LogLevel::FromString(const std::string& str){
#define XX(level, v) \
    if(str == #v){   \
        return LogLevel::level;  \
    }
    XX(DEBUG, debug);
    XX(INFO, info);
    XX(WARN, warn);
    XX(ERROR, error);
    XX(FATAL, fatal);

    XX(DEBUG, DEBUG);
    XX(INFO, INFO);
    XX(WARN, WARN);
    XX(ERROR, ERROR);
    XX(FATAL, FATAL);
    return LogLevel::UNKNOW;
#undef XX
}

LogEventWrap::LogEventWrap(LogEvent::ptr e):m_event(e){
}

LogEventWrap::~LogEventWrap(){
    m_event->getLogger->log(m_event->getLevel, m_event);
}

std::stringstream& LogEventWrap::getSS(){
    return m_event.getSS();
}

LogEvent::LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level Level
            , const char* file, int32_t line, uint32_t elapse
            , uint32_t thread_id, uint32_t fiber_id, uint64_t time
            , const std::string& thread_name)
            :m_file(file)
            ,m_line(line)
            ,m_elapse(elapse)
            ,m_threadId(thread_id)
            ,m_fiberId(fiber_id)
            ,m_time(time)
            ,m_threadName(thread_name)
            ,m_logger(logger),
            ,m_level(level){
}

void LogEvent::format(const char* fmt, ...){
    va_list val;
    va_start(val, fmt);
    format(fmt, val);
    va_end(val);
}

void LogEvent::format(const char* fmt, va_list al){
    char* buf = nullptr;
    int len = vasprintf(&buf, fmt, al);
    if(len != -1){
        m_ss<<std::string(buf, len);
        free(buf);
    }
}

Logger::Logger(const std::string& name):m_name(name), m_level(LogLevel::DEBUG){
    m_formatter.reset(new LogFormatter("%d{%Y-%m-%d %H:%M%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
}

void Logger::setFormatter(LogFormatter::ptr val){
    MutexType::Lock lock(m_mutex);
    m_formatter = val;
    for(auto& appender : m_appenders){
        MutexType::Lock l1(appender->m_mutex);
        if(!appender->m_hasFormatter){
            appender->m_formatter = m_formatter;
        }
    }
}

void Logger::setFormatter(const std::string& val){
    std::cout<<"---"<<val<<std::endl;
    cRPC::LogFormatter::ptr new_val(new cRPC::LogFormatter(val));
    if(new_val->isError()){
        std::cout<<"Logger setFormatter name="<<m_name
                 <<"value="<<val<<"invalid formatter"
                 <<std::endl;
        return;
    }
    setFormatter(new_val);
}

void Logger::log(LogLevel::Level level, LogEvent::ptr event){
    if(level >= m_level){
        auto self = shared_from_this();
        MutexType::Lock lock(m_mutex);
        if(!m_appenders.empty()){
            for(auto& appender : m_appenders){
                appender->log(self, level, event);
            }
        }else if(m_root){
            m_root->log(level, event);
        }
    }
}

void Logger::debug(LogEvent::ptr event){
    log(LogLevel::DEBUG, event);
}

void Logger::info(LogEvent::ptr event){
    log(LogLevel::INFO, event);
}

void Logger::warn(LogEvent::ptr event){
    log(LogLevel::WARN, event);
}

void Logger::error(LogEvent::ptr event){
    log(LogLevel::ERROR, event);
}

void Logger::fatal(LogEvent::ptr event){
    log(LogLevel::FATAL, event);
}

void Logger::addAppender(LogAppender::ptr appender){
    MutexType::Lock lock(m_mutex);
    if(!appender->getFormatter()){
        MutexType::Lock l1(appender->m_mutex);
        appender->m_formatter = m_formatter;
    }
    m_appenders.push_back(appender);
}

void Logger::delAppender(LogAppender::ptr appender){
    MutexType::Lock lock(m_mutex);
    for(auto it = m_appenders.begin(); it != m_appenders.end(); ++it){
        if(*it == appender){
            m_appenders.erase(it);
            break;
        }
    }
}

void Logger::clearAppenders(){
    MutexType::Lock lock(m_mutex);
    m_appenders.clear();
}

LogFormatter::ptr Logger::getFormatter(){
    MutexType::Lock lock(m_mutex);
    return m_formatter;
}

std::string Logger::toYamlString(){

}
}//namespace cRPC



