# vae_thrift

vaedb and it's associated goodness.


## Prerequisites

 - thrift
 - pcre
 - zeromq
 - libs3


### Install Prerequisites using a Mac

    brew install pcre 
    brew install zeromq
    brew install thrift


#### Install libs3:

    git clone https://github.com/bji/libs3.git
    cd libs3
    mv GNUmakefile.osx GNUmakefile
    sudo make install


### Install Prerequisites using Linux

Who the hell knows?  Someone please fill me in!



### Rebuilding the Thrift Defintion File

Whenever you need to rebuild the thrift definition file, simply run:

    ./vae.thrift

When you're done, you need to patch a few files.  Run:

    patch gen-cpp/vae_types.h < cpp/vae_types.h.patch

... except now this patch doesn't work.  Read the patch and see what it
basically does and then manually apply a similar change and that will
work for now.
