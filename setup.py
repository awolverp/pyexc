from setuptools import (setup, Extension)
# import os.path

with open("README.md", "r") as f:
    long_description = f.read()

ext = Extension(
    "_pyexc", sources=["_pyexc.cpp"], optional=True
)

setup(
    name="pyexc",
    version="1.1.0",
    
    description="Python exception manager written in C++.",
    long_description=long_description,
    long_description_content_type = "text/markdown",
    
    author="awolverp",
    author_email="awolverp@gmail.com",
    
    url="https://github.com/awolverp/pyexc/",
    
    ext_modules=[
        ext    
    ],

    py_modules=[
        "pyexc.py"
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
