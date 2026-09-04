#!/usr/bin/env bash
# build-builder-images.sh — 在 n8n VM 上批量构建 builder-* 预构建工具链镜像（加速 CI）
# ============================================================
# 背景：CI 每个 job 全新容器，6 平台每次 apt/apk/dnf 现场重装工具链
#       （ubuntu-22.04 实测 ~400s）。把工具链预构建进 builder-* 镜像后，
#       runner(pull_policy=if-not-present) 直接复用本地镜像零拉取，
#       before_script 检测 cmake 存在 -> 整段跳过。
#
# 用法（在宿主机 git-bash 执行，会 ssh 到 n8n VM）：
#   bash scripts/build-builder-images.sh                 # 全量 8 平台
#   bash scripts/build-builder-images.sh ubuntu-22.04    # 只重建单平台
#   N8N_HOST=192.168.56.11 N8N_KEY=... bash scripts/build-builder-images.sh
#
# 镜像 tag = builder-<平台 id>（.gitlab-ci.yml 的 BUILD_IMAGE 引用同名，无 registry 前缀）
# Dockerfile 与 .gitlab-ci.yml default.before_script 的镜像源 sed 保持同源逻辑。
# ============================================================
set -euo pipefail

N8N_KEY="${N8N_KEY:-E:/data/gitCode/AIO/devops-infra/.vagrant/machines/n8n/hyperv/private_key}"
N8N_USER="${N8N_USER:-vagrant}"
N8N_HOST="${N8N_HOST:-192.168.56.11}"
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
DOCKER_DIR="$ROOT/docker"
REMOTE_DIR=/tmp/builder-img
ONLY="${1:-ALL}"   # ALL 或单平台 id

# ---- 平台表: tag=<平台id>  Dockerfile=<文件名>  desc=<说明> ----
declare -A DF
declare -A DESC
for p in ubuntu-22.04 ubuntu-24.04 debian-12 alpine-3.20 rockylinux-9 fedora-40 windows android; do
  DF["$p"]="Dockerfile.builder-$p"
done
DESC[ubuntu-22.04]="apt(TUNA) cmake/make/g++"
DESC[ubuntu-24.04]="apt-deb822(TUNA) cmake/make/g++"
DESC[debian-12]="apt-deb822(TUNA) cmake/make/g++"
DESC[alpine-3.20]="apk(TUNA) cmake/make/g++"
DESC[rockylinux-9]="dnf(USTC) cmake/make/gcc-c++"
DESC[fedora-40]="dnf(aliyun) cmake/make/gcc-c++"
DESC[windows]="ubuntu22+mingw-w64 交叉"
DESC[android]="ubuntu22+wget/unzip (NDK 在宿主卷)"

# ---- 仅单平台时过滤 ----
if [ "$ONLY" != "ALL" ]; then
  if [ -z "${DF[$ONLY]:-}" ]; then echo "未知平台: $ONLY（可选: ${!DF[*]}）"; exit 1; fi
  BUILD_LIST="$ONLY"
else
  BUILD_LIST="${!DF[@]}"
fi

echo ">>> 目标 VM: ${N8N_USER}@${N8N_HOST}  key=${N8N_KEY}"
echo ">>> 待构建平台: ${BUILD_LIST}"

ssh -i "$N8N_KEY" -o StrictHostKeyChecking=no -o BatchMode=yes -o ConnectTimeout=15 \
    "${N8N_USER}@${N8N_HOST}" "mkdir -p $REMOTE_DIR && sudo rm -rf $REMOTE_DIR/* 2>/dev/null; echo remote-ready"

# 上传 Dockerfile（scp 到远端并统一命名）
for p in $BUILD_LIST; do
  f="$DOCKER_DIR/${DF[$p]}"
  [ -f "$f" ] || { echo "!! 缺少 $f"; exit 1; }
  scp -q -i "$N8N_KEY" -o StrictHostKeyChecking=no -o BatchMode=yes "$f" \
      "${N8N_USER}@${N8N_HOST}:$REMOTE_DIR/Dockerfile.builder-$p"
  echo "  上传 ${DF[$p]}"
done

# 远端循环 docker build（sudo：vagrant 用户需 sudo docker）
ssh -i "$N8N_KEY" -o StrictHostKeyChecking=no -o BatchMode=yes -o ConnectTimeout=15 \
    "${N8N_USER}@${N8N_HOST}" "cd $REMOTE_DIR && for p in $BUILD_LIST; do
  echo \"========== builder-\$p ==========\"
  if sudo docker build -f Dockerfile.builder-\$p -t builder-\$p . > /tmp/build-\$p.log 2>&1; then
    echo \"OK   builder-\$p  (size: \$(sudo docker image inspect builder-\$p --format '{{.Size}}' | awk '{printf \"%.1fMB\", \$1/1048576}'))\"
  else
    echo \"FAIL builder-\$p —— 日志尾部:\"; tail -25 /tmp/build-\$p.log; exit 1
  fi
done
echo '===== docker images (builder-*) ====='
sudo docker images --format '{{.Repository}}:{{.Tag}}\t{{.Size}}' | grep '^builder-' | sort"

echo ">>> 全部完成。CI 的 BUILD_IMAGE 现指向本地 builder-* 镜像（pull_policy=if-not-present 直命中）。"
