# 使用C语言编写Python扩展2——函数

上一节介绍了编写扩展的基本流程。这一回介绍一下在扩展模块中的函数调用，包括在扩展函数的参数提取和关键字参数解析，以及在C语言中调用Python方法。

同样的本文中的示例代码可从 https://github.com/wusuopu/python-c-extension-sample 获取到。  


## 参数提取
接着上一节的例子，我们继续编辑lc_hello.c文件。先往模块中添加一个名为  func1 的函数，即就是在 lc_hello_world_methods 数组中添加一项：  

    {"func1", (PyCFunction)func1_function, METH_VARARGS, NULL},

然后就是对该函数的实现。  
参数提取是使用 PyArg_ParseTuple 方法，其定义如下：  

    int PyArg_ParseTuple(PyObject *arg, char *format, ...);

其中 *arg* 参数为Python向C函数传递的参数列表，是一个无组对象；*format* 参数是一个格式化字符串，它的格式可以参考 Python/C API 文档。  
func1_function 函数实现如下：  

    static PyObject* func1_function(PyObject *self, PyObject *args)
    {
        int num, i, j;
        long lnum=0;
        const char* s1 = NULL;
        PyObject *obj = NULL;
        if (!PyArg_ParseTuple(args, "is(ii)|l",
                              &num, &s1, &i, &j, &lnum)) {
            printf("传入参数错误！\n");
            return NULL;
        }
        printf("num: %d\tstr1: %s\n"
               "i: %d\tj: %d\tlnum: %ld\n",
               num, s1, i, j, lnum);

        obj = Py_BuildValue("{sisisislss}",
                            "num", num, "i", i, "j", j, "lnum", lnum, "s1", s1);
        return obj;
    }

在Python中该函数可以接收3个或者4个参数。同时该函数使用了 Py_BuildValue 方法构造了一个字典对象并返回。Py_BuildValue的用法与PyArg_ParseTuple类似。  
接下来可以在Python中进行测试：  

    print(lc_hello_world.func1(11, 'abc', (2, 3), 100))
    print(lc_hello_world.func1(11, 'abc', (2, 3)))

## 关键字参数
再在 lc_hello_world_methods 数组中添加一项：  

    {"func2", (PyCFunction)func2_function, METH_VARARGS | METH_KEYWORDS, NULL},

关键字参数解析是使用 PyArg_ParseTupleAndKeywords 方法，其定义如下：  

    int PyArg_ParseTupleAndKeywords(PyObject *arg, PyObject *kwdict,
                                    char *format, char *kwlist[], ...);

其中 *arg* 参数和 *format* 参数与PyArg_ParseTuple一样。*kwdict*参数是一个字典对象，保存了关键字参数。*kwlist*是一个以NULL结尾的字符串数组。  
func2_function 函数实现如下：  

    static PyObject* func2_function(PyObject *self, PyObject *args, PyObject *kwargs)
    {
        int voltage;
        char *state = "a stiff";
        char *action = "voom";
        char *type = "Norwegian Blue";

        static char *kwlist[] = {"voltage", "state", "action", "type", NULL};

        if (!PyArg_ParseTupleAndKeywords(args, kwargs, "i|sss", kwlist,
                                         &voltage, &state, &action, &type))
            return NULL;

        printf("-- This parrot wouldn't %s if you put %i Volts through it.\n",
               action, voltage);
        printf("-- Lovely plumage, the %s -- It's %s!\n", type, state);
        Py_INCREF(Py_None);
        return Py_None;
    }

接下来可以在Python中进行测试：  

    lc_hello_world.func2(state="ok", action="test", type="func", voltage=13)
    lc_hello_world.func2(20)

## 在扩展模块中调用Python方法
在扩展模块中可以使用  PyObject_CallObject 方法来调用Python的函数方法。其定义如下：  

    PyObject* PyObject_CallObject(PyObject *callable_object, PyObject *args)

再在 lc_hello_world_methods 数组中添加一项：  

    {"func3", (PyCFunction)func3_function, METH_VARARGS, NULL},

func3_function 函数实现如下：  

    static PyObject* func3_function(PyObject *self, PyObject *args)
    {
        PyObject *my_callback = NULL;
        PyObject *result = NULL;
        PyObject *arg = NULL;
        if (!PyArg_ParseTuple(args, "OO:set_callback;argument;", &my_callback, &arg)) {
            printf("传入参数错误！\n");
            return NULL;
        }
        if (!PyCallable_Check(my_callback)) {
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }
        result = PyObject_CallObject(my_callback, arg);
        if (!result) {
            Py_INCREF(Py_None);
            result = Py_None;
        }
        return result;
    }

接下来可以在Python中进行测试：  

    print(lc_hello_world.func3(int, (1.234, )))
