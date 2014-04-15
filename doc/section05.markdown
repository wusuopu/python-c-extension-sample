# 使用C语言编写Python扩展5——垃圾回收管理

上一节介绍了创建一个具有属性的类，由于对象具有属性数据，因此在进行内存管理时要多加注意。这一节就介绍一下Python的垃圾回收管理。  
在Python中垃圾回收主要是靠的计数引用方法，但是单凭计数引用还是不够的。先看看下面这段Python代码。  

    n = []
    m = []
    n.append(m)
    m.append(n)
    del m
    del n

如果只靠计数引用的话执行上面这段代码之后n和m都不能被回收，因为它们的引用计算值都不为0。  
像上面例子这样相互循环引用称作循环引用垃圾，在Python中有循环垃圾回收器（cyclic-garbage collector）专门用于回收此类计数引用无法处理的垃圾内存。  

接着上一节的例子，继续编辑*noddy.c*。  

为了让该对象类型支持垃圾回收，将PyTypeObject的tp_flags字段增加Py_TPFLAGS_HAVE_GC这个标志位。同时与GC(Garbage Collection)相关的tp_traverse和tp_clear这两个字段也要设置。  

* tp_traverse是用于垃圾回收器（garbage collector）遍历该实例对象中所有需要回收的属性对象。  
* tp_clear是用于清除内部各个属性对象的。  

首先定义tp_traverse和tp_clear所对应的函数：  

    static int Noddy_traverse(noddy_NoddyObject *self, visitproc visit, void *arg)
    {
        Py_VISIT(self->first);
        Py_VISIT(self->last);
        return 0;
    }
    static int Noddy_clear(noddy_NoddyObject *self)
    {
        Py_CLEAR(self->first);
        Py_CLEAR(self->last);
        return 0;
    }

Py_VISIT和Py_CLEAR是两个宏，简化了visit操作和clear操作。  

然后再修改noddy_NoddyType结构体定义：  

    static PyTypeObject noddy_NoddyType = {
        PyObject_HEAD_INIT(NULL)
        0,                         /*ob_size*/
        "noddy.Noddy",             /*tp_name*/
        sizeof(noddy_NoddyObject), /*tp_basicsize*/
        0,                         /*tp_itemsize*/
        (destructor)Noddy_dealloc, /*tp_dealloc*/
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
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC,  /*tp_flags*/
        "Noddy objects",           /*tp_doc*/
        (traverseproc)Noddy_traverse,   /* tp_traverse */
        (inquiry)Noddy_clear,      /* tp_clear */
        0,                         /* tp_richcompare */
        0,                         /* tp_weaklistoffset */
        0,                         /* tp_iter */
        0,                         /* tp_iternext */
        Noddy_methods,             /* tp_methods */
        Noddy_members,             /* tp_members */
        0,                         /* tp_getset */
        0,                         /* tp_base */
        0,                         /* tp_dict */
        0,                         /* tp_descr_get */
        0,                         /* tp_descr_set */
        0,                         /* tp_dictoffset */
        (initproc)Noddy_init,      /* tp_init */
        0,                         /* tp_alloc */
        Noddy_new,                 /* tp_new */
    };

**注意**：  
Python的官方手册中说道如果设置了Py_TPFLAGS_HAVE_GC这个标志位的话，那么就必须使用PyObject_GC_New这个函数来创建实例对象，使用PyObject_GC_Del来销毁已创建了的实例对象。  

使用PyObject_GC_New创建实例对象之后再用PyObject_GC_Track将该实例添加到垃圾回收器所跟踪的对象集合中去。  
在对象销毁时再执行PyObject_GC_UnTrack和PyObject_GC_Del函数。  

然后再修改tp_new函数和tp_dealloc函数：  

    static PyObject * Noddy_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
    {
        noddy_NoddyObject*self;
        self = (noddy_NoddyObject*)PyObject_GC_New(noddy_NoddyObject, type);
        if (self != NULL) {
            PyObject_GC_Track(self);
            self->first = PyString_FromString("");
            if (self->first == NULL)
            {
              Py_DECREF(self);
              return NULL;
            }
            self->last = PyString_FromString("");
            if (self->last == NULL)
            {
              Py_DECREF(self);
              return NULL;
            }
            self->number = 0;
        }
        return (PyObject *)self;
    }
    
    static void Noddy_dealloc(noddy_NoddyObject* self)
    {
        PyObject_GC_UnTrack(self);
        Noddy_clear(self);
        PyObject_GC_Del(self);
    }

最后再写一段Python程序来测试下该模块：  

    import gc
    import noddy
    
    gc.set_debug(gc.DEBUG_STATS | gc.DEBUG_LEAK)
    
    o = noddy.Noddy()
    l = [o]
    o.first = l
    del l
    del o
    
    gc.collect()



本文中的示例代码可从 https://github.com/wusuopu/python-c-extension-sample 获取到。  
