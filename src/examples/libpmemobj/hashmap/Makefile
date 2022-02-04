# SPDX-License-Identifier: BSD-3-Clause
# Copyright 2015-2018, Intel Corporation

LIBRARIES = hashmap_atomic hashmap_tx hashmap_rp

LIBS = -lpmemobj

ifeq ($(SPP_OFF),1)
include ../../Makefile.inc
else
include ../../Makefile_spp.inc
endif

libhashmap_atomic.o: hashmap_atomic.o
libhashmap_tx.o: hashmap_tx.o
libhashmap_rp.o: hashmap_rp.o
