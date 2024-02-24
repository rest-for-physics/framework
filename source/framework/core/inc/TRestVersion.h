#ifndef REST_Version
#define REST_Version
/* This header has been generated using scripts/generateVersionHeader.py */

/*
 * The macros defined here can be used in the following way:
 *
 * #if REST_VERSION_CODE >= REST_VERSION(2,23,4)
 *     #include <newheader.h>
 * #else
 *     #include <oldheader.h>
 * #endif
 *
 */
#define REST_RELEASE "2.4.2"
#define REST_RELEASE_DATE "Mon Feb 12"
#define REST_RELEASE_TIME "22:23:31 CET 2024"
#define REST_RELEASE_NAME "Henry Primakoff"
#define REST_GIT_COMMIT "d8ec95be"
#define REST_VERSION_CODE 132098
#define REST_VERSION(a, b, c) (((a) << 16) + ((b) << 8) + (c))
#define REST_SCHEMA_EVOLUTION "ON"
#endif
