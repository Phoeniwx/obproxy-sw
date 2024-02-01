/**
 * Copyright (c) 2021 OceanBase
 * OceanBase Database Proxy(ODP) is licensed under Mulan PubL v2.
 * You can use this software according to the terms and conditions of the Mulan PubL v2.
 * You may obtain a copy of Mulan PubL v2 at:
 *          http://license.coscl.org.cn/MulanPubL-2.0
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PubL v2 for more details.
 */

#include "ob_async_log_struct.h"
#include "lib/objectpool/ob_concurrency_objpool.h"

using namespace tbutil;

namespace oceanbase
{
namespace common
{
int64_t ObLogItemFactory::alloc_count_[MAX_LOG_ITEM_TYPE] = {0};
int64_t ObLogItemFactory::release_count_[MAX_LOG_ITEM_TYPE] = {0};

ObLogItem::ObLogItem()
: item_type_(MAX_LOG_ITEM_TYPE), header_pos_(0), pos_(0), header_(),
  buf_((char *)(this) + sizeof(ObLogItem))
{
}

void ObLogItem::set_header(const timeval tv, const int32_t level, const ObLogFDType type,
                  const uint64_t trace_id_0, const uint64_t trace_id_1, const char *mod_name,
                  const char *file, const int32_t line, const char* function, const uint64_t dropped_log_count,
                  const int64_t tid)
{
  header_.timestamp_ = static_cast<int64_t>(tv.tv_sec) * static_cast<int64_t>(1000000) + static_cast<int64_t>(tv.tv_usec);
  header_.log_level_ = level;
  header_.fd_type_ = type;
  header_.trace_id_0_ = trace_id_0;
  header_.trace_id_1_ = trace_id_1;
  header_.mod_name_ = mod_name;
  header_.file_name_ = file;
  header_.line_ = line;
  header_.function_name_ = function;
  header_.dropped_log_count_ = dropped_log_count;
  header_.tid_ = tid;
}

void ObLogItem::ObLogItemHeader::reset()
{
  fd_type_ = MAX_FD_FILE;
  log_level_ = OB_LOG_LEVEL_NONE;
  timestamp_ = 0;
  mod_name_ = NULL;
  file_name_ = NULL;
  function_name_ = NULL;
  line_ = 0;
  trace_id_0_ = 0;
  trace_id_1_ = 0;
  dropped_log_count_ = 0;
  tid_ = 0;
}
void ObLogItem::reuse()
{
  header_pos_ = 0;
  pos_ = 0;
  header_.reset();
  //not not reset item_type_ and buf_
}

void ObLogItem::deep_copy_header_only(const ObLogItem &other)
{
  //can not set item_type_ and buf_
  header_ = other.header_;
  header_pos_ = other.get_header_len();
  pos_ = other.get_header_len();//use header pos
  memcpy(buf_, other.get_buf(), other.get_header_len());
}

ObLogItem *ObLogItemFactory::alloc(const int32_t type)
{
  ObLogItem *item = NULL;
  if (type >= 0
      && type < static_cast<int32_t>(MAX_LOG_ITEM_TYPE)
      && LOG_ITEM_SIZE[type] > sizeof(ObLogItem)) {
    void *ptr = NULL;
    if (NULL == (ptr = ob_malloc(LOG_ITEM_SIZE[type], ObModIds::OB_ASYNC_LOG_BUFFER))) {
      LOG_STDERR("ob malloc ObLogItem error. ptr = %p\n", ptr);
    } else {
      item = new(ptr)ObLogItem();
      item->set_item_type(static_cast<ObLogItemType>(type));
      (void)ATOMIC_FAA(&alloc_count_[type], 1);
    }
  }
  return item;
}

void ObLogItemFactory::release(ObLogItem *item)
{
  if (NULL == item) {
    LOG_STDERR("ObLogItem is NULL. item = %p\n", item);
  } else {
    const ObLogItemType type = item->get_item_type();
    ob_free(item);
    item = NULL;
    if (type < MAX_LOG_ITEM_TYPE) {
      (void)ATOMIC_FAA(&release_count_[type], 1);
    } else {
      LOG_STDERR("unknown log item type. type = %d\n", type);
    }
  }
}


ObLogFileStruct::ObLogFileStruct()
  : fd_(STDERR_FILENO), wf_fd_(STDERR_FILENO), write_count_(0), write_size_(0),
    file_size_(0), open_wf_flag_(false), enable_wf_flag_(false)
{
  filename_[0] = '\0';
  MEMSET(&stat_, 0, sizeof(stat_));
  MEMSET(&wf_stat_, 0, sizeof(wf_stat_));
}

int ObLogFileStruct::open(const char *file_name, const bool open_wf_flag, const bool redirect_flag)
{
  int ret = OB_SUCCESS;
  size_t fname_len = 0;
  if (OB_UNLIKELY(is_opened())) {
    LOG_STDERR("log_file is opened already fname = %s log_file = %s\n", filename_, file_name);
    ret = OB_ERR_UNEXPECTED;
  } else if (OB_ISNULL(file_name)) {
    LOG_STDERR("invalid argument log_file = %p\n", file_name);
    ret = OB_INVALID_ARGUMENT;
  //need care .wf.??
  } else if (OB_UNLIKELY((fname_len = strlen(file_name)) > MAX_LOG_FILE_NAME_SIZE - 5)) {
    LOG_STDERR("fname' size is overflow, log_file = %p\n", file_name);
    ret = OB_SIZE_OVERFLOW;
  } else {
    MEMCPY(filename_, file_name, fname_len);
    filename_[fname_len] = '\0';
    if (OB_FAIL(reopen(redirect_flag))) {
      LOG_STDERR("reopen error, ret= %d\n", ret);
    } else if (open_wf_flag && OB_FAIL(reopen_wf())) {
      LOG_STDERR("reopen_wf error, ret= %d\n", ret);
    } else {
      //open wf file
      open_wf_flag_ = open_wf_flag;
      enable_wf_flag_ = open_wf_flag;
    }
  }
  return ret;
}


int ObLogFileStruct::reopen(const bool redirect_flag)
{
  int ret = OB_SUCCESS;
  int32_t tmp_fd = -1;
  if (OB_UNLIKELY(strlen(filename_) <= 0)) {
    LOG_STDERR("invalid argument log_file = %p\n", filename_);
    ret = OB_INVALID_ARGUMENT;
  } else if (OB_UNLIKELY(tmp_fd = ::open(filename_, O_WRONLY | O_CREAT | O_APPEND , LOG_FILE_MODE)) < 0) {
    LOG_STDERR("open file = %s errno = %d error = %m\n", filename_, errno);
    ret = OB_ERR_UNEXPECTED;
  } else if (OB_UNLIKELY(0 != fstat(tmp_fd, &stat_))) {
    LOG_STDERR("fstat file = %s error\n", filename_);
    ret = OB_ERR_UNEXPECTED;
    (void)::close(tmp_fd);
    tmp_fd = -1;
  } else {
    if (redirect_flag) {
      dup2(tmp_fd, STDERR_FILENO);
      dup2(tmp_fd, STDOUT_FILENO);
      if (fd_ > STDERR_FILENO) {
        dup2(tmp_fd, fd_);
        close(tmp_fd);
      } else {
        fd_ = tmp_fd;
      }
    } else {
      if (fd_ > STDERR_FILENO) {
        dup2(tmp_fd, fd_);
        close(tmp_fd);
      } else {
        fd_ = tmp_fd;
      }
    }
    file_size_ = stat_.st_size;
  }
  return ret;
}

int ObLogFileStruct::reopen_wf()
{
  int ret = OB_SUCCESS;
  int32_t tmp_fd = -1;
  if (OB_UNLIKELY(strlen(filename_) <= 0)) {
    LOG_STDERR("invalid argument log_file = %p\n", filename_);
    ret = OB_INVALID_ARGUMENT;
  } else {
    const int64_t WARN_NAME_LEN = 3;
    char tmp_file_name[MAX_LOG_FILE_NAME_SIZE + WARN_NAME_LEN];
    snprintf(tmp_file_name, sizeof(tmp_file_name), "%s.wf", filename_);
    if (OB_UNLIKELY(tmp_fd = ::open(tmp_file_name, O_WRONLY | O_CREAT | O_APPEND , LOG_FILE_MODE)) < 0) {
      LOG_STDERR("open file = %s errno = %d error = %m\n", tmp_file_name, errno);
      ret = OB_ERR_UNEXPECTED;
    } else if (OB_UNLIKELY(0 != fstat(tmp_fd, &wf_stat_))) {
      LOG_STDERR("fstat file = %s error\n", tmp_file_name);
      ret = OB_ERR_UNEXPECTED;
      (void)::close(tmp_fd);
      tmp_fd = -1;
    } else {
      if (wf_fd_ > STDERR_FILENO) {
        dup2(tmp_fd, wf_fd_);
        close(tmp_fd);
      } else {
        wf_fd_ = tmp_fd;
      }
    }
  }
  return ret;
}

int ObLogFileStruct::close_all()
{
  int ret = OB_SUCCESS;
  if (fd_ > STDERR_FILENO) {
    (void)::close(fd_);
    fd_ = STDERR_FILENO;
  }
  if (wf_fd_ > STDERR_FILENO) {
    (void)::close(wf_fd_);
    wf_fd_ = STDERR_FILENO;
  }
  return ret;
}

} // end common
} // end oceanbase
