#!/usr/bin/env python2
# -*- coding: utf-8 -*-
"""
Created on Wed Nov  1 18:32:50 2017

@author: jesse
"""

from distutils.core import setup
from Cython.Build import cythonize

setup(
    ext_modules = cythonize("Cy_Ks_Gillespie.pyx")
)