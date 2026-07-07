#ifndef LIBUVC_CONFIG_H
#define LIBUVC_CONFIG_H

#define LIBUVC_VERSION_MAJOR 
#define LIBUVC_VERSION_MINOR 
#define LIBUVC_VERSION_PATCH 
#define LIBUVC_VERSION_STR ""
#define LIBUVC_VERSION_INT                      \
  (( << 16) |             \
   ( << 8) |              \
   ())

/** @brief Test whether libuvc is new enough
 * This macro evaluates true if and only if the current version is
 * at least as new as the version specified.
 */
#define LIBUVC_VERSION_GTE(major, minor, patch)                         \
  (LIBUVC_VERSION_INT >= (((major) << 16) | ((minor) << 8) | (patch)))

/* #undef LIBUVC_HAS_JPEG */

#endif // !def(LIBUVC_CONFIG_H)

/* Project-specific overrides */
#define MY_DEFINE 1
#define MY_OTHER_DEFINE 42

