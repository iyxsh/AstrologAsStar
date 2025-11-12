#!/bin/bash

# 流水线情况统计脚本
# 该脚本用于统计当前流水线和分支情况，并将统计信息作为注释添加到合并请求中

# 检查是否在 GitLab CI 环境中运行
if [ -z "$CI" ]; then
  echo "错误: 此脚本只能在 GitLab CI 环境中运行"
  exit 1
fi

# 检查必要的环境变量
if [ -z "$CI_PROJECT_ID" ] || [ -z "$CI_API_V4_URL" ]; then
  echo "错误: 缺少必要的 GitLab CI 环境变量"
  exit 1
fi

# 检查 PERSONAL_ACCESS_TOKEN 是否设置
if [ -z "$PERSONAL_ACCESS_TOKEN" ]; then
  echo "错误: 未设置 PERSONAL_ACCESS_TOKEN 环境变量"
  exit 1
fi

# 获取从命令行传递的合并请求 IID
MR_IID=$1

# 生成报告内容
REPORT_CONTENT="## 🚀 流水线执行概览

### 📋 流水线基本信息

| 项目 | 流水线 ID | 状态 | 触发源 |
|------|-----------|------|--------|
| $CI_PROJECT_NAME | $CI_PIPELINE_ID | ${CI_PIPELINE_STATUS:-未知} | $CI_PIPELINE_SOURCE |

### 🌳 分支信息

| 当前分支 | 默认分支 | 标签 | 合并请求 IID |
|----------|----------|------|---------------|
"

if [ -n "$CI_COMMIT_TAG" ]; then
  REPORT_CONTENT+="| $CI_COMMIT_TAG | $CI_DEFAULT_BRANCH | 是 | 无 |
"
elif [ -n "$CI_COMMIT_BRANCH" ]; then
  if [ -n "$MR_IID" ]; then
    REPORT_CONTENT+="| $CI_COMMIT_BRANCH | $CI_DEFAULT_BRANCH | 否 | $MR_IID |
"
  else
    REPORT_CONTENT+="| $CI_COMMIT_BRANCH | $CI_DEFAULT_BRANCH | 否 | 无法提取 |
"
  fi
else
  if [ -n "$MR_IID" ]; then
    REPORT_CONTENT+="| 未知 | $CI_DEFAULT_BRANCH | 未知 | $MR_IID |
"
  else
    REPORT_CONTENT+="| 未知 | $CI_DEFAULT_BRANCH | 未知 | 无法提取 |
"
  fi
fi

REPORT_CONTENT+="

### 📦 提交信息

| 提交 SHA | 提交作者 | 提交时间 | 提交信息 |
|----------|----------|----------|----------|
| $CI_COMMIT_SHORT_SHA | $CI_COMMIT_AUTHOR | ${CI_COMMIT_TIMESTAMP:-无} | ${CI_COMMIT_MESSAGE%%$'\n'*} |

### 👤 触发用户信息

| 用户名 | 用户邮箱 | 用户 ID |
|--------|----------|---------|
| ${GITLAB_USER_NAME:-未知} | ${GITLAB_USER_EMAIL:-未知} | ${GITLAB_USER_ID:-未知} |

### 🏗️ 构建环境信息

| 运行器描述 | 运行器标签 | 并发任务数 |
|------------|------------|------------|
| ${CI_RUNNER_DESCRIPTION:-未知} | ${CI_RUNNER_TAGS:-无} | ${CI_CONCURRENT_PROJECT_ID:-无} |

### 📁 项目信息

| 项目 ID | 项目 URL | 项目命名空间 |
|---------|----------|--------------|
| $CI_PROJECT_ID | $CI_PROJECT_URL | $CI_PROJECT_NAMESPACE |

### 🕒 时间信息

| 流水线创建时间 | 作业开始时间 | 时区 |
|----------------|--------------|------|
| ${CI_PIPELINE_CREATED_AT:-无} | ${CI_JOB_STARTED_AT:-无} | ${TZ:-无} |

### 📝 说明

此报告由 CI/CD 流水线自动生成，用于展示当前流水线执行情况和分支信息。

自动生成时间: $(date '+%Y-%m-%d %H:%M:%S')"

# 检查 MR_IID 是否存在
if [ -n "$MR_IID" ]; then
  echo "正在为合并请求 #$MR_IID 添加注释..."
  
  # 转义特殊字符以用于 JSON
  ESCAPED_COMMENT=$(printf '%s' "$REPORT_CONTENT" | jq -sR .)
  
  # 使用 GitLab API 添加注释到合并请求
  RESPONSE=$(curl --request POST \
    --header "PRIVATE-TOKEN: $PERSONAL_ACCESS_TOKEN" \
    --header "Content-Type: application/json" \
    --data "{\"body\": $ESCAPED_COMMENT}" \
    "$CI_API_V4_URL/projects/$CI_PROJECT_ID/merge_requests/$MR_IID/notes")
  
  # 检查响应结果
  if echo "$RESPONSE" | grep -q '"id"'; then
    echo "成功为合并请求 #$MR_IID 添加注释"
  else
    echo "添加注释失败"
    echo "响应内容: $RESPONSE"
    exit 1
  fi
else
  # 如果无法提取 MR_IID，则直接输出报告内容
  echo "无法提取合并请求 IID，直接输出报告内容:"
  echo "$REPORT_CONTENT"
fi