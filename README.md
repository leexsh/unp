# unix网络编程部分代码+注释
## 01_timeServer：unix中的获取服务器时间的demo
### 01_getTimeClient：获取服务时间的客户端
### 02_getTimeServer：获取服务器时间的服务器
### 03_bingfaServer:利用fork()实现多进程并发获取服务器时间的服务器
******
## 02_tcpDemo：unix中服务器利用TCP来回显客户端发送内容的demo
### 04_huisheServer:回显客户端内容的服务器代码  
#### 代码组成：
1. 利用fork()实现的多进程服务器端的代码
2. 利用select函数实现的I/O复用模型服务器端代码
#### [select介绍(来自CSDN)](https://blog.csdn.net/turkeyzhou/article/details/8609360)
3. 利用poll()函数实现的I/O复用服务器端代码
#### [poll机制介绍(来自CSDN)](https://blog.csdn.net/q2519008/article/details/84262193)
### 05_huisheClient：unix中服务器回显客户端内容的客户端代码
#### 代码组成：
1. 最原始版本
2. 加入了select()函数，来监听向服务器写数据和从服务器读取数据
3. 在select()函数的基础上，加入了Shutdown()函数。
****
## 03_udpDemo：unix中的服务器利用UDP来回显客户端发送内容的demo
