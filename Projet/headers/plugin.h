#ifndef PLUGIN
#define PLUGIN

// Headers used throughout the project from GCC plugin interface.
#pragma once

// Most import needed to write a plugin.
#include <gcc-plugin.h>
#include <plugin-version.h>

// Basic blocks are one of the fundamental compilation type.
#include <basic-block.h>

// Tree manipulation and pass-data.
#include <tree.h>
#include <tree-pass.h>

// Statement manipulation.
#include <gimple.h>
#include <gimple-iterator.h>

// Global GCC context.
#include <context.h>

// Computing dominance informations.
#include <dominance.h>

// Pragma manipulations.
#include <c-family/c-pragma.h>

// Types used when writing plugins.
#include <vec.h>
#include <bitmap.h>

// Errors and warning messages.
#include <errors.h>
#include <diagnostic-core.h>

// Project
#include "mpi_collectives.h"
#include "frontiers.h"

#endif PLUGIN
