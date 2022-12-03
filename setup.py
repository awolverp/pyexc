from setuptools import setup, Extension

import os.path

PLACE = os.path.dirname(os.path.abspath(__file__))

with open("README.md", "r") as f:
    data = f.read()

setup(
    name="pyexc",
    version="1.0.0a0",
    description="PyExc - Python C Extension to Manage Exceptions ...",
    long_description=data,
    url="https://github.com/awolverp/pyexc/",
    long_description_content_type = "text/markdown",
    project_urls = {
        "Github": "https://github.com/awolverp/pyexc/",
        "Bug Tracker": "https://github.com/awolverp/pyexc/issues/new",
    },
    classifiers = [
        "Natural Language :: English",
        "Natural Language :: Persian",
        "Programming Language :: C",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: Implementation :: CPython",
        "License :: OSI Approved :: GNU General Public License v3 (GPLv3)"
    ],
    data_files=[("", [PLACE+"/pyexc/pyexc.pyi"])],
    ext_modules=[
        Extension(
            "pyexc",
            sources=[PLACE+"/pyexc/pyexc.c"],
        )
    ]
)
