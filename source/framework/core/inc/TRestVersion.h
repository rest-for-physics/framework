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
#define REST_RELEASE "2.4.3"
#define REST_RELEASE_DATE "Fri May  3"
#define REST_RELEASE_TIME "17:36:10 CEST 2024"
#define REST_RELEASE_NAME "Steven Weinberg"
#define REST_GIT_COMMIT "bc42645d"
#define REST_VERSION_CODE 132099
#define REST_VERSION(a, b, c) (((a) << 16) + ((b) << 8) + (c))
#define REST_SCHEMA_EVOLUTION "ON"
#endif
