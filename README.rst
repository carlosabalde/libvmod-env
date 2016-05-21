
.. image:: https://travis-ci.org/carlosabalde/libvmod-env.svg?branch=4.1
   :alt: Travis CI badge
   :target: https://travis-ci.org/carlosabalde/libvmod-env/

VMOD useful to access to values of environment variables from VCL.

Looking for official support for this VMOD? Please, contact `Allenta Consulting <https://www.allenta.com>`_, a `Varnish Software Premium partner <https://www.varnish-software.com/partner/allenta-consulting>`_.

SYNOPSIS
========

import env;

::

    Function BOOL is_set(STRING name)
    Function STRING get(STRING name)

INSTALLATION
============

The source tree is based on autotools to configure the building, and does also have the necessary bits in place to do functional unit tests using the varnishtest tool.

COPYRIGHT
=========

See LICENSE for details.

BSD's implementation of the red–black tree and the splay tree data structures by Niels Provos has been borrowed from the `Varnish Cache project <https://github.com/varnishcache/varnish-cache>`_:

* https://github.com/varnishcache/varnish-cache/blob/master/include/vtree.h

Copyright (c) 2016 Carlos Abalde <carlos.abalde@gmail.com>
