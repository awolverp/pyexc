from setuptools import (setup, Extension)
from os import environ

with open("README.md", "r") as f:
    long_description = f.read()

if (environ.get("CEXTENSION", "1") == "1"):
    extensions = [Extension("_pyexc", sources=["_pyexc.cpp"], optional=True)]

else:
    extensions = None

setup(
    name="pyexc",
    version="1.1.0",
    
    description="Python exception manager written in C++.",
    long_description=long_description,
    long_description_content_type = "text/markdown",
    
    author="awolverp",
    author_email="awolverp@gmail.com",
    
    url="https://github.com/awolverp/pyexc/",
    
    ext_modules=extensions,

    py_modules=[
        "pyexc"
    ],
    
    project_urls = {
        "Github": "https://github.com/awolverp/pyexc/",
        "Bug Tracker": "https://github.com/awolverp/pyexc/issues/new",
    },

    classifiers = [
        "Programming Language :: C++",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: Implementation :: CPython",
        "License :: OSI Approved :: GNU General Public License v3 (GPLv3)"
    ]
)
