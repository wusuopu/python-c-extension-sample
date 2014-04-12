# 使用C语言编写Python扩展1——Hello World

能够使用C语言编写扩展是Python一大卖点吧，这可以将一些关键的代码使用C来写以提升程序的性能。本文是参考了Python的官方文档整理而来的，同时结合了Python2跟Python3。按照惯例现在先从一个Hello World开始讲解一下写扩展的基本流程。

详细的内容可以参考官方文档：  
https://docs.python.org/2.7/extending/index.html   
https://docs.python.org/3/extending/index.html  

https://docs.python.org/2.7/c-api/index.html  
https://docs.python.org/3/c-api/index.html  

同时本文中的示例代码可从 https://github.com/wusuopu/python-c-extension-sample 获取到。  

首先介绍一下我当前的开发环境：  
 * ArchLinux  
 * gcc 4.8.2  
 * glibc 2.19  
 * Python 2.7.6  
 * Python 3.3.5  


## 开始
先创建一个新的C代码文件 lc_hello.c。为了能够正常使用python的api，需要导入Python.h这个头文件。

    #include <Python.h>

然后再定义一个模块的初始化函数。

    PyMODINIT_FUNC initlc_hello_world(void)
    {
        Py_InitModule("lc_hello_world", lc_hello_world_methods);
        printf("init lc_hello_world module\n");
    }

这个函数是用于模块初始化的，即是在第一次使用import语句导入模块时会执行。其函数名必须为initmodule_name这样的格式，在这里我们的模块名为lc_hello_world，所以函数名就是initlc_hello_world。  
在这个函数中又调用了Py_InitModule函数，它执行了模块初始化的操作。Py_InitModule函数传入了两个参数，第一个参数为字符串，表示模块的名称；第二个参数是一个PyMethodDef的结构体数组，表示该模块都具有哪些方法。与Py_InitModule相似的方法还有Py_InitModule3和Py_InitModule4。因此在initlc_hello_world方法之前还需要先定义 lc_hello_world_methods 数组。  

    static PyMethodDef lc_hello_world_methods[] = {
        {"test", (PyCFunction)test_function, METH_NOARGS, "lc_hello_world extending test"},
        {"add", (PyCFunction)add_function, METH_VARARGS, NULL},
        {NULL, NULL, 0, NULL}
    };

PyMethodDef结构体有四个字段。  
  * 第一个是一个字符串，表示在Python中对应的方法的名称；  
  * 第二个是对应的C代码的函数；  
  * 第三个是一个标致位，表示该Python方法是否需要参数，METH_NOARGS表示不需要参数，METH_VARARGS表示需要参数；  
  * 第四个是一个字符串，它是该方法的__doc__属性，这个不是必须的，可以为NULL。  
PyMethodDef结构体数组最后以 {NULL, NULL, 0, NULL}结尾。（感觉好像不是必须的，但是通常都这么做那我们也这么做吧）  

**注意**：以上的用法都是针对Python2的，在Python3中又有些不同。  
在Python3中模块的初始化函数的函数名变为了PyInit_module_name这样的形式了，因此这里就需要定义一个函数 PyMODINIT_FUNC PyInit_lc_hello_world。并且还需要返回一个 module 类型的变量。  
其次在Python3中创建module对象的函数也由 Py_InitModule 变为了 PyModule_Create。  
因此在Python3中模块的初始化函数应该定义如下：  

    PyMODINIT_FUNC PyInit_lc_hello_world(void)
    {
        PyObject *m;
        m = PyModule_Create(&lc_hello_world_module);
        if (m == NULL)
            return NULL;
        printf("init lc_hello_world module\n");
        return m;
    }

PyModule_Create函数需要传入一个 PyModuleDef 类型的指针。  
因此在此之前还需要先定义 lc_hello_world_module 变量。  

    static struct PyModuleDef lc_hello_world_module = {
        PyModuleDef_HEAD_INIT,
        "lc_hello_world",        /* name of module */
        NULL,                    /* module documentation, may be NULL */
        -1,                      /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
        lc_hello_world_methods   /* A pointer to a table of module-level functions, described by PyMethodDef values. Can be NULL if no functions are present. */
    };

在 lc_hello_world_methods 中我们为模块指定了两个方法，接下来我们需要实现这两个方法。  

    static PyObject* test_function(PyObject *self)
    {
        PyObject_Print(self, stdout, 0);
        printf("lc_hello_world test\n");
        Py_INCREF(Py_True);
        return Py_True;
    }

这段代码定义了Python的test方法所对应的C函数。在这个函数中就只执行了一条printf语句，然后就返回了Py_True。  
Py_True即是Python中的True，Py_INCREF函数执行的操作是对Python对象的计数引用值进行加1。与Py_INCREF对应的是Py_DECREF，它是对计数引用减1,并且计数引用为0时就销毁对象并回收内存。  

    static PyObject* add_function(PyObject *self, PyObject *args)
    {
        int num1, num2;
        PyObject *result=NULL;
        if (!PyArg_ParseTuple(args, "nn", &num1, &num2)) {
            printf("传入参数错误！\n");
            return NULL;
        }
        result = PyInt_FromLong(num1+num2);
        return result;
    }

这须代码定义了Python的add方法所对应的C函数。该函数需要传入两个整数类型的参数。  
PyArg_ParseTuple是对传入的参数进行解析，关于这个函数的说明请查看Python手册。  

**注意**：在Python3中整数都是 long 类型的，因此这里的 PyInt_FromLong 需要改为 PyLong_FromLong，其作用是将C的int类型转为Python的int类型。  


## 编译
扩展模块编写完成后，接下来就是对其进行编译了。先编写一个 setup.py 脚本。  

    #!/usr/bin/env python
    #-*- coding:utf-8 -*-
    
    from setuptools import setup, Extension
    
    hello_world = Extension('lc_hello_world', sources=["lc_hello.c"])
    setup(ext_modules=[hello_world])

然后再执行命令进行编译：

    $ python setup.py build

执行成功后会在当前目录下的build目录中生成扩展模块文件。

## 测试
最后就是编写一个小程序来测试刚刚的模块是否可用。

    import lc_hello_world
    
    print(lc_hello_world.test.__doc__)
    print(lc_hello_world.add.__doc__)
    print(lc_hello_world.test())
    print(lc_hello_world.add(1, 2))
    print(lc_hello_world.add(1, '2'))    # 这个会报错


