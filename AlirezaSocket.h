/* Copyright (C) 2016 Alireza Forouzandeh Nezhad <alirezafn@gmx.us> 
 * look NOTICE for the license 
 */

#ifndef ALIREZA_SOCKET_H
#define ALIREZA_SOCKET_H
#define  _WINSOCK_DEPRECATED_NO_WARNINGS 1

#include <iostream>
#include <winsock2.h>
#include <thread>
#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

#pragma comment(lib,"ws2_32.lib")

using namespace std;

class ThreadPool {
public:
    ThreadPool(size_t);
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) 
        -> std::future<typename std::result_of<F(Args...)>::type>;
    ~ThreadPool();
private:
    std::vector< std::thread > workers;
    std::queue< std::function<void()> > tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};
 
inline ThreadPool::ThreadPool(size_t threads)
    :   stop(false) {
    for(size_t i = 0;i<threads;++i)
        workers.emplace_back([this] {
                for(;;) {
                    std::function<void()> task; 
                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        this->condition.wait(lock,
                            [this]{ return this->stop || !this->tasks.empty(); });
                        if(this->stop && this->tasks.empty())
                            return;
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }
                    task();
                }
            }
        );
}

template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args) 
    -> std::future<typename std::result_of<F(Args...)>::type> {
    using return_type = typename std::result_of<F(Args...)>::type;
    auto task = std::make_shared< std::packaged_task<return_type()> >(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );   
    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        if(stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");

        tasks.emplace([task](){ (*task)(); });
    }
    condition.notify_one();
    return res;
}

inline ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for(std::thread &worker: workers)
        worker.join();
}

class AlirezaSocket {
public:
	void Init() {
		WSADATA wsa;
		if (WSAStartup(514, &wsa) != 0) {
			throw std::runtime_error("Failed. Error Code : %d" + WSAGetLastError());
		}
	}
	AlirezaSocket() {
		Init();
		if ((Socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
			throw std::runtime_error("Failed to create socket");
		}
	}
	bool Connect(char *ip, int port) {
		server.sin_addr.s_addr = inet_addr(ip);
		server.sin_family = AF_INET;
		server.sin_port = htons(port);
		if (connect(Socket, (struct sockaddr *)&server, sizeof(server)) < 0) {
			throw std::runtime_error("Failed to connecct");
			return 1;
		}
		onConnect(Socket);
		return true;
	}
	virtual void onConnect(SOCKET) = 0;
	void Listen(int port) {
		server.sin_family = AF_INET;
		server.sin_addr.s_addr = INADDR_ANY;
		server.sin_port = htons(port);
		if (::bind(Socket,(struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
			throw std::runtime_error("Failed to bind socket " + WSAGetLastError());
		}
		listen(Socket, 5);
		int c = (sizeof(struct sockaddr_in));
		ThreadPool client_pool(4);
		while (1) {
			SOCKET new_socket = accept(Socket, (struct sockaddr *)&client, &c);
			if (new_socket == INVALID_SOCKET) {
				throw std::runtime_error("accept failed with error code : %d" + WSAGetLastError());
			}
			client_pool.enqueue([this,new_socket]() {
				this->onConnect(new_socket);
			});
		}
	}
	bool sendString(SOCKET socket,string s) {
		return (send(socket,s.c_str(),s.length()+1,0) < 0);
	}
	string recieveString(SOCKET socket) {
		char input[4096] = {0};
		recv(socket,input,4096,0);
		string s(input);
		return s;
	}
	~AlirezaSocket() {
		closesocket(Socket);
	}
private:
	SOCKET Socket;
	struct sockaddr_in server,client;
};

#endif