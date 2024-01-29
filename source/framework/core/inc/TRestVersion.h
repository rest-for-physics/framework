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
#define REST_RELEASE "2.4.1"
#define REST_RELEASE_DATE "Sat Dec 16"
#define REST_RELEASE_TIME "11:14:21 CET 2023"
#define REST_RELEASE_NAME "Igor G. Irastorza"
#define REST_GIT_COMMIT "6b9d0650"
#define REST_VERSION_CODE 132097
#define REST_VERSION(a, b, c) (((a) << 16) + ((b) << 8) + (c))
#define REST_SCHEMA_EVOLUTION "ON"
#endif
