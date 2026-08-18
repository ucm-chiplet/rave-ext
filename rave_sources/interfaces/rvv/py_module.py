# setup.py
from setuptools import setup, Extension
import os
import sys

current_dir = os.path.dirname(__file__)

# Get sysroot from environment or use default
sysroot = os.environ.get('SYSROOT', './build/sysroot')

# Force use of sysroot Python headers
python_include = os.path.join(sysroot, 'usr/include/python3.10')
python_platform_include = os.path.join(sysroot, 'usr/include/riscv64-linux-gnu/python3.10')

# Only use sysroot paths, not host paths
include_dirs = [
    python_include,
    python_platform_include,
    os.path.join(current_dir, '.'),
]

# Set compiler and linker flags
extra_compile_args = [
    '-O3',
    '--sysroot=' + sysroot,
    '-I' + python_include,
    '-I' + python_platform_include,
]

rave_user_events_module = Extension(
    'rave_user_events',
    sources=[os.path.join(os.path.dirname(current_dir), './common/rave_user_events_wrapper.c')],
    include_dirs=include_dirs,
    extra_compile_args=extra_compile_args,
    extra_link_args=['--sysroot=' + sysroot],
)

setup(
    name='rave_user_events',
    version='1.0',
    description='Python interface for the rave_user_events C library',
    ext_modules=[rave_user_events_module]
)
