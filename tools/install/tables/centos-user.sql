################################################################################
# create the database and user for CENTOS
################################################################################

CREATE DATABASE IF NOT EXISTS pci;

USE pci;

CREATE USER IF NOT EXISTS 'pci'@'localhost' IDENTIFIED BY '123456';

GRANT ALL PRIVILEGES ON pci.* TO 'pci'@'localhost';
