#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void NslSetErrorHandler(void (*ErrorFunc)(const char *));
char *NslCompileShaderSource(const char *Src, int *BinSize);
char *NslCompileShaderFile(const char *Path, int *BinSize);

#ifdef __cplusplus
}
#endif
