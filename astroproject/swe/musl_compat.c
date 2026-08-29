/*
 * musl 兼容 shim —— 仅供 musl libc 平台（如 Alpine Linux）编译。
 *
 * 背景：本仓库 swe/lib/linux/libswe.a 是预编译静态库，由 glibc 环境构建，
 * 内部引用了 glibc 特有的 64 位文件定位函数 fseeko64()/ftello64()。
 * musl libc 的 off_t 天生就是 64 位，直接提供 fseeko()/ftello()，
 * 但没有 glibc 风格的 *64 别名，导致链接失败：
 *   undefined reference to `fseeko64' / `ftello64'
 *
 * 本 shim 提供等价实现（musl 下 off_t 即 64 位，行为完全一致），
 * 仅当 CMake 检测到系统无 fseeko64 符号时才编译（见 CMakeLists.txt）。
 * 在 glibc 平台不会编译，避免与系统头文件中的声明冲突。
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h>

int fseeko64(FILE *stream, off_t offset, int whence)
{
    return fseeko(stream, offset, whence);
}

off_t ftello64(FILE *stream)
{
    return ftello(stream);
}
