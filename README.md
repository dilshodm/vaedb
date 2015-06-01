# VaeDB and VaeRubyd

The vae_thrift repository contains two daemons that run for the purpose
of servicing Vae Remote.  In a typical Vae installation, these are run
on separate machines for both security and scaling purposes.

You should build and configure VaeDB and VaeRubyd before you attempt to
configure Vae Remote on your development machine.


## Prerequisites

 - thrift
 - pcre
 - zeromq
 - libs3
 - libmemcached
 - mysql-connector-c++ (library)


### Install Prerequisites using a Mac:

    brew install pcre 
    brew install zeromq
    brew install thrift
    brew install libmemcached
    brew install mysql-connector-c++

    git clone https://github.com/bji/libs3.git
    cd libs3
    mv GNUmakefile.osx GNUmakefile
    sudo make install


### Install Prerequisites on Linux:

    apt-get install libzmq-dev libpcre3-dev libthrift-dev libmemcached-dev libmysqlcppconn-dev

    git clone https://github.com/bji/libs3.git
    cd libs3
    sudo make install


## Compiling VaeDB

    cd cpp
    ./configure
    make


### To test run VaeDB:

    cd cpp
    ./vaedb --test


### To test run VaeRubyd:

    cd rb
    bundle install
    ruby vaerubyd.rb


If all that stuff works, you should be golden and the test suite for Vae
Remote should be ready to run.


## Rebuilding the Thrift Defintion File

This only needs to be done when the interface between vae_thrift and
vae_remote changes.  (I.e. you made changes to the file called
vae.thrift).

Whenever you need to rebuild the thrift definition file, simply run:

    ./vae.thrift

There is no longer a patch needed after running this.  (Thanks Kevin!)


## Test Suite

This project is tested entirely using the test suite in Vae Remote.  It
is very easy to add more tasks to that test suite and we likely do not
need one here.
