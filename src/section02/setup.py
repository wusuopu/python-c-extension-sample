#!/usr/bin/env python
#-*- coding:utf-8 -*-

from setuptools import setup, Extension
#import sys

#if sys.version_info.major == 3:
    #hello_world = Extension('lc_hello_world', sources=["lc_hello3.c"])
#else:
    #hello_world = Extension('lc_hello_world', sources=["lc_hello2.c"])
hello_world = Extension('lc_hello_world', sources=["lc_hello.c"])
setup(ext_modules=[hello_world])
