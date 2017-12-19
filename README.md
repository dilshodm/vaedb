# VaeDB

In-memory database for querying data stored by the Vae CMS application.

This updated version replaces Thrift with a vanilla JSON-over-HTTP
server implementation.

You should build and configure VaeDB before you attempt to
configure Vae Remote on your development machine.


## License

Copyright (c) 2009-2018 Action Verb, LLC.

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

 - served
 - pcre
 - ragel
 - re2
 - zeromq
 - libs3
 - libmemcached
 - libjemalloc
 - mysql-connector-c++ (library)


### Install Prerequisites using a Mac:

    brew install re2 ragel pcre zeromq libmemcached mysql-connector-c++ jemalloc

    wget https://github.com/bji/libs3/archive/bb96e59583266a7abc9be7fc5d4d4f0e9c1167cb.zip
    unzip bb96e59583266a7abc9be7fc5d4d4f0e9c1167cb.zip
    cd libs3-bb96e59583266a7abc9be7fc5d4d4f0e9c1167cb
    mv GNUMakefile.osx Makefile
    DESTDIR=/usr/local make install

    git clone git@github.com:datasift/served.git
    mkdir served.build && cd served.build
    cmake ../served && make install


### Install Prerequisites on Linux:

    apt install automake bison flex g++ git libboost-all-dev libevent-dev libssl-dev libtool make pkg-config
    apt install libzmq3-dev libpcre3-dev libmemcached-dev libmysqlcppconn-dev libjemalloc-dev libcurl4-openssl-dev libxml2-dev libre2-dev ragel

    wget https://github.com/bji/libs3/archive/bb96e59583266a7abc9be7fc5d4d4f0e9c1167cb.zip
    unzip bb96e59583266a7abc9be7fc5d4d4f0e9c1167cb.zip
    cd libs3-bb96e59583266a7abc9be7fc5d4d4f0e9c1167cb
    make install

    git clone git@github.com:datasift/served.git
    mkdir served.build && cd served.build
    cmake ../served && make install


### Create Local MySQL Database for Vae Remote

Create a local mysql database called vaedb.  Then create a user
called vaedb and give that user a password.

Import the schema as follows:

    mysql -uvaedb < db/schema.sql


## Compiling

    ./configure
    make


### To test run:

    env MYSQL_USERNAME=<user> MYSQL_PASSWORD=<pass> MYSQL_DATABASE=<db> ./vaedb --test


If all that stuff works, you should be golden and the test suite for Vae
Remote should be ready to run.


## Test Suite

This project is tested entirely using the test suite in Vae Remote.  It
is very easy to add more tasks to that test suite and we likely do not
need one here.
