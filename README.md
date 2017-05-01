# VaeDB and VaeRubyd

The vae_thrift repository contains two daemons that run for the purpose
of servicing Vae Remote.  In a typical Vae installation, these are run
on separate machines for both security and scaling purposes.

You should build and configure VaeDB and VaeRubyd before you attempt to
configure Vae Remote on your development machine.


## License

Copyright (c) 2009-2016 Action Verb, LLC.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program, in the file called COPYING-AGPL.
If not, see http://www.gnu.org/licenses/.


## Prerequisites

 - thrift
 - pcre
 - zeromq
 - libs3
 - libmemcached
 - libjemalloc
 - mysql-connector-c++ (library)


### Install Prerequisites using a Mac:

    brew install pcre zeromq thrift libmemcached mysql-connector-c++ jemalloc

    git clone https://github.com/bji/libs3.git
    cd libs3
    mv GNUmakefile.osx GNUmakefile
    DESTDIR=/usr/local make install


### Install Prerequisites on Linux:

    apt install automake bison flex g++ git libboost-all-dev libevent-dev libssl-dev libtool make pkg-config

    wget http://archive.apache.org/dist/thrift/0.9.2/thrift-0.9.2.tar.gz
    tar xf thrift-0.9.2.tar.gz
    cd thrift-0.9.2
    ./configure
    make
    make install

    apt install libzmq3-dev libpcre3-dev libmemcached-dev libmysqlcppconn-dev libjemalloc-dev libcurl4-openssl-dev libxml2-dev

    wget https://github.com/bji/libs3/archive/bb96e59583266a7abc9be7fc5d4d4f0e9c1167cb.zip
    unzip bb96e59583266a7abc9be7fc5d4d4f0e9c1167cb.zip
    cd libs3-bb96e59583266a7abc9be7fc5d4d4f0e9c1167cb
    make install


### Create Local MySQL Database for Vae Remote

Create a local mysql database called vaedb.  Then create a user
called vaedb and give that user a password.

Import the schema as follows:

    mysql -uvaedb < db/schema.sql


## Compiling VaeDB

    cd cpp
    ./configure
    make


### To test run VaeDB:

    cd cpp
    env MYSQL_USERNAME=<user> MYSQL_PASSWORD=<pass> MYSQL_DATABASE=<db> ./vaedb --test


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
