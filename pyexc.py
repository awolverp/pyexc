try:
    from _pyexc import (
        occurred, clear, clearAll, getExc, setExc, raiseExc, printExc, setCallback,
        call, rcall, lenStates, maxState, states, version, __sizeof__
    )
    CEXTENSION = True
except ImportError:
    CEXTENSION = False

    from threading import Lock
    import traceback
    import typing

    _mutex = Lock()
    _exceptions = {}
    _callback = None

    def occurred(state: int = 0) -> bool:
        """
        If an exception is occurred, returns `True`, otherwise `False`.
        """
        with _mutex:
            return (state in _exceptions)

    def clear(state: int = 0) -> bool:
        """
        If an exception is occurred and successfully erased, Returns `True`, otherwise `False`.
        """
        with _mutex:
            return _exceptions.pop(state, None) is not None

    def clearAll() -> bool:
        """
        If any exception is occurred and successfully erased, returns `True`, otherwise `False`.
        """
        with _mutex:
            if len(_exceptions) == 0:
                return False
            
            _exceptions.clear()
            return True

    def getExc(state: int = 0) -> typing.Union[BaseException, typing.Type[BaseException], None]:
        """
        Returns the exception which is occurred in `state` scope.

        If any exception not occurred in `state` scope, returns `None`.
        """
        with _mutex:
            return _exceptions.get(state, None)

    def _is_exception(exc):
        try:
            return isinstance(exc, BaseException) or (
                BaseException in exc.mro()
            )
        except AttributeError:
            return False

    def setExc(exc: typing.Union[BaseException, typing.Type[BaseException]], state: int = 0, block: bool = False) -> bool:
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
        if (not _is_exception(exc)):
            raise TypeError("a BaseException is required for `exc` argument")
        
        with _mutex:
            if (exc in _exceptions and block):
                return False
            
            _exceptions[state] = exc

            if (_callback):
                _callback(state, exc)

        return True

    def raiseExc(state: int = 0, clear: bool = True) -> typing.NoReturn:
        """
        Raise the exception which is occurred in `state` scope. If any exception not occurred in `state` scope, will raise `SystemError`.

        Parameters:
            state (`int`):
                scope.
            
            clear (`bool`):
                will erase the exception after raise - default `True`.
        """
        exc = None
        with _mutex:
            exc = _exceptions.get(state, None)
            
            if (exc and clear):
                _exceptions.pop(state)
        
        raise (
            exc or SystemError
        )

    def printExc(state: int = 0, clear: bool = True) -> bool:
        """
        Print the exception which is occurred in `state` scope.

        Parameters:
            state (`int`):
                scope.
            
            clear (`bool`):
                will erase the exception after print - default True.
        """
        exc: BaseException = None
        with _mutex:
            exc = _exceptions.get(state, None)
            
            if (exc and clear):
                _exceptions.pop(state)
        
        if (exc):

            try:
                raise exc
            except:
                traceback.print_exc()
            
            return True
        
        return False

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
        global _callback

        if (not callable(callback) and callback is not None):
            raise TypeError("a callable or None object is required")
        
        with _mutex:
            _callback = callback

    def call(func: typing.Callable, state: int = 0, args: tuple = (), kwargs: dict = {}) -> typing.Any:
        """
        Calls `func` with `args` and `kwargs` parameters.
        If `func` raised exception, exception will sets in `state` scope and returns `None`.

        Parameters:
            func (`Callable`):
                function.
            
            state (`int`):
                scope.
            
            args (`tuple`):
                function args.
            
            kwargs (`dict`):
                function kwargs.
        """
        if not callable(func):
            raise TypeError("func must be callable.")
        
        if not isinstance(kwargs, dict):
            raise TypeError("kwargs must be dict.")
        
        if not isinstance(args, tuple):
            raise TypeError("args must be tuple.")

        try:
            return func(*args, **kwargs)
        except Exception as exc:
            with _mutex:
                _exceptions[state] = exc

                if (_callback):
                    _callback(state, exc)

            return None
    
    def rcall(func: typing.Callable, args: tuple = (), kwargs: dict = {}) -> typing.Any:
        """
        Like `pyexc.call` but returns exception instead of set in `state` scope.
        """
        if not callable(func):
            raise TypeError("func must be callable.")
        
        if not isinstance(kwargs, dict):
            raise TypeError("kwargs must be dict.")
        
        if not isinstance(args, tuple):
            raise TypeError("args must be tuple.")

        try:
            return func(*args, **kwargs)
        except Exception as exc:
            return exc

    def lenStates() -> int:
        """
        Returns `len(states)`.
        """
        with _mutex:
            return len(_exceptions)

    def maxState() -> int:
        """
        Returns the biggest number of states.
        """
        with _mutex:
            return max(_exceptions.keys())

    def states() -> typing.List[int]:
        """
        Returns states.
        """
        with _mutex:
            return list(_exceptions.keys())

    def version() -> typing.Tuple[int, int, int]:
        """
        Returns PyExc version as tuple.
        """
        return (1, 3, 2)

    def __sizeof__() -> int:
        """
        Returns allocated memory size in `Bytes`.
        """
        with _mutex:
            return _exceptions.__sizeof__()
