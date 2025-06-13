#ifndef __glad_h_
#define __glad_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "khrplatform.h"

#define GLAD_API_CALL KHRONOS_APICALL
#define GLAD_APIENTRY KHRONOS_APIENTRY

#define GLAD_MAKE_VERSION(major, minor) ((major) << 16 | (minor))
#define GLAD_VERSION_MAJOR(version) ((version) >> 16)
#define GLAD_VERSION_MINOR(version) ((version) & 0xffff)

#define GLAD_GLEXT_VERSION GLAD_MAKE_VERSION(1, 0)

typedef void* (*GLADloadproc)(const char* name);

GLAD_API_CALL int gladLoadGL(void);
GLAD_API_CALL int gladLoadGLLoader(GLADloadproc);

#ifdef __cplusplus
}
#endif

#endif /* __glad_h_ */
