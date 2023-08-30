#!/usr/bin/env python
# SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import os
import sys

def get_install_requires():
    with open(os.path.realpath('requirements.txt')) as f:
        required = f.read().splitlines()
        return required


try:
    from setuptools import find_packages, setup
except ImportError:
    print(
        "Package setuptools is missing from your Python installation. "
        "Please see the installation section in the esp-matter-mfg-tool "
        "documentation for instructions on how to install it."
    )
    exit(1)

VERSION = "1.0.4"

long_description = """
====================
esp-matter-mfg-tool
====================
The python utility helps to generate the matter manufacturing partitions.

Source code for `esp-matter-mfg-tool` is `hosted on
github <https://github.com/espressif/esp-matter/
tree/main/tools/mfg_tool>`_.

Documentation
-------------
Visit online `esp-matter-mfg-tool
documentation <https://github.com/espressif/
esp-matter/tree/main/tools/mfg_tool>`_ \
or run ``mfg_tool.py -h``.

License
-------
The License for the project can be found
`here <https://github.com/espressif/esp-matter/
tree/main/tools/LICENSE>`_
"""

setup(
    name="esp-matter-mfg-tool",
    version=VERSION,
    description="A python utility which helps to generate"
                "matter manufacturing partitions",
    long_description=long_description,
    long_description_content_type='text/x-rst',
    url="https://github.com/espressif/"
        "esp-matter/tree/main/tools/mfg_tool",
    project_urls={
        "Documentation": "https://github.com/espressif/"
                         "esp-matter/tree/main/tools/mfg_tool/README.md",
        "Source": "https://github.com/espressif/esp-matter/"
                  "tree/main/tools/mfg_tool",
    },
    author="Espressif Systems",
    author_email="",
    license="Apache-2.0",
    classifiers=[
        "License :: OSI Approved :: Apache Software License",
        "Intended Audience :: Developers",
        "Natural Language :: English",
        "Operating System :: POSIX",
        "Operating System :: Microsoft :: Windows",
        "Operating System :: MacOS :: MacOS X",
        "Topic :: Software Development :: Embedded Systems",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
    ],
    python_requires=">=3.9",
    setup_requires=(["wheel"] if "bdist_wheel" in sys.argv else []),
    install_requires=get_install_requires(),
    include_package_data=True,
    packages=find_packages(),
    scripts=["esp-matter-mfg-tool.py"],
)
