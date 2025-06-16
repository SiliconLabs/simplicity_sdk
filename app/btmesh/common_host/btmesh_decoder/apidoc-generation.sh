#!/bin/sh

(rm -rf apidoc && mkdir -p apidoc) || exit 1
doxygen doxyfile || exit 1
