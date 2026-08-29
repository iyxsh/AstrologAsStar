#!/bin/sh
# sync-atomgit-release.sh — 将 GitLab Release 同步到 Atomgit（Release 元数据 + 构建产物附件）
#
# 背景：GitLab 的 Release（发行版）是平台元数据，push mirror 只同步 git 数据，
#       Atomgit 的 Releases 页默认是空的。本脚本在 tag 流水线的 release 阶段执行，
#       用 Atomgit v5 API（https://api.atomgit.com）创建/更新同名 Release 并上传产物附件。
#
# 依赖环境变量（由 GitLab CI 提供或项目 CI/CD Variables 注入）：
#   ATOMGIT_PAT      必填，Atomgit 个人访问令牌（组织仓库需有写权限）
#   ATOMGIT_OWNER    可选，默认 iyxsh
#   ATOMGIT_REPO     可选，默认 AstrologAsStar
#   CI_COMMIT_TAG    必填，当前 tag 名（如 v0.1.2）
#
# 产物：从 build 阶段 job artifacts 获取（dependencies/needs），位于 astroproject/build/astroproject-*.tar.gz
# 幂等：Release 已存在则 PATCH 更新；同名附件已存在则跳过
set -e

: "${ATOMGIT_PAT:?需要 ATOMGIT_PAT 环境变量}"
: "${CI_COMMIT_TAG:?需要 CI_COMMIT_TAG}"
ATOMGIT_OWNER="${ATOMGIT_OWNER:-iyxsh}"
ATOMGIT_REPO="${ATOMGIT_REPO:-AstrologAsStar}"
API="https://api.atomgit.com/api/v5/repos/${ATOMGIT_OWNER}/${ATOMGIT_REPO}"
AUTH="access_token=${ATOMGIT_PAT}"
TAG="${CI_COMMIT_TAG}"

echo "==> 同步 Release ${TAG} 到 Atomgit ${ATOMGIT_OWNER}/${ATOMGIT_REPO}"

# 1. 构造 Release body（用 jq 生成 JSON，避免引号/换行陷阱）
BODY=$(jq -n \
  --arg tag "$TAG" \
  --arg name "Release $TAG" \
  --arg body "AstroProject $TAG 正式版（多系统矩阵 6 平台：ubuntu-22.04/24.04、debian-12、alpine-3.20、rockylinux-9、fedora-40）。产物为静态库 + include 树 + MANIFEST，由 GitLab CI 构建并同步。功能一致性：与旧预编译版本输出逐字节一致。" \
  '{tag_name: $tag, name: $name, body: $body, release_status: "latest"}')

# 2. Release 已存在则 PATCH，否则 POST
EXIST=$(curl -sf "${API}/releases?${AUTH}&per_page=100" | jq -r --arg t "$TAG" '.[] | select(.tag_name == $t) | .tag_name' | head -1)
if [ -z "$EXIST" ]; then
  echo "==> 创建 Release"
  curl -sf -X POST "${API}/releases?${AUTH}" -H "Content-Type: application/json" -d "$BODY" > /dev/null
else
  echo "==> Release 已存在，PATCH 更新"
  curl -sf -X PATCH "${API}/releases/${TAG}?${AUTH}" -H "Content-Type: application/json" -d "$BODY" > /dev/null
fi

# 3. 逐个上传产物附件（OBS 直传：GET upload_url → PUT）
for f in astroproject/build/astroproject-*.tar.gz; do
  [ -f "$f" ] || { echo "  [跳过] 无匹配产物: $f"; continue; }
  fname=$(basename "$f")
  HAVE=$(curl -sf "${API}/releases?${AUTH}&per_page=100" | jq -r --arg t "$TAG" --arg n "$fname" \
    '.[] | select(.tag_name == $t) | .assets[]? | select(.name == $n) | .name' | head -1)
  if [ -n "$HAVE" ]; then
    echo "  [跳过] 附件已存在: $fname"
    continue
  fi
  echo "  [上传] $fname"
  UP=$(curl -sf "${API}/releases/${TAG}/upload_url?${AUTH}&file_name=${fname}")
  URL=$(echo "$UP" | jq -r '.url')
  H_PROJECT=$(echo "$UP" | jq -r '.headers["x-obs-meta-project-id"]')
  H_ACL=$(echo "$UP" | jq -r '.headers["x-obs-acl"]')
  H_CB=$(echo "$UP" | jq -r '.headers["x-obs-callback"]')
  curl -sf -X PUT "$URL" \
    -H "x-obs-meta-project-id: $H_PROJECT" \
    -H "x-obs-acl: $H_ACL" \
    -H "x-obs-callback: $H_CB" \
    -H "Content-Type: application/octet-stream" \
    --data-binary "@$f" > /dev/null
  echo "  [完成] $fname"
done

echo "==> Atomgit Release ${TAG} 同步完成"
