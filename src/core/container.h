#pragma once

#include <core/proc.h>

struct container {};

typedef struct container container;

extern struct container *root_container;

void spawn_init_container();

int alloc_resource();

void trace_usage();
