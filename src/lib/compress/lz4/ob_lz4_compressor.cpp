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

#include "lib/compress/lz4/ob_lz4_compressor.h"
#include "lib/ob_errno.h"

namespace oceanbase
{
namespace common
{
const char *ObLZ4Compressor::compressor_name = "lz4_1.0";
int ObLZ4Compressor::compress(const char *src_buffer,
                              const int64_t src_data_size,
                              char *dst_buffer,
                              const int64_t dst_buffer_size,
                              int64_t &dst_data_size)
{
  int ret = OB_SUCCESS;
  int64_t max_overflow_size = 0;
  if (NULL == src_buffer
      || 0 >= src_data_size
      || NULL == dst_buffer
      || 0 >= dst_buffer_size) {
    ret = OB_INVALID_ARGUMENT;
    LIB_LOG(WDIAG, "invalid compress argument, ",
        K(ret), KP(src_buffer), K(src_data_size), KP(dst_buffer), K(dst_buffer_size));
  } else if (OB_FAIL(get_max_overflow_size(src_data_size, max_overflow_size))) {
    LIB_LOG(WDIAG, "fail to get max_overflow_size, ", K(ret), K(src_data_size));
  } else if ((src_data_size + max_overflow_size) > dst_buffer_size) {
    ret = OB_BUF_NOT_ENOUGH;
    LIB_LOG(WDIAG, "dst buffer not enough, ",
        K(ret), K(src_data_size), K(max_overflow_size), K(dst_buffer_size));
  } else if (0 >= (dst_data_size = LZ4_compress(src_buffer,
                                                dst_buffer,
                                                static_cast<int32_t>(src_data_size)))) {
    ret = OB_ERR_COMPRESS_DECOMPRESS_DATA;
    LIB_LOG(WDIAG, "fail to compress data by LZ4_compress, ",
        K(ret), K(src_buffer), K(src_data_size), K(dst_data_size));
  }

  return ret;
}

int ObLZ4Compressor::decompress(const char *src_buffer,
                                const int64_t src_data_size,
                                char *dst_buffer,
                                const int64_t dst_buffer_size,
                                int64_t &dst_data_size)
{
  int ret = OB_SUCCESS;

  if (NULL == src_buffer
      || 0 >= src_data_size
      || NULL == dst_buffer
      || 0 >= dst_buffer_size) {
    ret = OB_INVALID_ARGUMENT;
    LIB_LOG(WDIAG, "invalid decompress argument, ",
        K(ret), KP(src_buffer), K(src_data_size), K(dst_buffer), K(dst_buffer_size));
  } else if (0 > (dst_data_size = LZ4_uncompress(src_buffer,
                                                 dst_buffer,
                                                 static_cast<int32_t>(dst_buffer_size)))) {
    ret = OB_ERR_COMPRESS_DECOMPRESS_DATA;
    LIB_LOG(WDIAG, "fail to decompress by LZ4_uncompress, ",
        K(ret), KP(src_buffer), K(dst_buffer_size), K(dst_data_size));
  }

  return ret;
}

int ObLZ4Compressor::get_max_overflow_size(const int64_t src_data_size,
                                           int64_t &max_overflow_size) const
{
  int ret = OB_SUCCESS;
  if (src_data_size < 0) {
    ret = OB_INVALID_ARGUMENT;
    LIB_LOG(WDIAG, "invalid argument, ", K(ret), K(src_data_size));
  } else {
    max_overflow_size = src_data_size / 255 + 16;
  }
  return ret;
}

const char *ObLZ4Compressor::get_compressor_name() const
{
  return compressor_name;
}

}//namespace common
}//namesoace oceanbase
