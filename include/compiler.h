#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void SelenaSetErrorHandler(void (*ErrorFunc)(const char *));
char *SelenaCompileShaderSource(const char *Src, int *BinSize);
char *SelenaCompileShaderFile(const char *Path, int *BinSize);

#ifdef __cplusplus
}
#endif
