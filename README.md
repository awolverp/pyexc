# Pyexc
Simple python exception managing written in C as python extension.

> version: 1.0.0 - alpha

> **ThreadSafe**

> License: *GNU GPLv3*

```python
try:
    # ...
except Exception as e:
    pyexc.set(e)

# ...
pyexc.raise_exc() # or pyexc.print_exc() or ...
```

## Installation
> requirements: **setuptools**

**clone** project from here and install by *setup.py* file:
```bash
git clone https://github.com/awolverp/pyexc && cd pyexc && python3 setup.py install
```
Or `python3 setup.py build` if you want just build it.

## API
**Pyexc** includes 7 functions:
- includes:
    - [data](#def-data---tuplebaseexception--none-any)
    - [clear](#def-clear---bool)
    - [occurred](#def-occurred---bool)
    - [set](#def-settype-baseexception--typebaseexception-args-str---block-bool-----bool)
    - [raise_exc](#def-raise_excdefault-baseexception--typebaseexception-----noreturn)
    - [print_exc](#def-print_excexception-baseexception--typebaseexception-----none)
    - [exc_info](#exc_info----tuplebaseexceptiontypebaseexceptionnone-anynone-tracebacktypenone)

#### def data() -> Tuple[BaseException | None, Any]
Returns setted exception/args as tuple.
```python
pyexc.set(TypeError, "Hi")
pyexc.data() # (<class 'TypeError'>, "Hi")
```

#### def clear() -> bool
Clears setted exception and args. \
Returns True if ok.

#### def occurred() -> bool
Returns True if an exception setted.

#### def set(type: BaseException | Type[BaseException], args: str = ..., block: bool = ...) -> bool
Set exception.

- Parameters:
    - type [`BaseException | Type[BaseException]`] Exception type.
    - args [`str`] Exception argument.
    - block [`bool`] If True and an exception setted, break and returns False. (default False)

Returns True if setted.

#### def raise_exc(default: BaseException | Type[BaseException] = ...) -> NoReturn
Raise setted exception.

- Parameters:
    - default [`BaseException | Type[BaseException]`] Default exception if not exception setted. (default SystemError)

#### def print_exc(exception: BaseException | Type[BaseException] = ...) -> None
Print exception in stderr (with traceback).

- Parameters:
    - exception [`BaseException`] an raised exception to print.

```python
pyexc.set(TypeError)
pyexc.print_exc()
# or
try:
    raise TypeError()
except Exception as e:
    pyexc.print_exc(e)
```

#### exc_info: () -> Tuple[BaseException|Type[BaseException]|None, Any|None, TracebackType|None]
Returns pack of setted exception, exception args or setted data, and exception traceback.

## Example
```python
import pyexc

def handler():
    try:
        ...
    except Exception as e:
        pyexc.set(e)

handler()
if pyexc.occurred():
    pyexc.print_exc()
    pyexc.clear()
```

### TODO
- Add callback
- Add call
