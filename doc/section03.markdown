# 使用C语言编写Python扩展3——创建自定义类型(1)

在Python代码中如果要创建一个自定义对象使用class关键字即可，但是在C代码中就没那么方便了。  
首先简单介绍下Python中的类型。在python中一切皆对象，python中有两种对象：  
一种是类型对象（class对象）：表示Python定义的类型，例如int, str, object等；  
另一种是实例对象（instance对象）：表示由class对象创建的实例。 
Python中的所有对象都是直接或者间接继承object，然后object又是typy类型。可以运行下面的例子看看输出结果：  

    class A(object):
        pass
    
    a = A()
    
    print(type(a))
    print(isinstance(a, A))
    print(isinstance(a, object))
    print(isinstance(a, type))
    
    print(type(A))
    print(A.__base__)
    print(isinstance(A, object))
    print(isinstance(A, type))
    
    print(type(object))
    print(isinstance(object, type))
    
    print(type(type))
    print(isinstance(type, object))

python是一门面向对象的编程语言，它是用C写的，而C又是面向过程的编程语言，那么python的类在C中是如何实现的呢？答案就是用结构体来模拟。  

在Python的*object.h*头文件中定义了一个重要的结构体 PyTypeObject 。创建新的类型就是靠的它，该结构体定义如下：  

    typedef struct _typeobject {
        PyObject_VAR_HEAD
        char *tp_name; /* For printing, in format "<module>.<name>" */
        int tp_basicsize, tp_itemsize; /* For allocation */
        
        /* Methods to implement standard operations */
        
        destructor tp_dealloc;
        printfunc tp_print;
        getattrfunc tp_getattr;
        setattrfunc tp_setattr;
        cmpfunc tp_compare;
        reprfunc tp_repr;
        
        /* Method suites for standard classes */
        
        PyNumberMethods *tp_as_number;
        PySequenceMethods *tp_as_sequence;
        PyMappingMethods *tp_as_mapping;
        
        /* More standard operations (here for binary compatibility) */
        
        hashfunc tp_hash;
        ternaryfunc tp_call;
        reprfunc tp_str;
        getattrofunc tp_getattro;
        setattrofunc tp_setattro;
        
        /* Functions to access object as input/output buffer */
        PyBufferProcs *tp_as_buffer;
        
        /* Flags to define presence of optional/expanded features */
        long tp_flags;
        
        char *tp_doc; /* Documentation string */
        
        /* Assigned meaning in release 2.0 */
        /* call function for all accessible objects */
        traverseproc tp_traverse;
        
        /* delete references to contained objects */
        inquiry tp_clear;
        
        /* Assigned meaning in release 2.1 */
        /* rich comparisons */
        richcmpfunc tp_richcompare;
        
        /* weak reference enabler */
        long tp_weaklistoffset;
        
        /* Added in release 2.2 */
        /* Iterators */
        getiterfunc tp_iter;
        iternextfunc tp_iternext;
        
        /* Attribute descriptor and subclassing stuff */
        struct PyMethodDef *tp_methods;
        struct PyMemberDef *tp_members;
        struct PyGetSetDef *tp_getset;
        struct _typeobject *tp_base;
        PyObject *tp_dict;
        descrgetfunc tp_descr_get;
        descrsetfunc tp_descr_set;
        long tp_dictoffset;
        initproc tp_init;
        allocfunc tp_alloc;
        newfunc tp_new;
        freefunc tp_free; /* Low-level free-memory routine */
        inquiry tp_is_gc; /* For PyObject_IS_GC */
        PyObject *tp_bases;
        PyObject *tp_mro; /* method resolution order */
        PyObject *tp_cache;
        PyObject *tp_subclasses;
        PyObject *tp_weaklist;
    } PyTypeObject;

这个比较庞大，里面包含的数据比较多，大部分都是一些函数指针而且可以为空，至于每个字段是什么意思请查看Python文档。  


## 创建自定义类型
创建一个新的C代码文件 *noddy.c* ，然后我们编写一个名为 noddy 的扩展模块，该模块包含了一个名为 Noddy 的类。  

首先创建一个新的 PyTypeObject 类型的变量：

    typedef struct {
        PyObject_HEAD
        /* Type-specific fields go here. */
    } noddy_NoddyObject;
    static PyTypeObject noddy_NoddyType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "noddy.Noddy",             /*tp_name*/
        sizeof(noddy_NoddyObject), /*tp_basicsize*/
        0,                         /*tp_itemsize*/
        0,                         /*tp_dealloc*/
        0,                         /*tp_print*/
        0,                         /*tp_getattr*/
        0,                         /*tp_setattr*/
        0,                         /*tp_compare*/
        0,                         /*tp_repr*/
        0,                         /*tp_as_number*/
        0,                         /*tp_as_sequence*/
        0,                         /*tp_as_mapping*/
        0,                         /*tp_hash */
        0,                         /*tp_call*/
        0,                         /*tp_str*/
        0,                         /*tp_getattro*/
        0,                         /*tp_setattro*/
        0,                         /*tp_as_buffer*/
        Py_TPFLAGS_DEFAULT,        /*tp_flags*/
        "Noddy objects",           /*tp_doc*/
    };

这里是定义了一个noddy_NoddyObject结构体，它的第一个字段为 PyObject_HEAD ，因此相当于一个PyObject类型；然后还有一个 noddy_NoddyType 变量，它的第一个字段为 *PyVarObject_HEAD_INIT(NULL)* ，这个很很重要，按理说这个应该写成 *PyVarObject_HEAD_INIT(&PyType_Type, 0)* ，即表示Noddy这个类是一个type类型的对象。不过有的C编译器会对这个报错，因此这一项将在后面调用PyType_Ready函数来填充。  
noddy_NoddyType 即是 Noddy 类，它保存了该类的元信息；noddy_NoddyObject结构体用于保存该类的实例对象的数据。  
*只要是定义的结构体以PyObject_HEAD开始就属于是一个PyObject类型。PyObject_VAR_HEAD与PyObject_HEAD相似，只不PyObject_HEAD表示的是该类型占用内存大小是固定的如int、float；而PyObject_VAR_HEAD表示该类型占用的内存是可变的如list、dict。*   

然后创建一个新扩展模块，并完成初始化：

    static PyMethodDef noddy_methods[] = {
        {NULL}  /* Sentinel */
    };
    PyMODINIT_FUNC
    initnoddy(void) 
    {
        PyObject* m;
    
        noddy_NoddyType.tp_new = PyType_GenericNew;
        if (PyType_Ready(&noddy_NoddyType) < 0)
            return;
    
        m = Py_InitModule3("noddy", noddy_methods,
                           "Example module that creates an extension type.");
    
        Py_INCREF(&noddy_NoddyType);
        PyModule_AddObject(m, "Noddy", (PyObject *)&noddy_NoddyType);
    }

***注意***：以上是针对Python2的，在Python3中模块的初始化操作略有不同。请参考第一节的内容。  

noddy_NoddyType即是我们要创建的 Noddy 类，它是 PyTypeObject 类型的结构变量。为了创建新的类型，我们需要指明 tp_new 方法，它相当于Python中的 __new__，这里我们使用默认的 PyType_GenericNew 即可。  
然后调用 PyType_Ready 完成新类型的创建。  
最后调用 PyModule_AddObject 在该模块中添加刚刚创建的新类型。  


## 测试
最后就是编写一个小程序来测试刚刚的模块是否可用。

    import noddy
    
    o = noddy.Noddy()
    print(o)
    print(type(o), type(noddy.Noddy))
    
    # noddy.Noddy 类不能被继承
    class A(noddy.Noddy):
        pass
