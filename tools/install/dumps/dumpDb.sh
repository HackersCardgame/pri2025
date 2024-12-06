#!/bin/bash

if [ "$1" == "" ]
then
  sudo mysql pci -e "show tables;"

  echo "usage:   dumpDb.sh <tablename>"
  echo "example: dumpDb.sh config2"
else
  sudo mysqldump pci $1 >${1}-dump.sql
fi

