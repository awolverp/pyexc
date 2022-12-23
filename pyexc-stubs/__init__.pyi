import typing

def occurred(state: int = ...) -> bool:
    """
    If an exception is occurred, returns `True`, otherwise `False`.
    """
    ...

def clear(state: int = ...) -> bool:
    """
    If an exception is occurred and successfully erased, Returns `True`, otherwise `False`.
    """
    ...

def clearAll() -> bool:
    """
    If any exception is occurred and successfully erased, returns `True`, otherwise `False`.
    """
    ...

def getExc(state: int = ...) -> typing.Union[BaseException, typing.Type[BaseException], None]:
    """
    Returns the exception which is occurred in `state` scope.

    If any exception not occurred in `state` scope, returns `None`.
    """
    ...

def setExc(exc: typing.Union[BaseException, typing.Type[BaseException]], state: int = ..., block: bool = ...) -> bool:
    """
    Set an exception in `state` scope.

    Parameters:
        exc (`BaseException | Type[BaseException]`):
            An instance of `BaseException`.
        
        state (`int`):
            scope.
        
        block (`bool`):
            If True and already any exception have occurred in `state` scope, returns `False`.
    
    Returns:
        `True` if setted, otherwise `False`.
    """
    ...

def raiseExc(state: int = ..., clear: bool = ...) -> typing.NoReturn:
    """
    Raise the exception which is occurred in `state` scope. If any exception not occurred in `state` scope, will raise `SystemError`.

    Parameters:
        state (`int`):
            scope.
        
        clear (`bool`):
            will erase the exception after raise - default `True`.
    """
    ...

def printExc(state: int = ..., clear: bool = ...) -> bool:
    """
    Print the exception which is occurred in `state` scope.

    Parameters:
        state (`int`):
            scope.
        
        clear (`bool`):
            will erase the exception after print - default True.
    """
    ...

def setCallback(callback: typing.Callable[[int, typing.Union[BaseException, typing.Type[BaseException]]], None]) -> None:
    """
    Set callback function.
    The callback function will call after each use of `setExc`.

    Callback Function Arguments:
        state (`int`):
            scope.
        
        exc (`BaseException | Type[BaseException]`):
            An instance of BaseException.
    """
    ...

def lenStates() -> int:
    """
    Returns `len(states)`.
    """
    ...

def maxState() -> int:
    """
    Returns the biggest number of states.
    """
    ...

def states() -> typing.List[int]:
    """
    Returns states.
    """
    ...

def version() -> typing.Tuple[int, int, int]:
    """
    Returns PyExc version as tuple.
    """
    ...

def __sizeof__() -> int:
    """
    Returns allocated memory size in `Bytes`.
    """
    ...
