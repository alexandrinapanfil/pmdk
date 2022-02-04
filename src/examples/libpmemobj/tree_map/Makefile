# SPDX-License-Identifier: BSD-3-Clause
# Copyright 2015-2017, Intel Corporation

#
# examples/libpmemobj/tree_map/Makefile -- build the tree map example
#
LIBRARIES = ctree_map btree_map rtree_map rbtree_map

LIBS = -lpmemobj

ifeq ($(SPP_OFF),1)
include ../../Makefile.inc
else
include ../../Makefile_spp.inc
endif

libctree_map.o: ctree_map.o
libbtree_map.o: btree_map.o
librtree_map.o: rtree_map.o
librbtree_map.o: rbtree_map.o
