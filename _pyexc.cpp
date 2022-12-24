/*
PyExc - Python exception manager - 1.0.0 (by awolverp)
*/

#include <Python.h>
#include <unordered_map>

/*
- State
*/
typedef struct
{
    std::unordered_map<int, PyObject *>* exceptions;
    int last_state;

    // Callback
    PyObject* callback;
} PyExcState;

#define getstate(o) ((PyExcState *)PyModule_GetState(o))

/*
- Internal Functions
*/

static inline int _internal_pyexc_occurred(PyExcState *state, int state_n) { return state->exceptions->count(state_n); }
static inline int _internal_pyexc_clear(PyExcState *state, int state_n) {
    try {
        PyObject* value = state->exceptions->at(state_n);
        Py_XDECREF(value);
        return state->exceptions->erase(state_n);
    } catch (...) {
        return 0;
    }
}
static inline int _internal_pyexc_clear_all(PyExcState *state) {
    if (!state->exceptions->size()) {
        return 0;
    }

    for (auto iter = state->exceptions->cbegin(); iter != state->exceptions->cend(); iter++)
    {
        Py_XDECREF(iter->second);
    }

    state->exceptions->clear();
    return 1;
}
// Don't forget to use Py_XINCREF ...
static inline PyObject* _internal_pyexc_get_exc(PyExcState* state, int state_n) {
    try { return state->exceptions->at(state_n); } catch (...) { return Py_None; }
}
static inline int _internal_pyexc_set_exc(PyExcState* state, int state_n, PyObject* exc, int block) {
    Py_INCREF(exc);
    if (!PyObject_IsInstance(exc, (PyObject *)Py_TYPE(PyExc_BaseException)) && !PyObject_IsInstance(exc, PyExc_BaseException))
    {
        Py_XDECREF(exc);
        return -1;
    }
    
    if (_internal_pyexc_occurred(state, state_n)) {
        if (block) {
            Py_XDECREF(exc);
            return 0;
        }

        _internal_pyexc_clear(state, state_n);
    }

    state->exceptions->emplace(std::pair<int, PyObject*>(state_n, exc));
    state->last_state = state_n;
    return 1;
}
static inline void _internal_pyexc_raise_exc(PyExcState* state, int state_n, int clear)
{
    int occurred = 0;

    Py_BEGIN_ALLOW_THREADS
        occurred = _internal_pyexc_occurred(state, state_n);
    Py_END_ALLOW_THREADS

    if (!occurred)
    {
        PyErr_SetNone(PyExc_SystemError);
        return;
    }

    PyObject* exc = state->exceptions->at(state_n);

    if (PyObject_IsInstance(exc, (PyObject *)Py_TYPE(PyExc_BaseException)))
    {
        PyErr_SetNone(exc);
    }
    else if (PyObject_IsInstance(exc, PyExc_BaseException))
    {
        PyTypeObject *type = Py_TYPE(exc);
        PyBaseExceptionObject *value = (PyBaseExceptionObject *)exc;
        PyErr_SetObject((PyObject *)type, (PyObject *)value);
    }

    if (clear) {
        Py_BEGIN_ALLOW_THREADS
            _internal_pyexc_clear(state, state_n);
        Py_END_ALLOW_THREADS   
    }
}
static inline int _internal_pyexc_print_exc(PyExcState* state, int state_n, int clear)
{
    int occurred = 0;

    Py_BEGIN_ALLOW_THREADS
        occurred = _internal_pyexc_occurred(state, state_n);
    Py_END_ALLOW_THREADS

    if (!occurred)
    {
        return 0;
    }

    PyObject* exc = state->exceptions->at(state_n);

    if (PyObject_IsInstance(exc, (PyObject *)Py_TYPE(PyExc_BaseException)))
    {
        PyErr_SetNone(exc);
    }
    else if (PyObject_IsInstance(exc, PyExc_BaseException))
    {
        PyTypeObject *type = Py_TYPE(exc);
        PyBaseExceptionObject *value = (PyBaseExceptionObject *)exc;
        PyErr_SetObject((PyObject *)type, (PyObject *)value);
    }

    PyErr_Print();

    if (clear) {
        Py_BEGIN_ALLOW_THREADS
            _internal_pyexc_clear(state, state_n);
        Py_END_ALLOW_THREADS   
    }

    return 1;
}
static inline int _internal_pyexc_set_callback(PyExcState* state, PyObject* callback)
{
    if (callback == NULL)
        return 0;
    
    if (callback == Py_None)
    {
        Py_BEGIN_ALLOW_THREADS
            if (state->callback != NULL)
            {
                Py_DECREF(state->callback);
                state->callback = NULL;
            }
        Py_END_ALLOW_THREADS

        return 1;
    }
    
    if (!PyCallable_Check(callback))
        return 0;
    
    Py_BEGIN_ALLOW_THREADS
        if (state->callback != NULL)
        {
            Py_DECREF(state->callback);
            state->callback = NULL;
        }

        Py_INCREF(callback);
        state->callback = callback;
    Py_END_ALLOW_THREADS

    return 1;
}
static inline int _internal_pyexc_call_callback(PyExcState* state, int state_n, PyObject* exc)
{
    PyObject* callback = NULL;

    Py_BEGIN_ALLOW_THREADS
        callback = state->callback;
    Py_END_ALLOW_THREADS

    if (callback == NULL)
        return 1;
    
    PyObject* args = Py_BuildValue("(i,O)", state_n, exc);
    PyObject* result = PyObject_CallObject(callback, args);

    Py_DECREF(args);

    if (result == NULL)
        return 0;
    
    Py_DECREF(result);
    return 1;
}

/*
- Functions
*/

/**
 * Check is exception occurred on @c state or not.
 * 
 * @param state (int) state number
 * 
 * @return Returns True if exception occurred, otherwise False.
 */
static PyObject *Pyexc_Occurred(PyObject *self, PyObject *args, PyObject *kwds)
{
    static char *keywords[] = {(char *)"state", NULL};

    int state_n = 0, result = 0;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|i", keywords, &state_n))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
        PyExcState *state = getstate(self);
        result = _internal_pyexc_occurred(state, state_n);
    Py_END_ALLOW_THREADS

    return PyBool_FromLong((long)result);
}

/**
 * Clear exception which is occurred on @c state.
 * 
 * @param state (int) state number
 * 
 * @return Returns True if exception occurred and cleared, otherwise False.
 */
static PyObject *Pyexc_Clear(PyObject *self, PyObject *args, PyObject *kwds)
{
    static char *keywords[] = {(char *)"state", NULL};

    int state_n = 0, result = 0;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|i", keywords, &state_n))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
        PyExcState *state = getstate(self);
        result = _internal_pyexc_clear(state, state_n);
    Py_END_ALLOW_THREADS

    return PyBool_FromLong((long)result);
}

/**
 * Clear all occurred exceptions.
 * 
 * @return Returns True if any exception occurred and cleared, otherwise False.
 */
static PyObject *Pyexc_ClearAll(PyObject *self, PyObject *__unused_args)
{
    int result = 0;

    Py_BEGIN_ALLOW_THREADS
        PyExcState *state = getstate(self);
        result = _internal_pyexc_clear_all(state);
    Py_END_ALLOW_THREADS

    return PyBool_FromLong((long)result);
}

/**
 * @param state (int) state number
 * 
 * @return Returns exception if exception occurred on @c state, otherwise None.
 */
static PyObject *Pyexc_GetExc(PyObject *self, PyObject *args, PyObject *kwds)
{
    static char *keywords[] = {(char *)"state", NULL};

    int state_n = 0;
    PyObject* result = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|i", keywords, &state_n))
        return NULL;

    Py_BEGIN_ALLOW_THREADS
        PyExcState *state = getstate(self);
        result = _internal_pyexc_get_exc(state, state_n);
    Py_END_ALLOW_THREADS

    Py_XINCREF(result);
    return result;
}

/**
 * @brief Set exception.
 *
 * @param exc (PyObject*) Exception type.
 * @param state (int) Exception argument.
 * @param block (bool) If True and an exception setted, break and returns False. (default False)
 *
 * @returns Returns True if setted.
 */
static PyObject *Pyexc_SetExc(PyObject *self, PyObject *args, PyObject *kwds)
{
    static char *keywords[] = {(char *)"exc", (char *)"state", (char *)"block", NULL};
    PyObject *exc = NULL;
    int state_n = 0, block = 0;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O|ip", keywords, &exc, &state_n, &block))
        return NULL;

    int result = 0;
    PyExcState *state = NULL;

    Py_BEGIN_ALLOW_THREADS
        state = getstate(self);
        result = _internal_pyexc_set_exc(state, state_n, exc, block);
    Py_END_ALLOW_THREADS

    if (result == -1) {
        PyErr_SetString(PyExc_TypeError, "a BaseException is required for `exc` argument");
        return NULL;
    }

    if (result == 1)
    {
        // call callback
        int is_fail = _internal_pyexc_call_callback(state, state_n, exc);

        if (!is_fail)
            return NULL;
    }

    return PyBool_FromLong((long)result);
}

/**
 * @brief Raise occurred exception.
 *
 * @param state (int) state number
 * @param clear (bool) clear state after raise (default True)
 * 
 * @throw If exception occurred, Raise that, otherwise raise SystemError.
 */
static PyObject *Pyexc_RaiseExc(PyObject *self, PyObject *args, PyObject *kwds)
{
    static char *keywords[] = {(char *)"state", (char *)"clear", NULL};
    int state_n = 0, clear = 1;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|ip", keywords, &state_n, &clear))
        return NULL;
    
    PyExcState *state = getstate(self);

    _internal_pyexc_raise_exc(state, state_n, clear);

    return NULL;
}

/**
 * @brief Print occurred exception.
 *
 * @param state (int) state number
 * @param clear (bool) clear state after print (default True)
 * 
 * @returns If exception occurred, Print that and return True, otherwise return False.
 */
static PyObject *Pyexc_PrintExc(PyObject *self, PyObject *args, PyObject *kwds)
{
    static char *keywords[] = {(char *)"state", (char *)"clear", NULL};
    int state_n = 0, clear = 1;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|ip", keywords, &state_n, &clear))
        return NULL;
    
    PyExcState *state = getstate(self);

    return PyBool_FromLong((long)_internal_pyexc_print_exc(state, state_n, clear));
}

/**
 * @brief Set callback for setExc function.
 *
 * @param callback (PyObject*) callable or None object
 *
 * @return None
 */
static PyObject *Pyexc_SetCallback(PyObject *self, PyObject *args, PyObject *kwds)
{
    static char *keywords[] = {(char *)"callback", NULL};
    PyObject* callback = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O", keywords, &callback))
        return NULL;
    
    PyExcState* state = getstate(self);

    int result = _internal_pyexc_set_callback(state, callback);

    if (result == 0)
    {
        PyErr_SetString(PyExc_TypeError, "a callable or None object is required");
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *Pyexc_LenStates(PyObject *self, PyObject *__unused_args)
{
    size_t result = 0;

    Py_BEGIN_ALLOW_THREADS
        PyExcState *state = getstate(self);
        result = state->exceptions->size();
    Py_END_ALLOW_THREADS

    return PyLong_FromSize_t(result);
}

static PyObject *Pyexc_MaxState(PyObject *self, PyObject *__unused_args)
{
    int result = 0;

    Py_BEGIN_ALLOW_THREADS
        PyExcState *state = getstate(self);
        
        for (auto iter = state->exceptions->cbegin(); iter != state->exceptions->cend(); iter++)
        {
            int i = iter->first;
            if (i > result)
                result = i;
        }
    Py_END_ALLOW_THREADS

    return PyLong_FromLong((long)result);
}

static PyObject *Pyexc_States(PyObject *self, PyObject *__unused_args)
{
    PyObject *result = NULL;
    PyExcState *state = getstate(self);

    Py_BEGIN_ALLOW_THREADS
        result = PyList_New(state->exceptions->size());
    Py_END_ALLOW_THREADS

    if (result == NULL)
        return NULL;

    Py_BEGIN_ALLOW_THREADS
        int count = 0;
        for (auto iter = state->exceptions->cbegin(); iter != state->exceptions->cend(); iter++)
        {
            PyObject* rev = Py_BuildValue("i", iter->first);
            PyList_SetItem(result, count, rev);
            count++;
        }
    Py_END_ALLOW_THREADS

    return result;
}

static PyObject *Pyexc_Version(PyObject *self, PyObject *__unused_args) { return Py_BuildValue("(i,i,i)", 1, 1, 0); }

static PyObject *Pyexc___sizeof__(PyObject *self, PyObject *__unused_args)
{
    PyExcState *state = getstate(self);
    size_t size = state->exceptions->size();

    return PyLong_FromSize_t(
        (size_t)(size * sizeof(PyObject*)) + (size * sizeof(int))
    );
}

/*
- Module
*/

static void __pyexc_Free(PyObject *self)
{
    PyExcState *state = getstate(self);

    if (state->callback != NULL)
        Py_DECREF(state->callback);

    state->callback = NULL;

    for (auto iter = state->exceptions->cbegin(); iter != state->exceptions->cend(); iter++)
    {
        Py_XDECREF(iter->second);
    }

    delete state->exceptions;
}

static PyMethodDef pyexc_methods[] = {
    {
        "occurred", (PyCFunction)Pyexc_Occurred, METH_VARARGS | METH_KEYWORDS,
        "If an exception is occurred, returns `True`, otherwise `False`."
    },
    {
        "clear", (PyCFunction)Pyexc_Clear, METH_VARARGS | METH_KEYWORDS,
        "If an exception is occurred and successfully erased, returns `True`, otherwise `False`."
    },
    {
        "clearAll", (PyCFunction)Pyexc_ClearAll, METH_NOARGS,
        "If any exception is occurred and successfully erased, returns `True`, otherwise `False`."
    },
    {
        "getExc", (PyCFunction)Pyexc_GetExc, METH_VARARGS | METH_KEYWORDS,
        "Returns the exception which is occurred in `state` scope.\n\nIf any exception not occurred in `state` scope, returns `None`."
    },
    {
        "setExc", (PyCFunction)Pyexc_SetExc, METH_VARARGS | METH_KEYWORDS,
        "Set an exception in `state` scope.\n\n" \
        "Parameters:\n" \
        "   exc (`BaseException | Type[BaseException]`):\n" \
        "       An instance of `BaseException`.\n" \
        "   state (`int`):\n" \
        "       scope.\n" \
        "   block (`bool`):\n" \
        "       If True and already any exception have occurred in `state` scope, returns `False`.\n\n" \
        "Returns:\n" \
        "`True` if setted, otherwise `False`."
    },
    {
        "raiseExc", (PyCFunction)Pyexc_RaiseExc, METH_VARARGS | METH_KEYWORDS, 
        "Raise the exception which is occurred in `state` scope." \
        "If any exception not occurred in `state` scope, will raise `SystemError`.\n\n" \
        "Parameters:\n" \
        "   state (`int`):\n" \
        "       scope.\n" \
        "   clear (`bool`):\n" \
        "       will erase the exception after raise - default `True`."
    },
    {
        "printExc", (PyCFunction)Pyexc_PrintExc, METH_VARARGS | METH_KEYWORDS,
        "Print the exception which is occurred in `state` scope.\n\n" \
        "Parameters:\n" \
        "   state (`int`):\n" \
        "       scope.\n" \
        "   clear (`bool`):\n" \
        "       will erase the exception after print - default True."
    },
    {
        "setCallback", (PyCFunction)Pyexc_SetCallback, METH_VARARGS | METH_KEYWORDS,
        "Set callback function. The callback function will call after each use of `setExc`.\n\n" \
        "Callback Function Arguments:\n" \
        "   state (`int`):\n" \
        "       scope.\n" \
        "   exc (`BaseException | Type[BaseException]`):\n" \
        "       An instance of BaseException."
    },
    
    {
        "lenStates", (PyCFunction)Pyexc_LenStates, METH_NOARGS,
        "Returns `len(states)`."
    },
    {
        "maxState", (PyCFunction)Pyexc_MaxState, METH_NOARGS,
        "Returns the biggest number of states."
    },
    {"states", (PyCFunction)Pyexc_States, METH_NOARGS, "Returns states."},

    {"version", (PyCFunction)Pyexc_Version, METH_NOARGS, "Returns PyExc version as tuple."},
    {"__sizeof__", (PyCFunction)Pyexc___sizeof__, METH_NOARGS, "Returns allocated memory size in `Bytes`."},
    {NULL, NULL, 0, NULL},
};

struct PyModuleDef pyexc_module = {
    PyModuleDef_HEAD_INIT,
    .m_name = "pyexc",
    .m_doc = "Python exception managing ...",
    .m_size = sizeof(PyExcState),
    .m_methods = pyexc_methods,
    .m_free = (freefunc)__pyexc_Free,
};

PyMODINIT_FUNC PyInit_pyexc() {
    PyObject* m = PyModule_Create(&pyexc_module);
    if (m == NULL)
        return NULL;
    
    PyExcState* state = getstate(m);
    state->exceptions = new std::unordered_map<int, PyObject*>;

    return m;
}
