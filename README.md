# vae_thrift

vaedb and it's associated goodness.


## Prerequisites

 - thrift
 - pcre
 - zeromq
 - libs3
 - libmysqlclient
 - libmemcached


### Install Prerequisites using a Mac

    brew install pcre 
    brew install zeromq
    brew install thrift
    brew install libmemcached


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

There is no longer a patch needed after running this.  (Thanks Kevin!)
