#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := Env

include $(IDF_PATH)/make/project.mk

update:
<<<<<<< HEAD
	git submodule update --init --remote --merge
=======
	git submodule update --init --recursive
>>>>>>> 798b40983ecb2ece286abb99176d9c31db7ffd7a
	git commit -a -m "Library update"
