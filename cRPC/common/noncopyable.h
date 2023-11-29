#ifndef CRPC_COMMON_NONCOPYABLE_H
#define CRPC_COMMON_NONCOPYABLE_H

namespace cRPC{

class Noncopyable{
public:
    Noncopyable() = default;

    ~Noncopyable = default;

    Noncopyable(const Noncopyable&) = delete;

    Noncopyable& operator==(const Noncopyable&) = delete;
};

} //namespace cRPC

#endif //CRPC_COMMON_NONCOPYABLE_H