<h1 align=center>
    PyExc
</h1>
<p align=center>
    a Python library written in C++ - Manage exceptions.
</p>

<p align=center>
    <a href="#install">Install</a> - <a href="#usage">Usage</a> - <a href="#memory-usage">Memory Usage</a> - <a href="#speed">Speed</a>
</p>

----

> Version: 1.3.2 - **Thread-Safe** (License: GNU GPLv3)

###### Example:
```python
import pyexc

@pyexc.setCallback
def handle_exceptions(state, exc):
    pyexc.printExc(state=state)

try:
    # ...
except Exception as e:
    pyexc.setExc(e) # will call handle_exceptions
```

**NOTE** \
This repo is my practice to learn Python C Extension ...

----

## Install
**PIP**:
```bash
pip3 install -U git+https://github.com/awolverp/pyexc
```

**If you don't want to install C++ extension**, use these commands:

**Linux / MacOS**:
```bash
CEXTENSION=0 pip3 install -U git+https://github.com/awolverp/pyexc
```

**Windows**:
```bash
set CEXTENSION=0
pip3 install -U git+https://github.com/awolverp/pyexc
```

> NOTE: if you haven't c++ compiler, the extension isn't installed automatically.

## Usage

**How it works?** Its function is very simple. Suppose you have a dictionary whose key is the number named `state` and value is `Exception`.
You put any exception to this dictionary with `setExc` function and manage that by other functions.

See [example](#example) / [examples](#examples).


- **Content**:
    - [API Manual](#api-manual)
    - [Examples](#examples)

## API Manual
**Pyexc** includes 13 functions:

- [Install](#install)
- [Usage](#usage)
- [API Manual](#api-manual)
    - [occurred](#occurred)
    - [clear](#clear)
    - [clearAll](#clearall)
    - [getExc](#getexc)
    - [setExc](#setexc)
    - [raiseExc](#raiseexc)
    - [printExc](#printexc)
    - [setCallback](#setcallback)
    - [call](#call)
    - [rcall](#rcall)
    - [lenStates](#lenstates)
    - [maxState](#maxstate)
    - [states](#states)
    - [version](#version)
    - [\_\_ sizeof \_\_](#__-sizeof-__)
- [Examples](#examples)
- [Memory Usage](#memory-usage)
- [Speed](#speed)
- [TODO](#todo)

#### occurred
If an exception is occurred, returns `True`, otherwise `False`.

```python
pyexc.occurred(state: int = ...) -> bool
```

#### clear
If an exception is occurred and successfully erased, Returns `True`, otherwise `False`.

```python
pyexc.clear(state: int = ...) -> bool
```

#### clearAll
If any exception is occurred and successfully erased, returns `True`, otherwise `False`.

```python
pyexc.clearAll() -> bool
```

#### getExc
Returns the exception which is occurred in `state` scope. \
If any exception not occurred in `state` scope, returns `None`.

```python
getExc(state: int = ...) -> BaseException | Type[BaseException] | None
```

#### setExc
Set an exception in `state` scope.

- **Parameters:**
    - exc:
        An instance of `BaseException`.
    
    - state:
        scope.
    
    - block:
        If True and already any exception have occurred in `state` scope, returns `False`.

Returns: `True` if setted, otherwise `False`.

```python
setExc(exc: BaseException | Type[BaseException], state: int = ..., block: bool = ...) -> bool
```

#### raiseExc
Raise the exception which is occurred in `state` scope.
If any exception not occurred in `state` scope, will raise `SystemError`.

- **Parameters:**
    - state:
        scope.
    
    - clear:
        will erase the exception after raise - default `True`.

```python
raiseExc(state: int = ..., clear: bool = ...) -> NoReturn
```

#### printExc
Print the exception which is occurred in `state` scope.

- **Parameters:**
    - state:
        scope.
    
    - clear:
        will erase the exception after print - default `True`.

```python
printExc(state: int = ..., clear: bool = ...) -> bool
```

#### setCallback
Set callback function.
The callback function will call after each use of `setExc`.

- **Callback Function Arguments:**
    - state (`int`):
        scope.
    
    - exc (`BaseException | Type[BaseException]`):
        An instance of BaseException.

```python
setCallback(callback: (int, BaseException | Type[BaseException]) -> None) -> None:
```

#### call
Calls `func` with `args` and `kwargs` parameters.
If `func` raised exception, exception will sets in `state` scope and returns `None`.

- **Parameters**:
    - func (`Callable`):
        function.
    
    - state (`int`):
        scope.
    
    - args (`tuple`):
        function args.
    
    - kwargs (`dict[str, Any]`):
        function kwargs.

```python
call(func: (...) -> Any, state:int=0, args:tuple=(), kwargs:dict={}) -> Any
```

> **Note**: [**C EXTENSION bug**]: `pyexc.call` has a bug in c extension that doesn't keep exception values and just keeps type of it.

#### rcall
Like `pyexc.call` but returns exception instead of set in `state` scope.

```python
rcall(func: (...) -> Any, args:tuple=(), kwargs:dict={}) -> Any
```

> **Note**: [**C EXTENSION bug**]: `pyexc.rcall` has a bug in c extension that doesn't return exception values and just returns type of it.

#### lenStates
Returns `len(states)`.

```python
lenStates() -> int
```

#### maxState
Returns the biggest number of states.

```python
maxState() -> int
```

#### states
Returns states.

```python
states() -> list[int]
```

#### version
Returns **PyExc** version as tuple.

```python
version() -> tuple[int, int, int]
```

#### __ sizeof __
Returns allocated memory size in `Bytes`.

```python
__sizeof__() -> int
```

## Examples
##### exception
```python
import pyexc

def foo():
    try:
        # ... some code
    except Exception as e:
        pyexc.setExc(e, state=1)

foo()

if pyexc.occurred(state=1):
    pyexc.raiseExc(state=1)
```

##### multi exceptions
```python
import pyexc

def foo():
    for i in range(100):
        try:
            # ... some code
        except Exception as e:
            pyexc.setExc(e, state=i)

foo()

if pyexc.lenStates() != 0:
    for state in pyexc.states():
        exc = pyexc.getExc(state=state)
        # ...
```

##### multithreading: (not recommended)
```python
import threading
import time
import pyexc

def handler():
    for i in range(10):
        pyexc.setExc(TypeError(i))
        time.sleep(0.2)

def getter():
    for i in range(10):
        pyexc.printExc()
        time.sleep(0.2)

t1 = threading.Thread(target=handler)
t2 = threading.Thread(target=getter)
t1.start()
t2.start()
t1.join()
t2.join()
```

##### multithreading: (recommended)
```python
import threading
import pyexc

def getter(state, exc):
    pyexc.printExc(state=state)

pyexc.setCallback(getter)

def handler():
    for i in range(10):
        pyexc.setExc(TypeError(i))
        time.sleep(0.2)

t1 = threading.Thread(target=handler)
t1.start()
t1.join()
```

### Memory Usage
**PyExc** uses **Very Low** memory. *1/3* dict memory usage.

```python
>>> import pyexc
>>> for i in range(1000): pyexc.setExc(TypeError(), state=i)
>>> pyexc.__sizeof__()
12000 # 12000 Bytes

>>> d = dict()
>>> for i in range(1000): d[i] = TypeError()
>>> d.__sizeof__()
36944 # 36944 Bytes
```

### Speed
**PyExc** is **slower than** dict.

```python
>>> import pyexc
>>> %timeit for i in range(1000): pyexc.setExc(TypeError(), state=i)
1000 loops, best of 5: 599 µs per loop

>>> d = dict()
>>> %timeit for i in range(1000): d[i] = TypeError()
1000 loops, best of 5: 124 µs per loop
```

## TODO
- [x] Add python code
- [x] Add `call` function
- [x] Add `rcall` function
- [ ] Add `toString` function
- [ ] Add `writeTo` function
