#pragma once
#include <mutex>
#include <condition_variable>
#include <functional>
#include <unistd.h>
#include <arpa/inet.h>

template <typename T>
class ProgressObservable
{
public:
    virtual void setData(const T &newData, bool complete) = 0;
};

template <typename T>
class ProgressObserver
{
public:
    virtual T getData() = 0;
    virtual T wait() = 0;
    virtual bool isComplete() = 0;
    virtual void subscribe(std::function<void(const T &)> callback,
                           std::function<void(const T &)> complete) = 0;
};

template <typename T>
class ProgressObservableMutexImpl : public ProgressObservable<T>, public ProgressObserver<T>
{
protected:
    std::mutex mutex;
    std::condition_variable variable;
    bool done;
    T data;

public:
    ProgressObservableMutexImpl() : done(false) {}
    void setData(const T &newData, bool complete) override
    {
        std::unique_lock<std::mutex> lock(mutex);
        data = newData;
        done = complete;
        variable.notify_all();
    }
    T getData() override
    {
        std::unique_lock<std::mutex> lock(mutex);
        return data;
    }
    T wait() override
    {
        std::unique_lock<std::mutex> lock(mutex);
        variable.wait(lock);
        return data;
    }
    bool isComplete() override
    {
        std::unique_lock<std::mutex> lock(mutex);
        return done;
    }
    void subscribe(std::function<void(const T &)> callback, std::function<void(const T &)> complete) override
    {
        std::unique_lock<std::mutex> lock(mutex);
        while (!done)
        {
            variable.wait(lock);
            if (!done)
                callback(data);
        }
        complete(data);
    }
};

template <typename T>
class ProgressObservablePipeImpl : public ProgressObservable<T>
{
public:
    static constexpr uint8_t TYPE_DATA = 1;
    static constexpr uint8_t TYPE_COMPLETE = 2;

protected:
    std::function<std::vector<char>(T)> serializer;
    int fd[2];

public:
    ProgressObservablePipeImpl(
        std::function<std::vector<char>(T)> transformFunction) : serializer(transformFunction)
    {
        if (pipe(fd) == -1)
        {
            throw "Can't create pipe";
        }
    }
    ProgressObservablePipeImpl(
            std::function<std::vector<char>(T)> transformFunction, int writeFd) : serializer(transformFunction)
    {
        fd[1] = writeFd;
        fd[0] = -1;
    }
    ~ProgressObservablePipeImpl()
    {
        // close(fd[0]);
        close(fd[1]);
    }
    void setData(const T &newData, bool complete) override
    {
        auto data = serializer(newData);
        uint32_t dataSize = htonl(data.size());
        write(fd[1], complete ? &TYPE_COMPLETE : &TYPE_DATA, 1);
        write(fd[1], &dataSize, 4);
        write(fd[1], data.data(), data.size());
        //flush(fd[1]);
    }
    int getReadFd() { return fd[0]; }
};

template <typename T>
class ProgressObserverPipeImpl : public ProgressObserver<T>
{
private:
    T data;
    std::function<T(char *, size_t)> deserializer;
    int readFd;
    bool done;

private:
    void readData()
    {
        uint32_t size;
        read(readFd, &size, 4);
        size = ntohl(size);
        if (size > 16 * 1024)
            throw "Size overflow";
        char *buffer = new char[size];
        ssize_t readed = read(readFd, buffer, size);
        data = deserializer(buffer, readed);
        delete buffer;
    }

public:
    ProgressObserverPipeImpl(int readFd,
                             std::function<T(char *, size_t)> deserializer) : readFd(readFd), deserializer(deserializer), done(false)
    {
    }
    T getData() { return data; }
    T wait()
    {
        uint8_t type;
        read(readFd, &type, 1);
        if (type == ProgressObservablePipeImpl<T>::TYPE_COMPLETE)
        {
            done = true;
            readData();
        }
        else if (type == ProgressObservablePipeImpl<T>::TYPE_DATA)
        {
            readData();
        }
        return data;
    }
    bool isComplete() { return done; }
    void subscribe(std::function<void(const T &)> callback, std::function<void(const T &)> complete) override
    {
        while (!done)
        {
            wait();
            if (!done)
                callback(data);
        }
        complete(data);
    }
};
