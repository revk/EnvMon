#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := Env

include $(IDF_PATH)/make/project.mk

update:
	git submodule update --init --remote --merge
	git commit -a -m "Library update"
