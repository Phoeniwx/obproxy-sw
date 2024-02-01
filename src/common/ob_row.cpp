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

#define USING_LOG_PREFIX COMMON

#include "common/ob_row.h"
#include "lib/utility/utility.h"

namespace oceanbase
{
namespace common
{

DEFINE_SERIALIZE(ObColumnInfo)
{
  int ret = OB_SUCCESS;
  int32_t cs_type_int = static_cast<int32_t>(cs_type_);
  OB_UNIS_ENCODE(index_);
  OB_UNIS_ENCODE(cs_type_int);
  return ret;
}

DEFINE_DESERIALIZE(ObColumnInfo)
{
  int ret = OB_SUCCESS;
  int32_t cs_type_int = 0;
  OB_UNIS_DECODE(index_);
  OB_UNIS_DECODE(cs_type_int);
  cs_type_ = static_cast<ObCollationType>(cs_type_int);
  return ret;
}

DEFINE_GET_SERIALIZE_SIZE(ObColumnInfo)
{
  int64_t len = 0;
  int32_t cs_type_int = 0;
  OB_UNIS_ADD_LEN(index_);
  OB_UNIS_ADD_LEN(cs_type_int);   // for cs_type_.
  return len;
}

void ObNewRow::reset()
{
  cells_ = NULL;
  count_ = 0;
  projector_size_ = 0;
  projector_ = NULL;
}

int64_t ObNewRow::get_deep_copy_size() const
{
  int64_t size = sizeof(ObObj) * count_;
  size += sizeof(int32_t) * projector_size_;

  for (int64_t i = 0; i < count_; ++i) {
    size += cells_[i].get_deep_copy_size();
  }
  //The projector_ array is of int32_t type, no additional buffer is required
  return size;
}

bool ObNewRow::operator==(const ObNewRow &other)
{
  bool is_equal = true;
  int64_t count1 = get_count();
  int64_t count2 = other.get_count();
  if (count1 != count2) {
    is_equal = false;
  } else {
    for (int64_t i = 0; is_equal && i < count1; i++) {
      const ObObj *obj1 = get_cell(i);
      const ObObj *obj2 = other.get_cell(i);
      if (OB_ISNULL(obj1) && OB_ISNULL(obj2)) {
        is_equal = true;
      } else if (OB_NOT_NULL(obj1) && OB_NOT_NULL(obj2)) {
        is_equal = (*obj1 == *obj2); 
      } else {
        is_equal = false;
      }
    }
  }
  return is_equal;
}

int ObNewRow::deep_copy(const ObNewRow &src, char *buf, int64_t len, int64_t &pos)
{
  int ret = OB_SUCCESS;

  if (src.get_deep_copy_size() + pos > len) {
    ret = OB_SIZE_OVERFLOW;
    LOG_WDIAG("size overflow, ", K(ret), "need", src.get_deep_copy_size() + pos, K(len));
  } else {
    cells_ = new(buf + pos) ObObj[src.count_];
    pos += src.count_ * sizeof(ObObj);
    count_ = src.count_;
  }

  for (int64_t i = 0; OB_SUCC(ret) && i < src.count_; ++i) {
    if (OB_FAIL(cells_[i].deep_copy(src.cells_[i], buf, len, pos))) {
      LOG_WDIAG("fail to deep copy cell, ", K(ret));
    }
  }

  if (OB_SUCC(ret) && src.projector_size_ > 0) {
    projector_ = new(buf + pos) int32_t[src.projector_size_];
    projector_size_ = src.projector_size_;
    pos += projector_size_ * sizeof(int32_t);
  }

  for (int64_t i = 0; OB_SUCC(ret) && i < src.projector_size_; ++i) {
    projector_[i] = src.projector_[i];
  }
  return ret;
}

DEFINE_SERIALIZE(ObNewRow)
{
  int ret = OB_SUCCESS;
  OB_UNIS_ENCODE_ARRAY(cells_, count_);
  OB_UNIS_ENCODE_ARRAY(projector_, projector_size_);
  return ret;
}

DEFINE_DESERIALIZE(ObNewRow)
{
  int ret = OB_SUCCESS;
  int64_t count = 0;
  int64_t projector_size = 0;

  // deserialize cells
  OB_UNIS_DECODE(count);
  if (OB_SUCC(ret)) {
    if (count_ >= count) {
      count_ = count;
    } else {
      ret = OB_ERR_SYS;
      COMMON_LOG(EDIAG, "count must not larger than count_", K(ret), K(count), K(count_));
    }
  }
  if (OB_SUCCESS == ret && count_ > 0) {
    if (OB_ISNULL(cells_)) {
      ret = OB_NOT_INIT;
      LOG_WDIAG("cells is null");
    } else {
      OB_UNIS_DECODE_ARRAY(cells_, count_);
    }
  }

  // deserialize projector
  OB_UNIS_DECODE(projector_size);
  if (OB_SUCC(ret)) {
    if (projector_size_ >= projector_size) {
      projector_size_ = projector_size;
    } else {
      ret = OB_ERR_SYS;
      COMMON_LOG(EDIAG, "projector_size must not larger than projector_size_",
          K(ret), K(projector_size), K(projector_size_));
    }
  }

  if (OB_SUCCESS == ret && projector_size_ > 0) {
    if (OB_ISNULL(projector_)) {
      ret = OB_NOT_INIT;
      LOG_WDIAG("projector is null");
    } else {
      OB_UNIS_DECODE_ARRAY(projector_, projector_size_);
    }
  }
  return ret;
}

DEFINE_GET_SERIALIZE_SIZE(ObNewRow)
{
  int64_t len = 0;
  OB_UNIS_ADD_LEN_ARRAY(cells_, count_);
  OB_UNIS_ADD_LEN_ARRAY(projector_, projector_size_);
  return len;
}

} // end namespace common
} // end namespace oceanbase
