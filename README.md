# vae_thrift

vaedb and it's associated goodness.


## Prerequisites

vae_thrift requires thrift version 0.2.0.  You should install this
manually on your development system.

If you're using a Mac, you should also:

    brew install pcre


### Rebuilding the Thrift Defintion File

Whenever you need to rebuild the thrift definition file, simply run:

    ./vae.thrift

When you're done, you need to patch a few files.  Run:

    patch gen-cpp/vae_types.h < cpp/vae_types.h.patch
