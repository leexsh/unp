# unix网络编程部分代码+注释
## 01_timeServer：unix中的获取服务器时间的demo
### 01_getTimeClient：获取服务时间的客户端
### 02_getTimeServer：获取服务器时间的服务器
### 03_bingfaServer:利用fork()实现多进程并发获取服务器时间的服务器
******
## 02_huixian：unix中服务器回显客户端发送内容的demo
### 04_huisheServer:回显客户端内容的服务器代码  
#### 代码组成：
1. 利用fork()实现的多进程服务器端的代码
2. 利用select函数实现的I/O模型服务器端代码
3. poll等等。。。
#### [select介绍(来自CSDN)](https://blog.csdn.net/turkeyzhou/article/details/8609360)
### 05_huisheClient：unix中服务器回显客户端内容的客户端代码
#### 注释部分
1. 最原始版本
2. 加入了select()函数，来监听向服务器写数据和从服务器读取数据
3. 在select()函数的基础上，加入了Shutdown()函数。
