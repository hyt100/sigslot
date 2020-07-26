# sigslot

A simple signal-slot implement in c++11.

**信号槽定义**

在信号端记录了所绑定的函数指针，在槽函数记录了其绑定的信号。

**特点**

- 回调函数不支持返回值，只能是void
- 支持普通函数指针、成员函数指针
- 槽函数端不做记录