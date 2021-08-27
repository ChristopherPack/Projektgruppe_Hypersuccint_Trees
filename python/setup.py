from setuptools import setup
from Cython.Build import cythonize

setup(ext_modules = cythonize("pht_timer.pyx"), include_dirs = ["../include"])