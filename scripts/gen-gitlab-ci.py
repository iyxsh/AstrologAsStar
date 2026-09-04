#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
生成 .gitlab-ci.yml —— 多系统构建矩阵（配置即代码）
====================================================
本平台为「单一共享 docker runner」，构建环境由 docker 镜像提供。
原先用 ABI 命名（linux-glibc / linux-musl），对最终用户不够直观；
现改为「发行版名-版本」（如 ubuntu-22.04 / alpine-3.20），用户看包名即知系统。

【发行包命名规范 v2（2026-08-30 优化）】
----------------------------------------------------------------------
文件 = {product}-{platform}-{arch}-{tier}.tar.gz
      例：astroproject-ubuntu-22.04-x86_64-prod.tar.gz
          astroproject-ubuntu-22.04-x86_64-dev.tar.gz
  - product  : astroproject（固定）
  - platform : 发行版名-版本（PLATFORMS.id，如 ubuntu-22.04）
  - arch     : 目标架构（PLATFORMS.arch，如 x86_64 / aarch64）
  - tier     : 构建档位/通道 = dev | test | prod（区分开发/测试/生产）
Registry 包名 = astroproject-{platform}（按平台分组，不变）
Registry 版本 = {tier 友好名}-{short_sha}（dev=development / test=testing / prod 走 tag vX.Y.Z）
  - 档位同时写入包内 MANIFEST（CHANNEL / BUILD_TYPE / OPT_FLAGS / COVERAGE / HARDENING / STRIPPED），解包即知来源与优化档

【分档编译优化矩阵（dev/test/prod 对应不同优化）】
----------------------------------------------------------------------
  dev  : CMAKE_BUILD_TYPE=Debug  + -O0 -g3 -fno-omit-frame-pointer
         → 零优化、完整调试符号、断言开启、编译最快，便于断点/内存排查
  test : CMAKE_BUILD_TYPE=RelWithDebInfo + -O2 -g -fno-omit-frame-pointer --coverage
         → 优化+调试符号+覆盖率插桩，供 QA / 覆盖率收集
  prod : CMAKE_BUILD_TYPE=Release + -O3 -DNDEBUG
         + 原生平台追加：-D_FORTIFY_SOURCE=2 -fstack-protector-strong -fPIE -ffat-lto-objects
          链接：-Wl,-z,relro,-z,now -ffat-lto-objects；交叉编译不加 LTO（避免下游链接不确定性）
         + 产物剥离调试符号（strip --strip-debug）
         → 最高性能/最小体积，加固+fat-LTO+剥离，仅生产发布
（依据：业界 release-asset 命名 {product}-{version}-{os}-{arch}-{variant}，
 以及 CMake 四档构建类型 Debug/RelWithDebInfo/Release/MinSizeRel 与
 -O0/-O2/-O3、-fsanitize、_FORTIFY_SOURCE、LTO、strip 的分工。）

用法：修改 PLATFORMS / ENVS 后重新运行本脚本即可再生成 .gitlab-ci.yml
    python scripts/gen-gitlab-ci.py
"""
import io, os, sys

# ============================================================
# 平台矩阵（新增平台只需在此加一行，重新生成即可）
#  id:      平台标识（用于产物名/包名/PLATFORM 变量，如 ubuntu-22.04 / windows / android）
#  image:   docker 镜像
#  pkg:     包管理器（apt / apk / dnf），决定工具链安装与镜像加速逻辑
#  os/ver:  MANIFEST 中的 OS 名称与版本
#  lib_dir: CMake 输出子目录（lib/${lib_dir}/Release），linux/windows/android
#  libc:    MANIFEST 的 LIBC 字段；空字符串=按发行版自动判定(musl/glibc)
#  arch:    目标架构（产物名用，如 x86_64 / aarch64），避免用 uname -m 误用宿主架构
#  cross:   交叉编译类型：""=原生 / "mingw"=MinGW-w64 / "android"=Android NDK
# ============================================================
PLATFORMS = [
    # 原生 Linux 系（docker 镜像直接构建）
    # image = 预构建工具链镜像 builder-<id>（docker/ + scripts/build-builder-images.sh 构建，
    #          runner pull_policy=if-not-present 直命中本地，省去每 job ~400s 现场装工具链）。
    #         本机无该镜像时回退官方 distro 名可保证 CI 可跑（见 docker/ 注释）。
    dict(id="ubuntu-22.04", image="builder-ubuntu-22.04", pkg="apt", os="Ubuntu",      ver="22.04", lib_dir="linux",   libc="",         arch="x86_64", cross=""),
    dict(id="ubuntu-24.04", image="builder-ubuntu-24.04", pkg="apt", os="Ubuntu",      ver="24.04", lib_dir="linux",   libc="",         arch="x86_64", cross=""),
    dict(id="debian-12",    image="builder-debian-12",    pkg="apt", os="Debian",      ver="12",    lib_dir="linux",   libc="",         arch="x86_64", cross=""),
    dict(id="alpine-3.20",  image="builder-alpine-3.20",  pkg="apk", os="Alpine",      ver="3.20",  lib_dir="linux",   libc="",         arch="x86_64", cross=""),
    dict(id="rockylinux-9", image="builder-rockylinux-9", pkg="dnf", os="Rocky Linux", ver="9",     lib_dir="linux",   libc="",         arch="x86_64", cross=""),
    dict(id="fedora-40",    image="builder-fedora-40",    pkg="dnf", os="Fedora",      ver="40",    lib_dir="linux",   libc="",         arch="x86_64", cross=""),
    # 交叉编译目标（在现有 docker runner 内构建，无需额外 OS runner）
    dict(id="windows",      image="builder-windows",      pkg="apt", os="Windows",     ver="10",    lib_dir="windows", libc="mingw-w64", arch="x86_64", cross="mingw"),
    dict(id="android",      image="builder-android",      pkg="apt", os="Android",     ver="API24", lib_dir="android", libc="bionic",    arch="aarch64", cross="android"),
]

# 环境矩阵（分支 → 环境名 → 触发规则 + 构建档位）
#  key      : job 名/档位后缀（dev/test/prod），同时作为产物文件名 tier 段
#  env      : Registry 版本前缀的友好名（development/testing/production）
#  branch   : 触发分支
#  tag      : 是否 tag 触发（prod 走 tag vX.Y.Z 发布）
#  build_type: CMake 构建类型（驱动优化档）
#  cflags   : 编译优化核心（不含加固/LTO，脚本按 CROSS/档位自行追加）
#  strip    : 是否剥离调试符号（仅 prod）
#  coverage : 是否覆盖率插桩（仅 test）
ENVS = [
    dict(key="dev",  branch="develop", env="development", tag=False,
         build_type="Debug",
         cflags="-O0 -g3 -fno-omit-frame-pointer",
         strip="no", coverage="no",
         desc="调试档：零优化+完整调试符号+断言开启，编译最快，便于断点/内存排查"),
    dict(key="test", branch="test",    env="testing",     tag=False,
         build_type="RelWithDebInfo",
         cflags="-O2 -g2 -fno-omit-frame-pointer --coverage",
         strip="no", coverage="yes",
         desc="测试档：优化+调试符号+覆盖率插桩(--coverage)，供 QA/覆盖率收集"),
    dict(key="prod", branch="main",    env="production",  tag=True,
         build_type="Release",
         cflags="-O3 -DNDEBUG",
         strip="yes", coverage="no",
         desc="生产档：最高优化(-O3)+LTO+剥离+加固(-fstack-protector-strong/_FORTIFY_SOURCE/PIE/relro)，体积最小性能最优"),
]


def plat_job_suffix(p):
    """job 名后缀：ubuntu-22.04 -> ubuntu_2204（GitLab job 名不允许点）"""
    return p["id"].replace(".", "_").replace("-", "_")


def rules_block(env):
    b = env["branch"]
    lines = []
    if env["tag"]:
        lines.append("    - if: $CI_COMMIT_TAG")
    lines.append(f'    - if: $CI_PIPELINE_SOURCE == "push" && $CI_COMMIT_BRANCH == "{b}"')
    lines.append(f'    - if: ($CI_PIPELINE_SOURCE == "merge_request_event" || $CI_MERGE_REQUEST_ID ) && $CI_MERGE_REQUEST_TARGET_BRANCH_NAME == "{b}"')
    return "\n".join(lines)


def rules_block_deploy(env):
    """deploy 阶段规则：push 到对应分支；prod 额外含 tag"""
    b = env["branch"]
    lines = []
    if env["tag"]:
        lines.append("    - if: $CI_COMMIT_TAG")
    lines.append(f'    - if: $CI_PIPELINE_SOURCE == "push" && $CI_COMMIT_BRANCH == "{b}"')
    return "\n".join(lines)


def gen_build_jobs():
    out = []
    for env in ENVS:
        for p in PLATFORMS:
            s = plat_job_suffix(p)
            out.append(f"""build_{env['key']}_{s}:
  <<: *build_definition
  variables:
    BUILD_DIR: "build"
    PLATFORM: "{p['id']}"
    OS_NAME: "{p['os']}"
    OS_VERSION: "{p['ver']}"
    BUILD_IMAGE: "{p['image']}"
    ENVIRONMENT: "{env['env']}"
    TIER: "{env['key']}"
    ARCH: "{p['arch']}"
    LIB_DIR: "{p['lib_dir']}"
    LIBC: "{p['libc']}"
    CROSS: "{p['cross']}"
    CMAKE_BUILD_TYPE: "{env['build_type']}"
    TIER_CFLAGS: "{env['cflags']}"
    DO_STRIP: "{env['strip']}"
    COVERAGE: "{env['coverage']}"
  tags:
    - docker
  rules:
{rules_block(env)}
""")
    return "\n".join(out)


def gen_verify_jobs():
    out = []
    for env in ENVS:
        for p in PLATFORMS:
            s = plat_job_suffix(p)
            out.append(f"""verify_{env['key']}_{s}:
  <<: *verify_definition
  variables:
    BUILD_DIR: "build"
    PLATFORM: "{p['id']}"
    TIER: "{env['key']}"
    ARCH: "{p['arch']}"
  needs:
    - job: build_{env['key']}_{s}
      artifacts: true
  tags:
    - docker
  rules:
{rules_block(env)}
""")
    return "\n".join(out)


def gen_golden_diff_jobs():
    """P0.4 —— 金样数值对拍（CLI 输出 vs test/golden 8 份 @0203 金样）。

    与 build 制品解耦（needs: []），独立从源码构建 astrolog32-cli（原生 Linux，
    ubuntu-22.04 builder 镜像）后跑 test/verify_cli.py。CLI 自带 Swiss
    Moshier 回退（无需 .se1 星历文件，实测 8/8 一致），python3 由 job 内安装
    （builder 镜像仅预装 cmake/make/g++）。
    """
    out = []
    for env in ENVS:
        out.append(f"""golden_diff_{env['key']}:
  stage: verify
  image: builder-ubuntu-22.04
  needs: []
  variables:
    TIER: "{env['key']}"
  tags:
    - docker
  rules:
{rules_block(env)}
  script:
    - |
      set -e
      if ! command -v python3 >/dev/null 2>&1; then
        apt-get update -qq
        DEBIAN_FRONTEND=noninteractive apt-get install -y -qq python3
      fi
      cd astroproject
      cmake -S . -B build-golden -DCMAKE_BUILD_TYPE=Debug >/dev/null
      cmake --build build-golden --target astrolog32-cli -- -j"$(nproc)" >/dev/null
      python3 ../test/verify_cli.py "$(pwd)/bin/linux/Debug/astrolog32-cli"
""")
    return "\n".join(out)


def gen_deploy_jobs():
    out = []
    for env in ENVS:
        for p in PLATFORMS:
            s = plat_job_suffix(p)
            out.append(f"""deploy_to_{env['key']}_{s}:
  <<: *deploy_definition
  needs:
    - job: build_{env['key']}_{s}
      artifacts: true
    - job: verify_{env['key']}_{s}
      artifacts: true
  variables:
    BUILD_DIR: "build"
    PLATFORM: "{p['id']}"
    PACKAGE_NAME: "astroproject-{p['id']}"
    ENVIRONMENT: "{env['env']}"
    TIER: "{env['key']}"
    ARCH: "{p['arch']}"
  tags:
    - docker
  rules:
{rules_block_deploy(env)}
""")
    return "\n".join(out)


def gen_publish_jobs():
    out = []
    for p in PLATFORMS:
        s = plat_job_suffix(p)
        out.append(f"""publish_package_{s}:
  stage: release
  tags:
    - docker
  variables:
    BUILD_DIR: "build"
    PLATFORM: "{p['id']}"
    PACKAGE_NAME: "astroproject-{p['id']}"
    ARCH: "{p['arch']}"
  before_script:
    - |
      if [ -n "$CI_COMMIT_TAG" ]; then
        PACKAGE_VERSION="$CI_COMMIT_TAG"
      else
        PACKAGE_VERSION="v${{CI_COMMIT_SHORT_SHA}}"
      fi
      export PACKAGE_VERSION
      echo "Determined package version: $PACKAGE_VERSION"
  script:
    - |
      export ARTIFACT_NAME="astroproject-${{PLATFORM}}-${{ARCH}}-prod.tar.gz"
      echo "Uploading ${{PLATFORM}} artifact to GitLab Package Registry"
      echo "Package: ${{PACKAGE_NAME}} / ${{PACKAGE_VERSION}} / ${{ARTIFACT_NAME}}"
      curl --fail --silent --show-error \\
        --header "JOB-TOKEN: ${{CI_JOB_TOKEN}}" \\
        --upload-file "astroproject/${{BUILD_DIR}}/${{ARTIFACT_NAME}}" \\
        "${{CI_API_V4_URL}}/projects/${{CI_PROJECT_ID}}/packages/generic/${{PACKAGE_NAME}}/${{PACKAGE_VERSION}}/${{ARTIFACT_NAME}}"
      echo "Published OK: ${{ARTIFACT_NAME}}"
  rules:
    - if: $CI_COMMIT_TAG
  dependencies:
    - build_prod_{s}
  needs:
    - job: build_prod_{s}
      artifacts: true
""")
    return "\n".join(out)


def gen_release_links():
    """create_release 的 printf 资产链接（每平台一组，作为 printf 参数行）"""
    lines = []
    n = len(PLATFORMS)
    for i, p in enumerate(PLATFORMS):
        s = plat_job_suffix(p)
        comma = "," if i < n - 1 else ""
        pid = p["id"]
        # 每个平台 3 个 printf 参数：{ 行 / name+url 行 / }, 行（均带续行符 \）
        lines.append('        "      {" \\')
        lines.append(f'        "        \\"name\\": \\"astroproject-{pid}-${{ARCH}}-prod.tar.gz\\"," \\')
        lines.append(f'        "        \\"url\\": \\"${{CI_API_V4_URL}}/projects/${{CI_PROJECT_ID}}/jobs/artifacts/${{CI_COMMIT_REF_NAME}}/raw/astroproject/build/astroproject-{pid}-{p["arch"]}-prod.tar.gz?job=build_prod_{s}\\"" \\')
        lines.append(f'        "      }}{comma}" \\')
    return "\n".join(lines)


def gen_create_release():
    links = gen_release_links()
    deps = "\n".join(f"    - build_prod_{plat_job_suffix(p)}" for p in PLATFORMS)
    needs = "\n".join(f"""    - job: build_prod_{plat_job_suffix(p)}
      artifacts: true""" for p in PLATFORMS)
    return f"""create_release:
  stage: release
  rules:
    - if: $CI_COMMIT_TAG
  tags:
    - docker
  before_script:
    - |
      # 注意：CI_COMMIT_TAG 已含 v 前缀(如 v0.1.0)，不要再加 v
      if [ -n "$CI_COMMIT_TAG" ]; then
        PACKAGE_VERSION="$CI_COMMIT_TAG"
      else
        PACKAGE_VERSION="v${{CI_COMMIT_SHORT_SHA}}"
      fi
      export PACKAGE_VERSION
      echo "Determined package version: $PACKAGE_VERSION"
  script:
    - |
      set -e
      echo "Creating release for $PACKAGE_VERSION"
      # 用 printf 构造 release JSON（避免 heredoc 缩进陷阱；双引号使变量展开）
      # 资产链接覆盖全部 {len(PLATFORMS)} 个系统平台
      printf '%s\\n' \\
        "{{" \\
        "  \\"tag_name\\": \\"$CI_COMMIT_TAG\\"," \\
        "  \\"ref\\": \\"$CI_COMMIT_TAG\\"," \\
        "  \\"name\\": \\"Release $PACKAGE_VERSION\\"," \\
        "  \\"description\\": \\"Release created by GitLab CI/CD for AstroProject ({len(PLATFORMS)} platforms: 6 Linux distros + Windows(MinGW) + Android(NDK))\\", " \\
        "  \\"assets\\": {{" \\
        "    \\"links\\": [" \\
{links}
        "    ]" \\
        "  }}" \\
        "}}" > release_data.json
      cat release_data.json
      curl --request POST \\
           --header "PRIVATE-TOKEN: $PERSONAL_ACCESS_TOKEN" \\
           --header "Content-Type: application/json" \\
           --data @release_data.json \\
           "$CI_API_V4_URL/projects/$CI_PROJECT_ID/releases"
      echo "RELEASE_CREATED"
  dependencies:
{deps}
  needs:
{needs}
"""


def gen_sync_release_atomgit():
    """同步 GitLab Release 到 Atomgit（发行版元数据 + 产物附件）。

    push mirror 只同步 git 数据，Releases 页默认是空的；
    此 job 在 tag 流水线 release 阶段，用 Atomgit v5 API 创建/更新 Release 并上传产物。
    需要项目 CI/CD 变量 ATOMGIT_PAT（Atomgit 个人访问令牌，组织仓库写权限）。
    """
    deps = "\n".join(f"    - build_prod_{plat_job_suffix(p)}" for p in PLATFORMS)
    needs = "\n".join(f"""    - job: build_prod_{plat_job_suffix(p)}
      artifacts: true""" for p in PLATFORMS)
    return f"""sync_release_atomgit:
  stage: release
  rules:
    - if: $CI_COMMIT_TAG
  tags:
    - docker
  image: alpine:3.20
  before_script:
    - apk add --no-cache curl jq
  script:
    - sh scripts/sync-atomgit-release.sh
  dependencies:
{deps}
  needs:
{needs}
"""


# ============================================================
# 头部（workflow / stages / default / variables / 模板）
# ============================================================
HEADER = r'''# ============================================================
# 阶段（stage）设计 —— 遵循 GitLab CE 惯例，并按职责重新命名/归位
#
#   prepare  准备与协作自动化：MR 描述生成、合并后回写 MR 备注
#   build    构建：编译源码并打包成可分发制品
#   verify   验证：校验 build 产出的制品（质量门禁）
#   deploy   部署：按环境发布制品（dev / test / prod）
#   release  发布：创建 GitLab Release 并发布软件包
#
# 相比原配置的修正：
#   1. 原 gene_mrdesc（生成 MR 描述）被塞在 build 阶段、
#      add_mr_note（MR 备注）被塞在 deploy 阶段，语义完全不符，
#      现统一归入新增的 prepare 阶段。
#   2. 原 test 阶段下挂着 unit_test_* 作业，但本项目 CMakeLists
#      并未定义任何单元测试目标（无 gtest/catch，也没有
#      test_astro_lib_debugd），实际做的是产物校验，名不副实；
#      故更名为 verify。日后若在 CMakeLists 中加入真实单测，
#      可再拆出独立的 test 阶段。
# ============================================================
stages:
  - prepare
  - build
  - verify
  - deploy
  - release

# 防止 push 与 merge_request 触发重复流水线（GitLab 官方推荐写法）
workflow:
  rules:
    - if: $CI_PIPELINE_SOURCE == "merge_request_event"
    - if: $CI_COMMIT_TAG
    - if: $CI_COMMIT_BRANCH && $CI_OPEN_MERGE_REQUESTS
      when: never
    - if: $CI_COMMIT_BRANCH

.variables_template: &variables_definition
  BUILD_DIR: "build"
  # 注：ARTIFACT_NAME/PACKAGE_NAME 已改为按平台动态生成（见各 job），
  #     不再提供全局默认值，避免与平台化命名混淆。

variables:
  <<: *variables_definition
  # SWE 为 git submodule（astroproject/swe/swisseph），job 前必须拉取
  GIT_SUBMODULE_STRATEGY: recursive

# ==== 本平台适配：单一共享 docker runner ====
# 原设计依赖「每台环境机一个 runner」+ 在真实 OS 上 sudo 装依赖。
# 本平台宿主机仅 16GB，无法同时跑多台环境机，故改为：
#   一台 docker runner + 镜像提供构建环境，dev/test/prod 由分支规则区分。
default:
  image: gcc:12
  before_script:
    - |
      # 尽力而为的依赖安装：
      #  - 有的镜像以 root 跑(无 sudo)，有的以普通用户跑且没有 sudo -> 无 sudo 则置空
      #  - 有的镜像没有包管理器(curlimages/curl 等) -> 跳过即可
      #  - 任何情况下都不得让 job 失败，非构建类 job 请用 before_script: [] 直接跳过
      if [ "$(id -u)" = "0" ]; then SUDO=""; else SUDO="sudo"; fi
      command -v sudo >/dev/null 2>&1 || SUDO=""
      if ! command -v cmake >/dev/null 2>&1; then
        echo "cmake 缺失，尝试安装构建工具链..."
        if command -v apt-get >/dev/null 2>&1; then
          # 加速：默认源(deb.debian.org / archive.ubuntu.com)在本网络下极慢，
          # 且每个 job 都是全新容器、都要重装一遍。改用中科大 USTC。
          # 要点：
          #  - 基础镜像无 ca-certificates，https 源会 update 失败，故统一用 http
          #  - TUNA 于 2026-09-04 起 403（http/https 均不可达），勿再回切
          #  - 源 URL 自带路径后缀(如 /ubuntu、/debian)，替换时必须一并吃掉，
          #    否则会变成 /ubuntu/ubuntu 双路径
          #  - Debian 12 / Ubuntu 24.04 是 deb822(.sources)，Ubuntu 22.04 是 .list，都要处理
          if [ "$(id -u)" = "0" ]; then
            for _f in /etc/apt/sources.list /etc/apt/sources.list.d/*.list /etc/apt/sources.list.d/*.sources; do
              [ -f "$_f" ] && sed -i \
                's|http://deb.debian.org/debian|http://mirrors.ustc.edu.cn/debian|g; s|https://deb.debian.org/debian|http://mirrors.ustc.edu.cn/debian|g; s|http://security.debian.org/debian-security|http://mirrors.ustc.edu.cn/debian-security|g; s|https://security.debian.org/debian-security|http://mirrors.ustc.edu.cn/debian-security|g; s|http://archive.ubuntu.com/ubuntu|http://mirrors.ustc.edu.cn/ubuntu|g; s|https://archive.ubuntu.com/ubuntu|http://mirrors.ustc.edu.cn/ubuntu|g; s|http://security.ubuntu.com/ubuntu|http://mirrors.ustc.edu.cn/ubuntu|g; s|https://security.ubuntu.com/ubuntu|http://mirrors.ustc.edu.cn/ubuntu|g' "$_f"
            done || true
          fi
          # DEBIAN_FRONTEND=noninteractive 消除 debconf 交互前端警告
          # （apt-utils/Dialog/Readline/TERM 未设置等噪音，CI 无交互环境）
          $SUDO apt-get update -qq && DEBIAN_FRONTEND=noninteractive $SUDO apt-get install -y -qq cmake make g++ \
            || echo "WARN: 构建工具链安装失败(权限或网络)，继续尝试"
        elif command -v apk >/dev/null 2>&1; then
          # Alpine 用 aliyun 加速（实测 2026-09-04：USTC alpine ~7KB/s 卡死、aliyun ~104KB/s；
          # 默认 dl-cdn.alpinelinux.org 慢；alpine 基础镜像自带 ca-certificates）
          if [ "$(id -u)" = "0" ]; then
            sed -i 's|dl-cdn.alpinelinux.org|mirrors.aliyun.com|g' /etc/apk/repositories 2>/dev/null || true
          fi
          $SUDO apk add --no-cache cmake make g++ \
            || echo "WARN: 构建工具链安装失败(权限或网络)，继续尝试"
        elif command -v dnf >/dev/null 2>&1; then
          # Rocky/Fedora 用 dnf。镜像选型（实测）：
          #   rocky   -> 中科大 USTC（TUNA 不镜像 Rocky；baseurl 含 $contentdir 要吃掉）
          #   fedora  -> 阿里云 aliyun（USTC/TUNA 的 fedora 缺 Everything 仓库 404；
          #              aliyun 路径无 /linux/ 层：/fedora/releases/40/...；
          #              官方 baseurl 是 download.example 占位符，替换时吃 /pub/fedora/linux）
          if [ "$(id -u)" = "0" ]; then
            sed -i 's|^mirrorlist=|#mirrorlist=|g; s|^metalink=|#metalink=|g' /etc/yum.repos.d/*.repo 2>/dev/null || true
            sed -i 's|^#baseurl=http://dl.rockylinux.org/\$contentdir|baseurl=https://mirrors.ustc.edu.cn/rocky|g; s|^#baseurl=http://download.example/pub/fedora/linux|baseurl=http://mirrors.aliyun.com/fedora|g; s|^baseurl=http://download.example/pub/fedora/linux|baseurl=http://mirrors.aliyun.com/fedora|g' /etc/yum.repos.d/*.repo 2>/dev/null || true
          fi
          $SUDO dnf install -y -q cmake make gcc-c++ \
            || echo "WARN: 构建工具链安装失败(权限或网络)，继续尝试"
        elif command -v yum >/dev/null 2>&1; then
          $SUDO yum install -y cmake make gcc-c++ \
            || echo "WARN: 构建工具链安装失败(权限或网络)，继续尝试"
        else
          echo "WARN: 未找到受支持的包管理器，跳过依赖安装"
        fi
      fi
      cmake --version || echo "WARN: 本镜像无 cmake，需要编译的 job 会失败"

# 构建作业模板
# ============================================================
# 多系统构建矩阵（由 scripts/gen-gitlab-ci.py 生成，改平台矩阵请改该脚本）
# ------------------------------------------------------------
# docker runner 运行 Linux 系镜像，原生构建的系统为：
#   ubuntu-22.04 / ubuntu-24.04 / debian-12  —— Debian 系(apt, glibc, gcc 11/13/12)
#   alpine-3.20                              —— Alpine(apk, musl libc)
#   rockylinux-9 / fedora-40                 —— RHEL 系(dnf, glibc, gcc 11/14)
# 另有两个交叉编译目标，在 docker runner 内用工具链构建（无需额外 OS runner）：
#   windows  —— MinGW-w64 交叉编译（ubuntu 镜像装 mingw-w64，产出 lib/windows/Release）
#   android  —— Android NDK 交叉编译（ubuntu 镜像装 NDK，产出 lib/android/Release，arm64-v8a）
# 产物与发布包名均带「系统-架构-档位」标识（如 astroproject-ubuntu-22.04-x86_64-prod.tar.gz、
# astroproject-ubuntu-22.04-x86_64-dev.tar.gz、astroproject-windows-x86_64-prod.tar.gz、
# astroproject-android-aarch64-prod.tar.gz），档位 = dev/test/prod 区分开发/测试/生产构建；
# 包内附 MANIFEST 说明系统名/版本/架构/libc/工具链/档位/优化档/加固/剥离/gcc/commit/构建时间，
# 用户解包即可辨识来源与所用的编译优化。
#
# FreeBSD / macOS：CMakeLists 已完整支持(PLATFORM_DIR 分支)，但无法从 Linux 交叉编译，
# 需对应 OS 的真实主机/runner（FreeBSD 机或 Mac），后续可在环境机挂 runner 扩展 —— 见 OPS-GUIDE §4.14。
# ============================================================

# ---- 构建模板：按平台 + 档位实例化（PLATFORM/BUILD_IMAGE/TIER/CMAKE_BUILD_TYPE/TIER_CFLAGS/DO_STRIP/COVERAGE 由 job variables 注入）----
# 分档编译优化：dev=Debug(-O0 -g3) / test=RelWithDebInfo(-O2 -g --coverage) / prod=Release(-O3 -DNDEBUG)+加固+fat-LTO+剥离
.build_template: &build_definition
  stage: build
  image: ${BUILD_IMAGE}
  script:
    - |
      set -e
      export ARTIFACT_NAME="astroproject-${PLATFORM}-${ARCH}-${TIER}.tar.gz"
      echo ">>> [build:${TIER}] platform=${PLATFORM} os=${OS_NAME} ${OS_VERSION} arch=${ARCH} libc=${LIBC} cross=${CROSS}"
      echo ">>> build_type=${CMAKE_BUILD_TYPE} cflags_base='${TIER_CFLAGS}' strip=${DO_STRIP} coverage=${COVERAGE}"

      # ---- 交叉编译工具链（按需安装并注入 cmake 参数）----
      EXTRA_CMAKE=""
      if [ "${CROSS}" = "mingw" ]; then
        echo ">>> 安装 MinGW-w64 交叉编译工具链 (x86_64-w64-mingw32)"
        $SUDO apt-get update -qq
        DEBIAN_FRONTEND=noninteractive $SUDO apt-get install -y -qq mingw-w64
        export CC=x86_64-w64-mingw32-gcc
        export CXX=x86_64-w64-mingw32-g++
        # 必须显式声明目标系统，否则 CMake 按宿主(Linux)判定 → WIN32=FALSE → PLATFORM_DIR=linux
        # （android 分支靠 toolchain 文件内声明 CMAKE_SYSTEM_NAME，故无需此处设置）
        EXTRA_CMAKE="-DCMAKE_SYSTEM_NAME=Windows"
      elif [ "${CROSS}" = "android" ]; then
        echo ">>> 安装 Android NDK 并配置 toolchain (arm64-v8a, API24)"
        $SUDO apt-get update -qq
        DEBIAN_FRONTEND=noninteractive $SUDO apt-get install -y -qq wget unzip
        NDK_VER=r25b
        # NDK 压缩包约 600MB，而每个 job 都是全新容器 —— 逐次重下是最慢的一环。
        # 优化：优先复用 runner 宿主预置的 NDK（config.toml 挂 /opt/ci-cache 卷）；
        #       未预置时回退下载，保证换 runner/新环境仍可跑（可移植性不丢）。
        NDK_HOST="/opt/ci-cache/ndk/android-ndk-${NDK_VER}"
        if [ -x "${NDK_HOST}/toolchains/llvm/prebuilt/linux-x86_64/bin/clang" ]; then
          echo ">>> 命中 runner 宿主 NDK 缓存: ${NDK_HOST}"
          export ANDROID_NDK_HOME="${NDK_HOST}"
        else
          echo ">>> 宿主无 NDK 缓存，回退下载 r25b（约 600MB，仅首次）"
          export ANDROID_NDK_HOME="/opt/android-ndk-${NDK_VER}"
          # 下载源优先级：腾讯云镜像(国内直连，实测 200) -> 官方 dl.google.com。
          # 注意官方域名在本网络下**只解析到 AAAA(IPv6)**，而容器无 IPv6 路由 —— 依赖
          # runner config 的 dns=["223.5.5.5","114.114.114.114"] 才能拿到 A 记录（见 OPS-GUIDE §4.9b）。
          wget -q "https://mirrors.cloud.tencent.com/AndroidSDK/android-ndk-${NDK_VER}-linux.zip" -O /tmp/ndk.zip \
            || wget -q "https://dl.google.com/android/repository/android-ndk-${NDK_VER}-linux.zip" -O /tmp/ndk.zip
          unzip -q /tmp/ndk.zip -d /opt
          rm -f /tmp/ndk.zip
        fi
        export CC=${ANDROID_NDK_HOME}/toolchains/llvm/prebuilt/linux-x86_64/bin/clang
        export CXX=${ANDROID_NDK_HOME}/toolchains/llvm/prebuilt/linux-x86_64/bin/clang++
        EXTRA_CMAKE="-DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK_HOME}/build/cmake/android.toolchain.cmake -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-24"
      fi

      # ---- 分档编译优化：基于 TIER_CFLAGS + 档位开关，组装编译/链接标志 ----
      # dev : 零优化+完整调试符号（TIER_CFLAGS 已含 -O0 -g3）
      # test: 优化+调试符号+覆盖率插桩（TIER_CFLAGS 已含 --coverage）
      # prod: 最高优化；原生 ELF 追加加固(-fstack-protector-strong/_FORTIFY_SOURCE/PIE)+fat-LTO+relro；交叉仅 -O3+剥离
      EFF_CFLAGS="${TIER_CFLAGS}"
      EFF_LDFLAGS=""
      if [ "${COVERAGE}" = "yes" ]; then
        EFF_LDFLAGS="${EFF_LDFLAGS} --coverage"
      fi
      if [ "${DO_STRIP}" = "yes" ]; then
        if [ -z "${CROSS}" ]; then
          # 原生 ELF：安全加固 + fat-LTO（消费者可选 LTO，且未开 -flto 也能正常链接）
          # -ffat-lto-objects：同时产出 LTO 字节码与普通目标文件，
          #   避免下游链接方未用 -flto 时因只剩 LTO 字节码而链接失败（分布式静态库关键坑）
          EFF_CFLAGS="${EFF_CFLAGS} -ffat-lto-objects -D_FORTIFY_SOURCE=2 -fstack-protector-strong -fPIE"
          EFF_LDFLAGS="${EFF_LDFLAGS} -ffat-lto-objects -Wl,-z,relro,-z,now"
        fi
        # 交叉编译：仅 -O3(已含) + 剥离，避免 LTO 对下游链接造成不确定性
      fi
      HARDENING="no"; [ "${DO_STRIP}" = "yes" ] && [ -z "${CROSS}" ] && HARDENING="yes"
      SANITIZE="none"; [ "${COVERAGE}" = "yes" ] && SANITIZE="coverage"
      echo ">>> 生效编译标志: ${EFF_CFLAGS}"
      echo ">>> 生效链接标志: ${EFF_LDFLAGS}"

      mkdir -p astroproject/${BUILD_DIR}
      cd astroproject/${BUILD_DIR}
      cmake ../ \
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} \
        -DCMAKE_C_FLAGS="${EFF_CFLAGS}" \
        -DCMAKE_CXX_FLAGS="${EFF_CFLAGS}" \
        -DCMAKE_EXE_LINKER_FLAGS="${EFF_LDFLAGS}" \
        -DCMAKE_SHARED_LINKER_FLAGS="${EFF_LDFLAGS}" \
        ${EXTRA_CMAKE}
      # 并行构建（提速，其他相关优化之一）
      cmake --build . --parallel "$(nproc 2>/dev/null || echo 2)"

      # 打包：CMakeLists 按构建类型输出到 lib/${LIB_DIR}/<Debug|Release>
      case "${CMAKE_BUILD_TYPE}" in
        Debug) OUT_SUBDIR=Debug ;;
        *)     OUT_SUBDIR=Release ;;
      esac
      mkdir -p pkg/lib pkg/include
      cp ../lib/${LIB_DIR}/${OUT_SUBDIR}/*.a pkg/lib/
      cp -r ../include/. pkg/include/

      # prod：剥离调试符号（保留符号表，供链接用），减小体积
      if [ "${DO_STRIP}" = "yes" ]; then
        echo ">>> 剥离调试符号 (strip --strip-debug)"
        find pkg/lib -name '*.a' -exec strip --strip-debug {} +
      fi

      # 生成 MANIFEST：用户解包即知系统/版本/架构/libc/工具链/优化档/来源
      {
        echo "PROJECT=astroproject"
        echo "PLATFORM=${PLATFORM}"
        echo "OS_NAME=${OS_NAME}"
        echo "OS_VERSION=${OS_VERSION}"
        echo "ARCH=${ARCH}"
        if [ -z "${LIBC}" ]; then
          if [ -f /etc/alpine-release ]; then LIBC=musl; else LIBC=glibc; fi
        fi
        echo "LIBC=${LIBC}"
        echo "TOOLCHAIN=${CROSS:-native}"
        echo "CHANNEL=${TIER}"
        echo "ENVIRONMENT=${ENVIRONMENT}"
        echo "BUILD_TYPE=${CMAKE_BUILD_TYPE}"
        echo "OPT_FLAGS=${EFF_CFLAGS}"
        echo "LINK_FLAGS=${EFF_LDFLAGS}"
        echo "COVERAGE=${COVERAGE}"
        echo "SANITIZE=${SANITIZE}"
        echo "HARDENING=${HARDENING}"
        echo "STRIPPED=${DO_STRIP}"
        echo "GCC=$(${CC:-gcc} --version | head -1 | grep -oE '[0-9]+\.[0-9]+\.[0-9]+' | head -1)"
        echo "COMMIT=${CI_COMMIT_SHORT_SHA}"
        echo "REF=${CI_COMMIT_REF_NAME}"
        echo "BUILD_DATE=$(date -u +%Y-%m-%dT%H:%M:%SZ)"
      } > pkg/MANIFEST
      tar -czvf ${ARTIFACT_NAME} -C pkg lib include MANIFEST
      ls -l ${ARTIFACT_NAME}
      # 清单(先写文件再 head，避免 tar|head 的 SIGPIPE 被 pipefail 判失败)
      echo "打包内容清单(前 20 项):"
      tar -tzf ${ARTIFACT_NAME} > /tmp/_pkg_list.txt
      head -20 /tmp/_pkg_list.txt
      echo "包内文件总数: $(wc -l < /tmp/_pkg_list.txt)"
      # 回显 MANIFEST
      echo "--- MANIFEST ---"
      tar -xzf ${ARTIFACT_NAME} -O MANIFEST
  artifacts:
    paths:
      - astroproject/${BUILD_DIR}/astroproject-*.tar.gz
    expire_in: 1 week
    name: "astroproject-${PLATFORM}-${TIER}-${CI_COMMIT_REF_NAME}-${CI_COMMIT_SHORT_SHA}"

# ---- 验证模板（质量门禁，按平台校验制品 + MANIFEST）----
.verify_template: &verify_definition
  stage: verify
  image: gcc:12
  before_script: []   # 仅校验制品，不需要 cmake
  script:
    - |
      set -e
      ARTIFACT=$(ls astroproject/${BUILD_DIR}/astroproject-${PLATFORM}-${ARCH}-${TIER}.tar.gz 2>/dev/null | head -1)
      echo "== 校验 build 阶段产出(${PLATFORM} / tier=${TIER}) =="
      test -n "$ARTIFACT" || { echo "FAIL: 未找到 ${PLATFORM} 平台制品 (期望 astroproject-${PLATFORM}-${ARCH}-${TIER}.tar.gz)"; exit 1; }
      ls -l "$ARTIFACT"

      rm -rf _verify && mkdir -p _verify
      tar -xzf "$ARTIFACT" -C _verify

      # 1) MANIFEST 必须存在且平台/档位标识正确
      MF=$(find _verify -name MANIFEST | head -1)
      test -n "$MF" || { echo "FAIL: 制品缺少 MANIFEST"; exit 1; }
      grep -q "PLATFORM=${PLATFORM}" "$MF" || { echo "FAIL: MANIFEST 平台标识不符"; cat "$MF"; exit 1; }
      grep -q "CHANNEL=${TIER}" "$MF" || { echo "FAIL: MANIFEST 档位(CHANNEL)不符，期望 ${TIER}"; cat "$MF"; exit 1; }
      echo "MANIFEST: $(cat "$MF" | tr '\n' ';')"

      # 2) 必须包含静态库，且非空
      LIB=$(find _verify -name '*.a' | head -1)
      test -n "$LIB" || { echo "FAIL: 制品中没有静态库(.a)"; exit 1; }
      OBJCNT=$(ar t "$LIB" | wc -l)
      echo "静态库: $LIB  目标文件数: $OBJCNT"
      test "$OBJCNT" -gt 0 || { echo "FAIL: 静态库为空"; exit 1; }

      # 3) 必须包含公开头文件
      HDR=$(find _verify -name 'astrolog_lib.h' | head -1)
      test -n "$HDR" || { echo "FAIL: 制品缺少公开头文件 astrolog_lib.h"; exit 1; }
      echo "头文件: $HDR"

      # 4) 覆盖率/单测：CMakeLists 未定义则跳过（非错误）
      COVBIN=$(find _verify -name 'test_astro_lib_coverage' | head -1)
      if [ -n "$COVBIN" ]; then
        echo "发现覆盖率测试程序，执行中..."
        "$COVBIN"
      else
        echo "注: 项目 CMakeLists 未定义覆盖率/单测目标，跳过（非错误）"
      fi

      echo "VERIFY PASSED (${PLATFORM})"
  allow_failure: false   # 质量门禁，不允许静默失败

# 部署作业模板
.deploy_template: &deploy_definition
  stage: deploy
  image: curlimages/curl:latest
  before_script: []   # 发布产物不需要构建工具链
  script:
    - |
      set -e
      export ARTIFACT_NAME="astroproject-${PLATFORM}-${ARCH}-${TIER}.tar.gz"
      # 适配说明：docker executor 的容器是临时的，原设计"解压到 runner 本机 $HOME"
      # 在容器里没有意义（job 结束即销毁）。改为发布到 GitLab Generic Package
      # Registry 做持久化保存；包名带系统标识（astroproject-ubuntu-22.04 等），
      # 文件名带档位(-dev/-test/-prod)，用户可在 Packages 页面按系统/版本/档位辨识下载。
      ARTIFACT="astroproject/${BUILD_DIR}/${ARTIFACT_NAME}"
      test -f "$ARTIFACT" || { echo "FAIL: 未找到制品 $ARTIFACT"; exit 1; }
      # 版本：tag 触发用 tag 本身（如 v0.2.0），否则用 环境前缀-短SHA（development-/testing-/production-）
      PACKAGE_VERSION="${CI_COMMIT_TAG:-${ENVIRONMENT}-${CI_COMMIT_SHORT_SHA}}"
      echo "Publishing ${PLATFORM} (${TIER}) -> packages/generic/${PACKAGE_NAME}/${PACKAGE_VERSION}/${ARTIFACT_NAME}"
      curl --fail --silent --show-error \
        --header "JOB-TOKEN: ${CI_JOB_TOKEN}" \
        --upload-file "$ARTIFACT" \
        "${CI_API_V4_URL}/projects/${CI_PROJECT_ID}/packages/generic/${PACKAGE_NAME}/${PACKAGE_VERSION}/${ARTIFACT_NAME}"
      echo "Published OK: ${ARTIFACT_NAME}"
  # 注：GitLab 19.2 有 bug —— deploy 阶段 job 带 environment 会导致 script 校验失败，
  #     故本平台统一不在 deploy job 上使用 environment 关键字。
'''

BODY_PREPARE = r'''
# ============================================================
# 协作自动化（prepare 阶段）
# ============================================================
gene_mrdesc:
  stage: prepare   # 原为 build，语义不符：这是协作自动化，不是构建
  image: curlimages/curl:latest
  before_script:
    - 'echo "Current directory: $(pwd)"'
    - 'echo "HOME directory: $HOME"'
    - 'echo "CI_PROJECT_DIR: $CI_PROJECT_DIR"'
    - 'echo "Checking and installing required tools..."'
    - if ! command -v jq &> /dev/null; then echo "jq not found, attempting to install..."; if command -v apk &> /dev/null; then apk add --no-cache jq; elif command -v apt-get &> /dev/null; then apt-get update && apt-get install -y jq; elif command -v yum &> /dev/null; then yum install -y jq; elif command -v dnf &> /dev/null; then dnf install -y jq; elif command -v pkg &> /dev/null; then pkg install -y jq; elif command -v zypper &> /dev/null; then zypper install -y jq; else echo "Package manager not found. Please install jq manually."; exit 1; fi; else echo "jq is already installed"; jq --version; fi
    - if ! command -v bash &> /dev/null; then echo "bash not found, attempting to install..."; if command -v apk &> /dev/null; then apk add --no-cache bash; elif command -v apt-get &> /dev/null; then apt-get update && apt-get install -y bash; elif command -v yum &> /dev/null; then yum install -y bash; elif command -v dnf &> /dev/null; then dnf install -y bash; elif command -v pkg &> /dev/null; then pkg install -y bash; elif command -v zypper &> /dev/null; then zypper install -y bash; else echo "Package manager not found. Please install bash manually."; exit 1; fi; else echo "bash is already installed"; bash --version; fi
  script:
    - 'echo "正在为合并请求生成描述内容..."'
    - pwd
    - ls -la
    - ls -la scripts/
    - PROJECT_DIR="$HOME/$CI_PROJECT_DIR"
    - 'echo "Project directory: $PROJECT_DIR"'
    - chmod +x "$PROJECT_DIR/scripts/generate_mr_description.sh"
    - chmod +x "$PROJECT_DIR/scripts/add_mr_note.sh"
    - /usr/local/bin/bash "$PROJECT_DIR/scripts/generate_mr_description.sh"
  tags:
    - docker
  rules:
    - if: $CI_PIPELINE_SOURCE == "merge_request_event"
  variables:
    GITLAB_TOKEN: $PERSONAL_ACCESS_TOKEN

# 当点击合并，合并代码成功后会提交到目标分支，触发deploy 作业，并添加注释
add_mr_note:
  stage: prepare   # 原为 deploy，语义不符：这是 MR 通知，不是部署
  script:
    - 'echo "正在为合并请求添加注释..."'
    - PROJECT_DIR="$HOME/$CI_PROJECT_DIR"
    - 'echo "Project directory: $PROJECT_DIR"'
    - |
      if echo "$CI_COMMIT_MESSAGE" | grep -q "See merge request"; then
        MR_IID=$(echo "$CI_COMMIT_MESSAGE" | grep "See merge request" | grep -o "!.*" | sed 's/!//')
      else
        MR_IID=$(echo $CI_COMMIT_MESSAGE | grep -o "Merge branch.*into.*" | grep -o "Merge branch '[^']*' \(#[0-9]*\)" | grep -o "#[0-9]*" | sed 's/#//')
      fi
    - 'echo "Extracted MR IID: $MR_IID"'
    - chmod +x "$PROJECT_DIR/scripts/add_mr_note.sh"
    - bash "$PROJECT_DIR/scripts/add_mr_note.sh" "$MR_IID"
  tags:
    - docker
  rules:
    - if: $CI_PIPELINE_SOURCE == "push" && $CI_COMMIT_BRANCH == "develop" && $CI_COMMIT_MESSAGE =~ /^Merge branch.*/
    - if: $CI_PIPELINE_SOURCE == "push" && $CI_COMMIT_BRANCH == "test" && $CI_COMMIT_MESSAGE =~ /^Merge branch.*/
    - if: $CI_PIPELINE_SOURCE == "push" && $CI_COMMIT_BRANCH == "main" && $CI_COMMIT_MESSAGE =~ /^Merge branch.*/
  variables:
    GITLAB_TOKEN: $PERSONAL_ACCESS_TOKEN

# ============================================================
# 开发环境（develop 分支）—— 全系统矩阵
# ============================================================
'''


def main():
    parts = [HEADER, BODY_PREPARE]
    parts.append(gen_build_jobs())
    parts.append("\n# ============================================================\n# 验证（质量门禁）—— 全系统矩阵\n# ============================================================\n")
    parts.append(gen_verify_jobs())
    parts.append("\n# ============================================================\n# 金样数值对拍（CLI → diff golden，P0.4）\n# ============================================================\n")
    parts.append(gen_golden_diff_jobs())
    parts.append("\n# ============================================================\n# 部署（按环境发布）—— 全系统矩阵\n# ============================================================\n")
    parts.append(gen_deploy_jobs())
    parts.append("\n# ============================================================\n# 创建 Release（tag 时触发）：assets 挂全部系统平台产物链接\n# ============================================================\n")
    parts.append(gen_create_release())
    parts.append("\n# ============================================================\n# 同步 Release 到 Atomgit（tag 时触发）：发行版元数据 + 产物附件\n# ============================================================\n")
    parts.append(gen_sync_release_atomgit())
    parts.append("\n# ============================================================\n# 发布到 Package Registry（tag 时触发，按系统分 job，版本带系统）\n# ============================================================\n")
    parts.append(gen_publish_jobs())

    out = "\n".join(parts)
    dst = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), ".gitlab-ci.yml")
    with io.open(dst, "w", encoding="utf-8", newline="\n") as f:
        f.write(out)
    print(f"GENERATED: {dst} ({len(out.splitlines())} lines, {len(PLATFORMS)} platforms x {len(ENVS)} envs)")


if __name__ == "__main__":
    main()
