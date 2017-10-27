#!/usr/bin/env python3
# encoding: utf-8

from setuptools import setup, Extension
from codecs import open
from os import path

here = path.abspath(path.dirname(__file__))

# Get the long description from the README file
with open(path.join(here, 'README.rst'), encoding='utf-8') as f:
    long_description = f.read()

_hrutils_module = Extension('_hrutils', sources = ['c/hrutils.c'])

setup(	name = 'hrutils',
		version = '0.1.0',
		
		description = 'Home Router utilities.',
		long_description = long_description,

		url = 'https://github.com/ShawnBaker/hrutils',
		
		author = 'Shawn Baker',
		author_email = 'shawn@frozen.ca',
		
		license = 'MIT',
		
		classifiers = [
			'Development Status :: 3 - Alpha',

			'Intended Audience :: Developers',
			'Topic :: Software Development :: Build Tools',

			'License :: OSI Approved :: MIT License',

			'Programming Language :: Python :: 3',
			'Programming Language :: Python :: 3.5',
		],
		
		ext_modules = [_hrutils_module],
		packages = ['hrutils'],
		
		keywords='router network utilties',
)
