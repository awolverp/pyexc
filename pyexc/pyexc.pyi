import typing, types

def data() -> typing.Tuple[typing.Union[BaseException, None], typing.Any]:
    """
    Returns setted exception/args as tuple.

    Example::

        pyexc.set(TypeError, "h")
        pyexc.data()
        # (<class 'TypeError'>, "h")
    """
    ...

def clear() -> bool:
    """
    Clears setted exception and args.

    Returns:
        Returns True if ok.
    """
    ...

def set(
    type: typing.Union[BaseException, typing.Type[BaseException]],
    args: str = ..., block: bool = ...
) -> bool:
    """
    Set exception.

    Parameters:
        type [`BaseException | Type[BaseException]`] Exception type.

        args [`str`] Exception argument.
        
        block [`bool`] If True and an exception setted, break and returns False. (default False)

    Returns:
        Returns True if setted.
    """
    ...

def raise_exc(default: typing.Union[BaseException, typing.Type[BaseException]] = ...) -> typing.NoReturn:
    """
    Raise setted exception.

    Parameters:
        default [`BaseException | Type[BaseException]`] Default exception if not exception setted. (default SystemError)
    """
    ...

def occurred() -> bool:
    """
    Returns True if an exception setted.
    """
    ...

def print_exc(exception: typing.Union[BaseException, typing.Type[BaseException]] = ...) -> None:
    """
    Print exception in `stderr` (with traceback).

    Parameters:
        exception [`BaseException`] an raised exception to print.
    
    Example::

        pyexc.set(TypeError)
        pyexc.print_exc()
        # or
        try:
            raise TypeError
        except Exception as e:
            pyexc.print_exc(e)
    """
    ...

def exc_info() -> typing.Tuple[
    typing.Union[None, BaseException, typing.Type[BaseException]],
    typing.Union[None, typing.Any, typing.Tuple[typing.Any, ...]],
    typing.Union[None, types.TracebackType]
]:
    """
    Returns exception info as tuple.

    Returns:
        Returns pack of setted exception, exception args or setted data, and exception traceback.
    """
    ...
