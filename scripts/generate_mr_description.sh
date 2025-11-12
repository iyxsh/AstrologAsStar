#!/bin/bash

# 合并请求描述自动生成脚本

# 获取合并请求 ID（如果在合并请求流水线中运行）
if [ -n "$CI_MERGE_REQUEST_IID" ]; then
  MR_IID=$CI_MERGE_REQUEST_IID
elif [ -n "$1" ]; then
  MR_IID=$1
else
  echo "请提供合并请求 IID"
  exit 1
fi

echo "调试信息: MR_IID = $MR_IID"
echo "调试信息: PROJECT_ID = $CI_PROJECT_ID"
echo "调试信息: PROJECT_PATH = $CI_PROJECT_PATH"
echo "调试信息: PROJECT_URL = $CI_PROJECT_URL"
echo "调试信息: GITLAB_URL = $CI_API_V4_URL"

# 获取项目信息
PROJECT_ID=$CI_PROJECT_ID
PROJECT_PATH=$CI_PROJECT_PATH
GITLAB_URL=$CI_API_V4_URL

# 调试信息：检查环境变量
echo "调试信息: CI_JOB_TOKEN 长度: ${#CI_JOB_TOKEN}"
echo "调试信息: PERSONAL_ACCESS_TOKEN 长度: ${#PERSONAL_ACCESS_TOKEN}"
echo "调试信息: GITLAB_TOKEN 长度: ${#GITLAB_TOKEN}"

# 直接使用 PERSONAL_ACCESS_TOKEN，类似于 create_release 作业
if [ -n "$PERSONAL_ACCESS_TOKEN" ] && [ ${#PERSONAL_ACCESS_TOKEN} -gt 0 ]; then
  AUTH_HEADER="PRIVATE-TOKEN: $PERSONAL_ACCESS_TOKEN"
  echo "使用 PERSONAL_ACCESS_TOKEN 进行认证"
elif [ -n "$GITLAB_TOKEN" ] && [ ${#GITLAB_TOKEN} -gt 0 ]; then
  AUTH_HEADER="PRIVATE-TOKEN: $GITLAB_TOKEN"
  echo "使用 GITLAB_TOKEN 进行认证"
else
  echo "错误: 未设置 PERSONAL_ACCESS_TOKEN 或 GITLAB_TOKEN 环境变量"
  echo "请在 GitLab 项目的 Settings > CI/CD > Variables 中添加 PERSONAL_ACCESS_TOKEN 变量"
  echo "或者检查变量的可见性设置（Protected/Unprotected）"
  exit 1
fi

# 检查 curl 和 jq 是否可用
if ! command -v curl &> /dev/null; then
  echo "错误: curl 命令不可用，请确保 curl 已安装"
  exit 1
fi

if ! command -v jq &> /dev/null; then
  echo "错误: jq 命令不可用，请确保 jq 已安装"
  exit 1
fi

# 获取合并请求信息 - 尝试多种方式
echo "调试信息: 正在获取合并请求信息"

# 方式1: 使用项目ID
echo "使用项目ID方式请求URL: $GITLAB_URL/projects/$PROJECT_ID/merge_requests/$MR_IID"
MR_INFO_1=$(curl --header "$AUTH_HEADER" \
  "$GITLAB_URL/projects/$PROJECT_ID/merge_requests/$MR_IID")
echo "调试信息: MR_INFO_1 响应:"
echo "$MR_INFO_1"

# 方式2: 使用URL编码的项目路径
ENCODED_PROJECT_PATH=$(echo "$PROJECT_PATH" | sed 's/\//%2F/g')
echo "编码后的项目路径: $ENCODED_PROJECT_PATH"
echo "使用项目路径方式请求URL: $GITLAB_URL/projects/$ENCODED_PROJECT_PATH/merge_requests/$MR_IID"
MR_INFO_2=$(curl --header "$AUTH_HEADER" \
  "$GITLAB_URL/projects/$ENCODED_PROJECT_PATH/merge_requests/$MR_IID")
echo "调试信息: MR_INFO_2 响应:"
echo "$MR_INFO_2"

# 选择有效的响应
if echo "$MR_INFO_1" | grep -q '"id"'; then
  MR_INFO=$MR_INFO_1
  ACCESS_METHOD="PROJECT_ID"
  echo "使用项目ID方式获取数据成功"
elif echo "$MR_INFO_2" | grep -q '"id"'; then
  MR_INFO=$MR_INFO_2
  ACCESS_METHOD="PROJECT_PATH"
  echo "使用项目路径方式获取数据成功"
else
  # 如果两种方式都失败，尝试使用项目URL中的项目名
  PROJECT_NAME_FROM_URL=$(echo "$CI_PROJECT_URL" | sed 's|.*/||')
  ENCODED_PROJECT_NAME=$(echo "$PROJECT_NAME_FROM_URL" | sed 's/\//%2F/g')
  echo "从项目URL提取的项目名: $PROJECT_NAME_FROM_URL"
  echo "编码后的项目名: $ENCODED_PROJECT_NAME"
  echo "使用项目名方式请求URL: $GITLAB_URL/projects/$ENCODED_PROJECT_NAME/merge_requests/$MR_IID"
  MR_INFO_3=$(curl --header "$AUTH_HEADER" \
    "$GITLAB_URL/projects/$ENCODED_PROJECT_NAME/merge_requests/$MR_IID")
  echo "调试信息: MR_INFO_3 响应:"
  echo "$MR_INFO_3"
  
  if echo "$MR_INFO_3" | grep -q '"id"'; then
    MR_INFO=$MR_INFO_3
    ACCESS_METHOD="PROJECT_NAME"
    echo "使用项目名方式获取数据成功"
  else
    echo "错误: 无法通过任何方式获取合并请求信息"
    echo "MR_INFO_1: $MR_INFO_1"
    echo "MR_INFO_2: $MR_INFO_2"
    echo "MR_INFO_3: $MR_INFO_3"
    
    # 检查是否是权限问题
    if echo "$MR_INFO_1" | grep -q "401 Unauthorized" || \
       echo "$MR_INFO_2" | grep -q "401 Unauthorized" || \
       echo "$MR_INFO_3" | grep -q "401 Unauthorized"; then
      echo "关键问题: 访问返回 401 Unauthorized，这通常意味着:"
      echo "1. 认证令牌不正确或已过期"
      echo "2. 认证令牌没有足够权限"
      echo "3. 项目访问权限不足"
      echo ""
      echo "当前使用的认证方式: $AUTH_HEADER"
      echo "请检查以下事项:"
      echo "- 确保 PERSONAL_ACCESS_TOKEN 设置正确"
      echo "- 确保令牌具有对项目操作的足够权限"
      echo "- 确保 Runner 具有适当的项目访问权限"
    fi
    exit 1
  fi
fi

# 检查是否成功获取合并请求信息
if echo "$MR_INFO" | grep -q "401 Unauthorized"; then
  echo "错误: GitLab API 身份验证失败"
  exit 1
fi

# 提取合并请求信息
TARGET_BRANCH=$(echo $MR_INFO | jq -r '.target_branch')
SOURCE_BRANCH=$(echo $MR_INFO | jq -r '.source_branch')
MR_TITLE=$(echo $MR_INFO | jq -r '.title')
MR_AUTHOR=$(echo $MR_INFO | jq -r '.author.name')
MR_CREATED_AT=$(echo $MR_INFO | jq -r '.created_at' | cut -d'T' -f1)
MR_DESCRIPTION=$(echo $MR_INFO | jq -r '.description')
MR_STATE=$(echo $MR_INFO | jq -r '.state')
MR_WEB_URL=$(echo $MR_INFO | jq -r '.web_url')
MR_UPVOTES=$(echo $MR_INFO | jq -r '.upvotes')
MR_DOWNVOTES=$(echo $MR_INFO | jq -r '.downvotes')
MR_LABELS=$(echo $MR_INFO | jq -r '.labels | join(", ")')
MR_MILESTONE=$(echo $MR_INFO | jq -r '.milestone.title')
MR_ASSIGNEES=$(echo $MR_INFO | jq -r '[.assignees[].name] | join(", ")')
MR_REVIEWERS=$(echo $MR_INFO | jq -r '[.reviewers[].name] | join(", ")')

echo "正在处理合并请求 #$MR_IID"
echo "标题: $MR_TITLE"
echo "作者: $MR_AUTHOR"
echo "创建日期: $MR_CREATED_AT"
echo "源分支: $SOURCE_BRANCH"
echo "目标分支: $TARGET_BRANCH"

# 获取提交列表 - 使用与获取MR信息相同的方式
echo "调试信息: 正在获取提交列表"
case $ACCESS_METHOD in
  "PROJECT_ID")
    COMMITS=$(curl --header "$AUTH_HEADER" \
      "$GITLAB_URL/projects/$PROJECT_ID/merge_requests/$MR_IID/commits")
    ;;
  "PROJECT_PATH")
    COMMITS=$(curl --header "$AUTH_HEADER" \
      "$GITLAB_URL/projects/$ENCODED_PROJECT_PATH/merge_requests/$MR_IID/commits")
    ;;
  "PROJECT_NAME")
    COMMITS=$(curl --header "$AUTH_HEADER" \
      "$GITLAB_URL/projects/$ENCODED_PROJECT_NAME/merge_requests/$MR_IID/commits")
    ;;
esac

echo "调试信息: COMMITS 响应状态检查"

# 检查是否成功获取提交信息
if echo "$COMMITS" | grep -q "401 Unauthorized"; then
  echo "错误: GitLab API 身份验证失败"
  exit 1
fi

# 计算提交数量
COMMIT_COUNT=$(echo $COMMITS | jq length)
echo "发现 $COMMIT_COUNT 个提交"

# 分析提交类型（基于 conventional commits 规范）
FEATURE_COUNT=0
FIX_COUNT=0
DOCS_COUNT=0
STYLE_COUNT=0
REFACTOR_COUNT=0
TEST_COUNT=0
CHORE_COUNT=0
OTHER_COUNT=0

# 临时文件用于存储提交类型
TEMP_FILE="/tmp/commit_types_$$.txt"
> "$TEMP_FILE"  # 清空或创建文件

echo $COMMITS | jq -c '.[]' | while IFS= read -r commit; do
  COMMIT_MESSAGE=$(echo $commit | jq -r '.message' | head -n1 | tr '[:upper:]' '[:lower:]')
  
  if [[ $COMMIT_MESSAGE == feat:* ]]; then
    echo "feat" >> "$TEMP_FILE"
  elif [[ $COMMIT_MESSAGE == fix:* ]]; then
    echo "fix" >> "$TEMP_FILE"
  elif [[ $COMMIT_MESSAGE == docs:* ]]; then
    echo "docs" >> "$TEMP_FILE"
  elif [[ $COMMIT_MESSAGE == style:* ]]; then
    echo "style" >> "$TEMP_FILE"
  elif [[ $COMMIT_MESSAGE == refactor:* ]]; then
    echo "refactor" >> "$TEMP_FILE"
  elif [[ $COMMIT_MESSAGE == test:* ]]; then
    echo "test" >> "$TEMP_FILE"
  elif [[ $COMMIT_MESSAGE == chore:* ]]; then
    echo "chore" >> "$TEMP_FILE"
  else
    echo "other" >> "$TEMP_FILE"
  fi
done

# 统计各类型提交数量
FEATURE_COUNT=$(grep -c "^feat$" "$TEMP_FILE" || echo "0")
FIX_COUNT=$(grep -c "^fix$" "$TEMP_FILE" || echo "0")
DOCS_COUNT=$(grep -c "^docs$" "$TEMP_FILE" || echo "0")
STYLE_COUNT=$(grep -c "^style$" "$TEMP_FILE" || echo "0")
REFACTOR_COUNT=$(grep -c "^refactor$" "$TEMP_FILE" || echo "0")
TEST_COUNT=$(grep -c "^test$" "$TEMP_FILE" || echo "0")
CHORE_COUNT=$(grep -c "^chore$" "$TEMP_FILE" || echo "0")
OTHER_COUNT=$(grep -c "^other$" "$TEMP_FILE" || echo "0")

# 清理临时文件
rm -f "$TEMP_FILE"

# 生成描述内容
DESCRIPTION="## 合并请求概述

- **标题**: $MR_TITLE
- **作者**: $MR_AUTHOR
- **状态**: $MR_STATE
- **创建日期**: $MR_CREATED_AT
- **源分支**: $SOURCE_BRANCH
- **目标分支**: $TARGET_BRANCH
- **提交总数**: $COMMIT_COUNT
- **赞成票**: $MR_UPVOTES
- **反对票**: $MR_DOWNVOTES

### 标签和里程碑

- **标签**: ${MR_LABELS:-无}
- **里程碑**: ${MR_MILESTONE:-无}

### 分配和审查

- **分配给**: ${MR_ASSIGNEES:-无}
- **审查者**: ${MR_REVIEWERS:-无}

### 提交类型统计

| 类型 | 数量 | 说明 |
|------|------|------|
| feat | $FEATURE_COUNT | 新功能 |
| fix | $FIX_COUNT | 修复 |
| docs | $DOCS_COUNT | 文档更新 |
| style | $STYLE_COUNT | 代码格式调整 |
| refactor | $REFACTOR_COUNT | 代码重构 |
| test | $TEST_COUNT | 测试相关 |
| chore | $CHORE_COUNT | 构建过程或辅助工具的变动 |
| other | $OTHER_COUNT | 其他类型 |

"

# 移除提交详情部分，不再包含具体的提交信息

# 添加文件变更统计 - 使用与获取MR信息相同的方式
echo "调试信息: 正在获取文件变更统计"
case $ACCESS_METHOD in
  "PROJECT_ID")
    CHANGES=$(curl --header "$AUTH_HEADER" \
      "$GITLAB_URL/projects/$PROJECT_ID/merge_requests/$MR_IID/changes")
    ;;
  "PROJECT_PATH")
    CHANGES=$(curl --header "$AUTH_HEADER" \
      "$GITLAB_URL/projects/$ENCODED_PROJECT_PATH/merge_requests/$MR_IID/changes")
    ;;
  "PROJECT_NAME")
    CHANGES=$(curl --header "$AUTH_HEADER" \
      "$GITLAB_URL/projects/$ENCODED_PROJECT_NAME/merge_requests/$MR_IID/changes")
    ;;
esac

if [ $? -eq 0 ]; then
  CHANGED_FILES=$(echo "$CHANGES" | jq '.changes | length')
  ADDITIONS=$(echo "$CHANGES" | jq '[.changes[].diff] | map(select(. != null)) | join("") | split("\n+") | length')
  DELETIONS=$(echo "$CHANGES" | jq '[.changes[].diff] | map(select(. != null)) | join("") | split("\n-") | length')
  
  DESCRIPTION+="### 文件变更统计

- **变更文件数**: $CHANGED_FILES
- **新增行数**: $ADDITIONS
- **删除行数**: $DELETIONS

"
fi

# 添加提交详情说明（已简化）
DESCRIPTION+="
## 详细变更

如有需要请在当前页面的提交下查看每个提交的详细信息以了解具体变更内容。

## 测试情况

请查看流水线中的测试结果和测试报告，确保所有测试用例均通过。

## 其他说明

> ⚠️ 该描述由自动化脚本生成，基于合并请求中的所有提交。如需更改此页面显示的内容，请根据实际情况进行补充和修改。

自动生成时间: $(date '+%Y-%m-%d %H:%M:%S') (流水线 ID: $CI_PIPELINE_ID)
"

echo "调试信息: 准备注释描述"
echo "调试信息: 描述内容:"
echo "$DESCRIPTION"

# 转义特殊字符
ESCAPED_DESCRIPTION=$(printf '%s' "$DESCRIPTION" | jq -sR .)

echo "调试信息: 转义后的描述内容:"
echo "$ESCAPED_DESCRIPTION"

# 更新合并请求描述 - 使用与获取MR信息相同的方式
echo "调试信息: 正在更新合并请求描述"
case $ACCESS_METHOD in
  "PROJECT_ID")
    echo "使用项目ID方式请求URL: $GITLAB_URL/projects/$PROJECT_ID/merge_requests/$MR_IID"
    RESPONSE=$(curl --request PUT \
      --header "$AUTH_HEADER" \
      --header "Content-Type: application/json" \
      --data "{\"description\": $ESCAPED_DESCRIPTION}" \
      "$GITLAB_URL/projects/$PROJECT_ID/merge_requests/$MR_IID")
    ;;
  "PROJECT_PATH")
    echo "使用项目路径方式请求URL: $GITLAB_URL/projects/$ENCODED_PROJECT_PATH/merge_requests/$MR_IID"
    RESPONSE=$(curl --request PUT \
      --header "$AUTH_HEADER" \
      --header "Content-Type: application/json" \
      --data "{\"description\": $ESCAPED_DESCRIPTION}" \
      "$GITLAB_URL/projects/$ENCODED_PROJECT_PATH/merge_requests/$MR_IID")
    ;;
  "PROJECT_NAME")
    echo "使用项目名方式请求URL: $GITLAB_URL/projects/$ENCODED_PROJECT_NAME/merge_requests/$MR_IID"
    RESPONSE=$(curl --request PUT \
      --header "$AUTH_HEADER" \
      --header "Content-Type: application/json" \
      --data "{\"description\": $ESCAPED_DESCRIPTION}" \
      "$GITLAB_URL/projects/$ENCODED_PROJECT_NAME/merge_requests/$MR_IID")
    ;;
esac

echo "调试信息: API 响应:"
echo "$RESPONSE"

# 检查响应中的错误信息
if echo "$RESPONSE" | grep -q "401 Unauthorized"; then
  echo "错误: GitLab API 身份验证失败"
  echo "当前使用的认证方式: $AUTH_HEADER"
  echo "请检查令牌是否正确配置且具有足够权限"
  exit 1
elif echo "$RESPONSE" | grep -q "403 Forbidden"; then
  echo "警告: 没有权限更新合并请求描述，需要具有 Maintainer 权限"
  exit 0
elif echo "$RESPONSE" | grep -q "message.*403"; then
  echo "警告: 没有权限更新合并请求描述，需要具有 Maintainer 权限"
  exit 0
fi

if echo "$RESPONSE" | grep -q "id"; then
  echo "合并请求描述已更新成功"
else
  echo "合并请求描述更新失败"
  echo "响应内容: $RESPONSE"
fi