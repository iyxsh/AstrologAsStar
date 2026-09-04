#ifndef ASTROLOG_CONFIG_H
#define ASTROLOG_CONFIG_H
#include <stddef.h>   /* size_t */
/* ============================================================================
 * P1.1 — 原版 astrolog32.dat 格式配置解释器（Config 层，A16 闸门）
 * ----------------------------------------------------------------------------
 * 词汇表 = 原版 FProcessSwitchesMain 开关子集（"原版即规格"，默认值 = 原版，
 * 即 us 静态聚合初始化，未覆盖时保持原版默认）。本模块把文本开关写入全局
 * us / oscLilith / PolarMCflip 等运行时设置，供 astrolog32-cli 与 dat 文件
 * 共用同一解释器（一份代码两个入口，避免双轨漂移）。
 *
 * 阶段边界（P1 特性域 ~25 开关，逐 P 阶段扩充）：
 *   -c <n>        宫位系统 16 系（A2）      -h <n>  中心天体
 *   -s [-s 族]    恒星黄道/分点/度数格式     -P 族   阿拉伯点个数/类型/翻转
 *   -Y 族         n 真节点 / L 真莉莉丝 / c 宫位角 / d 欧式日期 / t 欧式时间
 *                 C 智能相位 / H PolarMCflip（A3/A4/A6 依赖）
 * 尚未实现的开关一律安全 no-op（不报错），已实现但参数越界才报错 —— 与原版
 * "未知开关忽略、已知开关参数非法报错"的容错分层一致，便于逐阶段扩展。
 *
 * 线程模型：写入全局单例 us（非线程安全，与原版一致，文档明示）。
 * ==========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/* 处理一串开关 token（原版 argv 语义，如 { "-c", "5" }；-c 的数字是独立 token）。
 * 逐 token 解释并消费各自参数；遇到首个参数非法返回 0（errtxt 给原因）；
 * 全部处理完返回 1。未知/未实现开关不报错（跳过该 token）。 */
int ConfigProcessTokens(const char* const* argv, int argc,
                        char* errtxt, size_t errsz);

/* 从原版 astrolog32.dat 文本文件加载：逐行切词（空白分隔，支持双引号分组），
 * '#' 或 ';' 行首为注释；整行送入 ConfigProcessTokens。首个错误返回 0。 */
int ConfigLoadFile(const char* path, char* errtxt, size_t errsz);

/* 单开关快捷入口（内部按一个 token 处理，无独立参数跟随）。 */
int ConfigApply(const char* token, char* errtxt, size_t errsz);

#ifdef __cplusplus
}
#endif
#endif /* ASTROLOG_CONFIG_H */
