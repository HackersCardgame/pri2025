#!/bin/bash

scl enable devtoolset-8 bash <<EOF

  pwd

  cd /builds/phase/pri42

  ln -s /usr/bin/mysql_config /usr/bin/mariadb_config

  make

EOF

