# Pyexc
Simple python exception managing written in C as python extension.

> version: 1.0.0 - alpha

> **ThreadSafe**

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

Use **PIP** to install:
```bash
$ pip3 install -U pyexc
```

Or **clone** project from here and install by *setup.py* file:
```bash
python3 setup.py install
```
Or `python3 setup.py build` if you want just build it.

## API
> **Pyexc** includes 7 functions:
