/* Copyright (C) 2012 ~ 2013 Deepin, Inc.
 *               2012 ~ 2013 Long Changjin
 * 
 * Author:     Long Changjin <admin@longchangjin.cn>
 * Maintainer: Long Changjin <admin@longchangjin.cn>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// 该扩展是针对python2的
// 基本的自定义类型
#include <Python.h>
#include <structmember.h>

typedef struct {
    PyObject_HEAD
    /* Type-specific fields go here. */
    PyObject *first; /* first name */
    PyObject *last;  /* last name */
    int number;
} noddy_NoddyObject;

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

static void Noddy_dealloc(noddy_NoddyObject* self)
{
    PyObject_GC_UnTrack(self);
    Noddy_clear(self);
    PyObject_GC_Del(self);
}

static PyMemberDef Noddy_members[] = {
    {"first", T_OBJECT_EX, offsetof(noddy_NoddyObject, first), 0, "first name"},
    {"last", T_OBJECT_EX, offsetof(noddy_NoddyObject, last), 0, "last name"},
    {"number", T_INT, offsetof(noddy_NoddyObject, number), 0, "noddy number"},
    {NULL}  /* Sentinel */
};

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

static PyMethodDef noddy_methods[] = {
    {NULL}  /* Sentinel */
};

PyMODINIT_FUNC initnoddy(void) 
{
    PyObject* m;

    if (PyType_Ready(&noddy_NoddyType) < 0)
        return;

    m = Py_InitModule3("noddy", noddy_methods,
                       "Example module that creates an extension type.");

    Py_INCREF(&noddy_NoddyType);
    PyModule_AddObject(m, "Noddy", (PyObject *)&noddy_NoddyType);
}

