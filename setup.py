from setuptools import (setup, Extension)
import os.path

BASE = os.path.dirname(os.path.abspath(__file__))

join_path = lambda x: os.path.join(BASE, *x)

with open(join_path(["README.md"]), "r") as f:
    long_description = f.read()

STUBS = join_path(["pyexc-stubs"])

setup(
    name="pyexc",
    version="1.0.0",
    description="Python exception manager written in C++.",
    long_description=long_description,
    long_description_content_type = "text/markdown",
    author="awolverp",
    author_email="awolverp@gmail.com",
    url="https://github.com/awolverp/pyexc",
    include_package_data=True,
    packages=[STUBS],
    package_data={STUBS: ["__init__.pyi"]},
    ext_modules=[
        Extension(
            "pyexc",
            sources=[join_path(["src", "pyexc.cpp"])],
            language="c++"
        )
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
