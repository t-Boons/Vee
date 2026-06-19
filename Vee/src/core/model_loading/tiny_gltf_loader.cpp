#pragma once
#include "core/common.hpp"

VEE_WARNING_PUSH(0)
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_USE_CPP14
#include "tinygltf/tiny_gltf.h"
VEE_WARNING_POP