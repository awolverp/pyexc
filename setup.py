from setuptools import setup, Extension

import os.path

PLACE = os.path.dirname(os.path.abspath(__file__))

setup(
    name="pyexc",
    version="1.0.0a0",
    description="PyExc - Python C Extension to Manage Exceptions ...",
    author="awolverp",
    data_files=[("", [PLACE+"/pyexc/pyexc.pyi"])],
    ext_modules=[
        Extension(
            "pyexc",
            sources=[PLACE+"/pyexc/pyexc.c"],
        )
    ]
)
