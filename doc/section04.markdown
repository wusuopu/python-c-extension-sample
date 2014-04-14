# 使用C语言编写Python扩展4——创建自定义类型(2)

上一节中我们创建了一个简单的类。这一节我们将对这个类进行扩展，添加属性、方法，并且支持子类。

## 为类型添加方法和数据

接着上一节的例子，继续编辑*noddy.c*。  

    typedef struct {
        PyObject_HEAD
        /* Type-specific fields go here. */
        PyObject *first; /* first name */
        PyObject *last;  /* last name */
        int number;
    } noddy_NoddyObject;

修改 noddy_NoddyObject 结构体，为其添加三个字段。  


然后定义自己的__new__方法，为对象分配内存空间：  

    static PyObject * Noddy_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
    {
        noddy_NoddyObject*self;
        self = (noddy_NoddyObject*)type->tp_alloc(type, 0);
        if (self != NULL) {
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

接着定义对象的初始化函数__init__：  

    static int Noddy_init(noddy_NoddyObject*self, PyObject *args, PyObject *kwds)
    {
        PyObject *first=NULL, *last=NULL, *tmp;
        static char *kwlist[] = {"first", "last", "number", NULL};

        if (! PyArg_ParseTupleAndKeywords(args, kwds, "|OOi", kwlist, 
                                          &first, &last, 
                                          &self->number))
            return -1; 

        if (first) {
            tmp = self->first;
            Py_INCREF(first);
            self->first = first;
            Py_XDECREF(tmp);
        }

        if (last) {
            tmp = self->last;
            Py_INCREF(last);
            self->last = last;
            Py_XDECREF(tmp);
        }
        return 0;
    }

由于对象包含了几项数据，因此在对象销毁时需要先释放数据的资源。定义资源释放方法：

    static void Noddy_dealloc(noddy_NoddyObject* self)
    {
        Py_XDECREF(self->first);
        Py_XDECREF(self->last);
        Py_TYPE(self)->tp_free((PyObject*)self);
    }

然后再为该类定义一个方法用于返回该对象的first值和last值：  

    static PyObject * Noddy_name(noddy_NoddyObject* self)
    {
        static PyObject *format = NULL;
        PyObject *args, *result;
        if (format == NULL) {
            format = PyString_FromString("%s %s");
            if (format == NULL)
                return NULL;
        }
        if (self->first == NULL) {
            PyErr_SetString(PyExc_AttributeError, "first");
            return NULL;
        }
        if (self->last == NULL) {
            PyErr_SetString(PyExc_AttributeError, "last");
            return NULL;
        }
        args = Py_BuildValue("OO", self->first, self->last);
        if (args == NULL)
            return NULL;
        result = PyString_Format(format, args);
        Py_DECREF(args);
        return result;
    }
    static PyMethodDef Noddy_methods[] = {
        {"name", (PyCFunction)Noddy_name, METH_NOARGS, "Return the name, combining the first and last name"},
        {NULL}  /* Sentinel */
    };

最后在定义 noddy_NoddyType 变量时将对应字段进行填充：

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
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,  /*tp_flags*/
        "Noddy objects",           /*tp_doc*/
        0,                         /* tp_traverse */
        0,                         /* tp_clear */
        0,                         /* tp_richcompare */
        0,                         /* tp_weaklistoffset */
        0,                         /* tp_iter */
        0,                         /* tp_iternext */
        Noddy_methods,             /* tp_methods */
        0,                         /* tp_members */
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

tp_flags字段增加了  Py_TPFLAGS_BASETYPE 属性，表示该类可以被继承。最后进行编译测试：  

    import noddy
    
    o = noddy.Noddy("abc", "def", 12)
    print(o, o.name())
    print(type(o), type(noddy.Noddy))
    print(o.number, o.first, o.last)
    
    class A(noddy.Noddy):
        pass

运行以上程序会发现Noddy对象没有number、first和last这几个属性。这是因为虽然在noddy_NoddyObject结构体中定义了这几个字段，但是它们仍然在Python中是不可见的。  
为了能在Python中访问这几个属性，需要设置noddy_NoddyType的tp_members字段。  

    static PyMemberDef Noddy_members[] = {
        {"first", T_OBJECT_EX, offsetof(noddy_NoddyObject, first), 0, "first name"},
        {"last", T_OBJECT_EX, offsetof(noddy_NoddyObject, last), 0, "last name"},
        {"number", T_INT, offsetof(noddy_NoddyObject, number), 0, "noddy number"},
        {NULL}  /* Sentinel */
    };

先定义一个 PyMemberDef 结构体类型的数组，然后将noddy_NoddyType的tp_members字段设为Noddy_members。PyMemberDef和T_OBJECT_EX以及T_INT均是在 *structmember.h* 头文件中定义的，因此还需要先包含该文件。  

    #include <structmember.h>


## 数据属性的访问控制
数据属性的访问控制可以对属性的设置进行合法性检查，例如这里我们想要确保 Noddy 对象的first属性和last属性都必须是字符串。  
首先定义属性的get方法和set方法：  

    static PyObject * Noddy_getfirst(noddy_NoddyObject *self, void *closure)
    {
        Py_INCREF(self->first);
        return self->first;
    }
    static int Noddy_setfirst(noddy_NoddyObject *self, PyObject *value, void *closure)
    {
      if (value == NULL) {
        PyErr_SetString(PyExc_TypeError, "Cannot delete the first attribute");
        return -1;
      }
      if (! PyString_Check(value)) {
        PyErr_SetString(PyExc_TypeError, 
                        "The first attribute value must be a string");
        return -1;
      }
      Py_DECREF(self->first);
      Py_INCREF(value);
      self->first = value;    
      return 0;
    }
    static PyObject * Noddy_getlast(noddy_NoddyObject *self, void *closure)
    {
        Py_INCREF(self->last);
        return self->last;
    }
    static int Noddy_setlast(noddy_NoddyObject *self, PyObject *value, void *closure)
    {
      if (value == NULL) {
        PyErr_SetString(PyExc_TypeError, "Cannot delete the last attribute");
        return -1;
      }
      if (! PyString_Check(value)) {
        PyErr_SetString(PyExc_TypeError, 
                        "The last attribute value must be a string");
        return -1;
      }
      Py_DECREF(self->last);
      Py_INCREF(value);
      self->last = value;    
      return 0;
    }

然后创建一个 PyGetSetDef 结构类型的数组：  

    static PyGetSetDef Noddy_getseters[] = {
        {"first", (getter)Noddy_getfirst, (setter)Noddy_setfirst, "first name", NULL},
        {"last", (getter)Noddy_getlast, (setter)Noddy_setlast, "last name", NULL},
        {NULL}  /* Sentinel */
    };

最后再设置 noddy_NoddyType 的tp_getset字段的值为 Noddy_getseters 即可。  


**注意**：以上的代码均是针对Python2的，在Python3中略有不同。  

本文中的示例代码可从 https://github.com/wusuopu/python-c-extension-sample 获取到。  

