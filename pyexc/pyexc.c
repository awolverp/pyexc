/*
Name: pyexc
Author: awolverp
*/
#include <Python.h>
#include <stdio.h>

typedef struct
{
    PyObject *exc_type; // Exception
    PyObject *exc_data; // Any
} PyexcModuleState;

#define pyexcstate(o) ((PyexcModuleState *)PyModule_GetState(o))

inline int pyexc_clear_state(PyexcModuleState *state)
{
    if (state->exc_type == NULL && state->exc_data == NULL)
    {
        return 0;
    }

    if (state->exc_type != NULL)
    {
        Py_XDECREF(state->exc_type);
        state->exc_type = NULL;
    }
    if (state->exc_data != NULL)
    {
        Py_XDECREF(state->exc_data);
        state->exc_data = NULL;
    }

    return 1;
}

/**
 * @brief information about exception.
 *
 * @returns
 * Returns tuple[Exception | None, Any].
 */
static PyObject *Pyexc_data(PyObject *m)
{
    PyexcModuleState *state = NULL;

    Py_BEGIN_ALLOW_THREADS
        state = pyexcstate(m);
    Py_END_ALLOW_THREADS;

    return Py_BuildValue(
        "(O, O)",
        (state->exc_type == NULL) ? Py_None : state->exc_type, // None | Exception
        (state->exc_data == NULL) ? Py_None : state->exc_data  // None | str
    );
}

/**
 * @brief Clears occurred exception.
 *
 * @returns
 * Returns True if ok.
 */
static PyObject *Pyexc_clear(PyObject *m)
{
    PyexcModuleState *state = NULL;

    Py_BEGIN_ALLOW_THREADS
        state = pyexcstate(m);
    Py_END_ALLOW_THREADS;

    if (!pyexc_clear_state(state))
        Py_RETURN_FALSE;

    else
        Py_RETURN_TRUE;
}

/**
 * @brief Set exception.
 *
 * @param type (PyObject*) Exception type.
 * @param data (PyObject*) Exception argument.
 * @param block (bool) If True and an exception setted, break and returns False. (default False)
 *
 * @returns
 * Returns True if setted.
 */
static PyObject *Pyexc_set(PyObject *m, PyObject *args, PyObject *kwds)
{
    static char *keywords[] = {(char *)"type", (char *)"args", (char *)"block", NULL};
    PyObject *type = NULL;
    PyObject *data = NULL;
    int block = 0;

    PyexcModuleState *state = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O|Op", keywords, &type, &data, &block))
        return NULL;

    Py_INCREF(type);
    if (!PyObject_IsInstance(type, (PyObject *)Py_TYPE(PyExc_BaseException)) && !PyObject_IsInstance(type, PyExc_BaseException))
    {
        Py_XDECREF(type);
        PyErr_SetString(PyExc_TypeError, "`type` argument of `pyexc.set` function must be instance of BaseException.");
        return NULL;
    }

    Py_BEGIN_ALLOW_THREADS
        state = pyexcstate(m);
    Py_END_ALLOW_THREADS;

    if (state->exc_type != NULL)
    {
        if (block)
        {
            Py_XDECREF(type);
            Py_RETURN_FALSE;
        }

        pyexc_clear_state(state);
    }

    state->exc_type = type;

    if (data != NULL)
    {
        Py_INCREF(data);
        state->exc_data = data;
    }

    Py_RETURN_TRUE;
}

inline void pyexc_raise_exception(PyObject *exc, PyObject *data)
{
    if (PyObject_IsInstance(exc, (PyObject *)Py_TYPE(PyExc_BaseException)))
    {
        if (data == NULL)
            PyErr_SetNone(exc);
        else
            PyErr_SetObject(exc, data);
    }
    else if (PyObject_IsInstance(exc, PyExc_BaseException))
    {
        PyTypeObject *type = Py_TYPE(exc);
        PyBaseExceptionObject *value = (PyBaseExceptionObject *)exc;
        PyErr_SetObject((PyObject *)type, (PyObject *)value);
    }
}

/**
 * @brief Raise setted exception.
 *
 * @param default (PyObject*) Default exception if not exception setted. (default SystemError)
 */
static PyObject *Pyexc_raise_exc(PyObject *m, PyObject *args, PyObject *kwds)
{
    static char *keywords[] = {(char *)"default", NULL};
    PyObject *default_exception = NULL;

    PyexcModuleState *state = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|O", keywords, &default_exception))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
        state = pyexcstate(m);
    Py_END_ALLOW_THREADS;

    if (default_exception != NULL)
    {
        if (!PyObject_IsInstance(default_exception, (PyObject *)Py_TYPE(PyExc_BaseException)) && !PyObject_IsInstance(default_exception, PyExc_BaseException))
        {
            PyErr_SetString(PyExc_TypeError, "`default` argument of `pyexc.raise_exc` function must be instance of BaseException.");
            return NULL;
        }
    }

    if (state->exc_type == NULL)
    {
        pyexc_raise_exception((default_exception != NULL) ? default_exception : PyExc_SystemError, NULL);
        return NULL;
    }

    pyexc_raise_exception(state->exc_type, state->exc_data);
    pyexc_clear_state(state);
    return NULL;
}

/**
 * Returns True if an exception setted.
 */
static PyObject *Pyexc_occurred(PyObject *m)
{
    PyexcModuleState *state = NULL;

    Py_BEGIN_ALLOW_THREADS
        state = pyexcstate(m);
    Py_END_ALLOW_THREADS;

    return PyBool_FromLong((long)(state->exc_type != NULL ? 1 : 0));
}

static PyObject *Pyexc_print_exc(PyObject *m, PyObject *args, PyObject *kwds)
{
    static char *keywords[] = {(char *)"exception", NULL};
    PyObject *exception = NULL;

    PyexcModuleState *state = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|O", keywords, &exception))
        return NULL;

    if (exception != NULL)
    {
        if (!PyObject_IsInstance(exception, (PyObject *)Py_TYPE(PyExc_BaseException)) && !PyObject_IsInstance(exception, PyExc_BaseException))
        {
            PyErr_SetString(PyExc_TypeError, "`default` argument of `pyexc.print_exc` function must be instance of BaseException.");
            return NULL;
        }
        PyErr_Clear();
        pyexc_raise_exception(exception, NULL);
        PyErr_Print();
        PyErr_Clear();
        Py_RETURN_NONE;
    }

    Py_BEGIN_ALLOW_THREADS
        state = pyexcstate(m);
    Py_END_ALLOW_THREADS;

    if (state->exc_type == NULL)
        Py_RETURN_NONE;

    PyErr_Clear();
    pyexc_raise_exception(state->exc_type, state->exc_data);
    PyErr_Print();
    PyErr_Clear();
    Py_RETURN_NONE;
}

static PyObject *Pyexc_get_exc_info(PyObject *m)
{
    PyexcModuleState *state = NULL;

    Py_BEGIN_ALLOW_THREADS
        state = pyexcstate(m);
    Py_END_ALLOW_THREADS;

    if (state->exc_type == NULL)
        Py_RETURN_NONE;

    PyObject *result = NULL;

    if (PyObject_IsInstance(state->exc_type, (PyObject *)Py_TYPE(PyExc_BaseException)))
    {
        if (state->exc_data == NULL)
        {
            result = Py_BuildValue("(O,O,O)", state->exc_type, Py_None, Py_None);
        }
        else
        {
            result = Py_BuildValue("(O,O,O)", state->exc_type, state->exc_data, Py_None);
        }
    }
    else if (PyObject_IsInstance(state->exc_type, PyExc_BaseException))
    {
        PyBaseExceptionObject *value = (PyBaseExceptionObject *)state->exc_type;
        result = Py_BuildValue("(O,O,O)", state->exc_type, (value->args != NULL) ? value->args : Py_None, (value->traceback != NULL) ? value->traceback : Py_None);
    }

    return result;
}

static int __Pyexc_traverse(PyObject *m, visitproc visit, void *arg)
{
    Py_VISIT(pyexcstate(m)->exc_type);
    Py_VISIT(pyexcstate(m)->exc_data);
    return 0;
}

static int __Pyexc_clear(PyObject *m)
{
    Py_CLEAR(pyexcstate(m)->exc_type);
    Py_CLEAR(pyexcstate(m)->exc_data);
    return 0;
}

static PyMethodDef pyexc_methods[] = {
    {"data", (PyCFunction)Pyexc_data, METH_NOARGS, "Returns setted exception and args as tuple."},
    {"clear", (PyCFunction)Pyexc_clear, METH_NOARGS, "Clears setted exception and args."},
    {"set", (PyCFunction)Pyexc_set, METH_VARARGS | METH_KEYWORDS, "Set exception."},
    {"raise_exc", (PyCFunction)Pyexc_raise_exc, METH_VARARGS | METH_KEYWORDS, "Raise setted exception."},
    {"occurred", (PyCFunction)Pyexc_occurred, METH_NOARGS, "Returns True if an exception setted."},
    {"print_exc", (PyCFunction)Pyexc_print_exc, METH_VARARGS | METH_KEYWORDS, "Print exception in `stderr` (with traceback)."},
    {"exc_info", (PyCFunction)Pyexc_get_exc_info, METH_NOARGS, "Returns exception info as tuple."},
    {NULL, NULL, 0, NULL},
};

struct PyModuleDef pyexc_module = {
    PyModuleDef_HEAD_INIT,
    .m_name = "pyexc",
    .m_doc = "PyExc - Python Exception Managing...",
    .m_size = sizeof(PyexcModuleState),
    .m_methods = pyexc_methods,
    .m_traverse = (traverseproc)__Pyexc_traverse,
    .m_clear = (inquiry)__Pyexc_clear};

PyMODINIT_FUNC PyInit_pyexc() { return PyModule_Create(&pyexc_module); }
