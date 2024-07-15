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

#ifndef OCEANBASE_COMMON_OB_ROWKEY_H_
#define OCEANBASE_COMMON_OB_ROWKEY_H_

#include <stdint.h>
#include "lib/oblog/ob_log.h"
#include "lib/ob_define.h"
#include "lib/utility/utility.h"
#include "lib/regex/ob_regex.h"
#include "common/ob_common_utility.h"
#include "common/ob_object.h"

namespace oceanbase
{
namespace common
{

class ObBatchChecksum;
class ObRowkeyInfo;

class ObRowkey
{
public:
  ObRowkey() : obj_ptr_(NULL), obj_cnt_(0) {}
  ObRowkey(ObObj *ptr, const int64_t cnt) : obj_ptr_(ptr), obj_cnt_(cnt) {}
  ~ObRowkey() {}
public:
  void reset() {obj_ptr_ = NULL; obj_cnt_ = 0; }
  inline int64_t get_obj_cnt() const { return obj_cnt_; }
  inline const ObObj *get_obj_ptr() const { return obj_ptr_; }
  inline ObObj *get_obj_ptr() { return obj_ptr_; }
  // for convenience compactible with ObString
  inline int64_t length()  const { return obj_cnt_; }
  inline const ObObj *ptr() const { return obj_ptr_; }
  inline bool is_legal() const { return !(NULL == obj_ptr_ && obj_cnt_ > 0); }
  inline bool is_valid() const { return NULL != obj_ptr_ && obj_cnt_ > 0; }
  // is min rowkey or max rowkey
  inline bool is_min_row(void) const
  {
    bool bret = false;
    int64_t i = 0;
    for (i = 0; i < obj_cnt_; i++) {
      if (!obj_ptr_[i].is_min_value()) {
        break;
      }
    }
    if (obj_cnt_ > 0 && i >= obj_cnt_) {
      bret = true;
    }
    return bret;
  }

  inline bool is_max_row(void) const
  {
    bool bret = false;
    int64_t i = 0;
    for (i = 0; i < obj_cnt_; i++) {
      if (!obj_ptr_[i].is_max_value()) {
        break;
      }
    }
    if (obj_cnt_ > 0 && i >= obj_cnt_) {
      bret = true;
    }
    return bret;
  }

  inline void set_min_row(void) { *this = ObRowkey::MIN_ROWKEY; }
  inline void set_max_row(void) { *this = ObRowkey::MAX_ROWKEY; }
  inline void set_length(const int64_t cnt) { obj_cnt_ = cnt; }
  inline void assign(ObObj *ptr, const int64_t cnt)
  {
    obj_ptr_ = ptr;
    obj_cnt_ = cnt;
  }
  inline int obj_copy(ObRowkey &rowkey, ObObj *obj_buf, const int64_t cnt) const;

  int64_t get_deep_copy_size() const;

  template <typename Allocator>
  int deep_copy(ObRowkey &rhs, Allocator &allocator) const;

  inline int deep_copy(char *buffer, int64_t size) const;

  template <typename Allocator>
  int deserialize(Allocator &allocator, const char *buf, const int64_t data_len, int64_t &pos);

  int serialize(char *buf, const int64_t buf_len, int64_t &pos) const;
  int64_t get_serialize_size(void) const;
  int deserialize(const char *buf, const int64_t buf_len, int64_t &pos);

  int serialize_objs(char *buf, const int64_t buf_len, int64_t &pos) const;
  int64_t get_serialize_objs_size(void) const;
  int deserialize_objs(const char *buf, const int64_t buf_len, int64_t &pos);

  int64_t to_string(char *buffer, const int64_t length) const;
  int64_t to_plain_string(char *buffer, const int64_t length) const;

  int checksum(ObBatchChecksum &bc) const;
  uint64_t murmurhash(const uint64_t hash) const;
  inline uint64_t hash() const
  {
    return murmurhash(0);
  }

public:
  int32_t compare(const ObRowkey &rhs) const;
  int32_t compare(const ObRowkey &rhs, const ObRowkeyInfo *rowkey_info) const;
  int32_t fast_compare(const ObRowkey &rhs) const;

  inline bool operator<(const ObRowkey &rhs) const
  {
    return compare(rhs) < 0;
  }

  inline bool operator<=(const ObRowkey &rhs) const
  {
    return compare(rhs) <= 0;
  }

  inline bool operator>(const ObRowkey &rhs) const
  {
    return compare(rhs) > 0;
  }

  inline bool operator>=(const ObRowkey &rhs) const
  {
    return compare(rhs) >= 0;
  }

  inline bool operator==(const ObRowkey &rhs) const
  {
    return compare(rhs) == 0;
  }
  inline bool operator!=(const ObRowkey &rhs) const
  {
    return compare(rhs) != 0;
  }

private:
  ObObj *obj_ptr_;
  int64_t obj_cnt_;
public:
  static ObObj MIN_OBJECT;
  static ObObj MAX_OBJECT;
  static ObRowkey MIN_ROWKEY;
  static ObRowkey MAX_ROWKEY;
};

class RowkeyInfoHolder
{
public:
  RowkeyInfoHolder() : rowkey_info_(NULL) {}
  explicit RowkeyInfoHolder(const ObRowkeyInfo *ri);
  virtual ~RowkeyInfoHolder();

  void set_rowkey_info(const ObRowkeyInfo *ri);
  inline const ObRowkeyInfo *get_rowkey_info() const { return rowkey_info_; }
protected:
  // only for compare with rowkey_info;
  const ObRowkeyInfo *rowkey_info_;
};

class ObRowkeyLess : public RowkeyInfoHolder
{
public:
  explicit ObRowkeyLess(const ObRowkeyInfo *ri)
      : RowkeyInfoHolder(ri)
  {
  }
  ~ObRowkeyLess() {}
  bool operator()(const ObRowkey &lhs, const ObRowkey &rhs) const;
  int compare(const ObRowkey &lhs, const ObRowkey &rhs) const;
};

inline int64_t ObRowkey::get_deep_copy_size() const
{
  int tmp_ret = OB_SUCCESS;
  int64_t obj_arr_len = obj_cnt_ * sizeof(common::ObObj);
  int64_t total_len = obj_arr_len;

  if (OB_UNLIKELY(!is_legal())) {
    tmp_ret = OB_INVALID_DATA;
    COMMON_LOG(EDIAG, "illegal rowkey.",
               KP_(obj_ptr), K_(obj_cnt), K(tmp_ret));
  } else {
    for (int64_t i = 0; i < obj_cnt_; ++i) {
      total_len += obj_ptr_[i].get_deep_copy_size();
    }
  }

  return total_len;
}

template <typename Allocator>
int ObRowkey::deserialize(Allocator &allocator, const char *buf, const int64_t data_len,
                          int64_t &pos)
{
  int ret = OB_SUCCESS;
  ObObj array[OB_MAX_ROWKEY_COLUMN_NUMBER];
  ObRowkey copy_rowkey;
  copy_rowkey.assign(array, OB_MAX_ROWKEY_COLUMN_NUMBER);

  if (NULL == buf || data_len <= 0) {
    ret = OB_INVALID_ARGUMENT;
    COMMON_LOG(WDIAG, "invalid arguments.",
               KP(buf), K(data_len), K(ret));
  } else if (OB_FAIL(copy_rowkey.deserialize(buf, data_len, pos))) {
    COMMON_LOG(WDIAG, "deserlize to shallow copy key failed.",
               KP(buf), K(data_len), K(pos), K(ret));
  } else if (OB_FAIL(copy_rowkey.deep_copy(*this, allocator))) {
    COMMON_LOG(WDIAG, "deep copy to self failed.",
               KP(buf), K(data_len), K(pos), K(ret));
  }

  return ret;
}

int ObRowkey::deep_copy(char *ptr, int64_t size) const
{
  int ret = OB_SUCCESS;

  int64_t obj_arr_len = obj_cnt_ * sizeof(ObObj);
  if (OB_UNLIKELY(NULL == ptr) || OB_UNLIKELY(size < obj_arr_len)) {
    ret = OB_INVALID_ARGUMENT;
    COMMON_LOG(WDIAG, "invalid arguments.",
               KP(ptr), K(size), KP_(obj_cnt), K(ret));
  } else if (obj_cnt_ > 0 && NULL != obj_ptr_) {
    ObObj *obj_ptr = NULL;
    int64_t pos = 0;

    obj_ptr = reinterpret_cast<ObObj *>(ptr);
    pos += obj_arr_len;

    for (int64_t i = 0; i < obj_cnt_ && OB_SUCCESS == ret; ++i) {
      if (OB_FAIL(obj_ptr[i].deep_copy(obj_ptr_[i], ptr, size, pos))) {
        COMMON_LOG(WDIAG, "deep copy object failed.",
                   K(i), K(obj_ptr_[i]), K(size), K(pos), K(ret));
      }
    }

  }

  return ret;

}

template <typename Allocator>
int ObRowkey::deep_copy(ObRowkey &rhs, Allocator &allocator) const
{
  int ret = OB_SUCCESS;

  int64_t total_len = get_deep_copy_size();
  char *ptr = NULL;
  if (NULL == (ptr = (char *)allocator.alloc(total_len))) {
    ret = OB_ALLOCATE_MEMORY_FAILED;
    COMMON_LOG(WDIAG, "allocate mem for obj array failed.",
               K(total_len), K(ret));
  } else if (OB_UNLIKELY(!is_legal())) {
    ret = OB_INVALID_DATA;
    COMMON_LOG(WDIAG, "illegal rowkey.",
               KP_(obj_ptr), K_(obj_cnt), K(ret));
  } else if (obj_cnt_ > 0 && NULL != obj_ptr_) {
    if (OB_FAIL(deep_copy(ptr, total_len))) {
      COMMON_LOG(WDIAG, "deep copy rowkey failed.",
                 KP_(obj_ptr), KP_(obj_cnt), K(ret));
    } else {
      rhs.assign(reinterpret_cast<ObObj *>(ptr), obj_cnt_);
    }
  } else {
    rhs.assign(NULL, 0);
  }

  if (OB_FAIL(ret) && NULL != ptr) {
    allocator.free(ptr);
    ptr = NULL;
  }

  return ret;

}

inline std::ostream &operator<<(std::ostream &os, const ObRowkey &key)  // for google test
{
  os << " len=" << key.get_obj_cnt();
  return os;
}

template <typename AllocatorT>
int ob_write_rowkey(AllocatorT &allocator, const ObRowkey &src, ObRowkey &dst)
{
  return src.deep_copy(dst, allocator);
}

inline int32_t ObRowkey::fast_compare(const ObRowkey &rhs) const
{
  int32_t cmp = 0;
  int64_t i = 0;
  for (; i < obj_cnt_ && 0 == cmp; ++i) {
    __builtin_prefetch(&rhs.obj_ptr_[i]);
    if (i < rhs.obj_cnt_) {
      cmp = obj_ptr_[i].compare(rhs.obj_ptr_[i]);
    } else {
      cmp = 1;
      break;
    }
  }

  // rhs.cnt > this.cnt
  if (0 == cmp && i < rhs.obj_cnt_) {
    cmp = -1;
  }

  return cmp;
}

// only shallow copy obj, varchar is not copied
inline int ObRowkey::obj_copy(ObRowkey &dest_rowkey, ObObj *obj_buf, const int64_t cnt) const
{
  int ret = OB_SUCCESS;
  if (OB_UNLIKELY(NULL == obj_buf)
      || OB_UNLIKELY(cnt < obj_cnt_)) {
    ret = OB_INVALID_ARGUMENT;
    COMMON_LOG(WDIAG, "invalid argument.",
               KP(obj_buf), K(cnt), K_(obj_cnt), K(ret));
  } else if (OB_UNLIKELY(!is_legal())) {
    ret = OB_INVALID_DATA;
    COMMON_LOG(WDIAG, "illegal rowkey.",
               KP_(obj_ptr), K_(obj_cnt), K(ret));
  } else {
    for (int64_t i = 0; i < obj_cnt_; ++i) {
      obj_buf[i] = obj_ptr_[i];
    }
    dest_rowkey.assign(obj_buf, obj_cnt_);
  }

  return ret;
}
}
}


#endif //OCEANBASE_COMMON_OB_ROWKEY_H_
