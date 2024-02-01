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

#include "ob_compressor_pool.h"

namespace oceanbase
{
namespace common
{
ObCompressorPool::ObCompressorPool()
    :none_compressor(),
     lz4_compressor(),
     snappy_compressor(),
     zlib_compressor(),
     zstd_compressor_1_3_8()
{
}
ObCompressorPool &ObCompressorPool::get_instance()
{
  static ObCompressorPool instance_;
  return instance_;
}

int ObCompressorPool::get_compressor(const char *compressor_name,
                                     ObCompressor *&compressor)
{
  int ret = OB_SUCCESS;
  ObCompressorType compressor_type = INVALID_COMPRESSOR;

  if (NULL == compressor_name) {
    ret = OB_INVALID_ARGUMENT;
    LIB_LOG(WDIAG, "invalid compressor name argument, ", K(ret), KP(compressor_name));
  } else if (OB_FAIL(get_compressor_type(compressor_name, compressor_type))) {
    LIB_LOG(WDIAG, "fail to get compressor type, ", K(ret), K(compressor_name));
  } else if (OB_FAIL(get_compressor(compressor_type, compressor))) {
    LIB_LOG(WDIAG, "fail to get compressor", K(ret), K(compressor_type));
  }
  return ret;
}

int ObCompressorPool::get_compressor(const ObCompressorType& compressor_type, ObCompressor*& compressor)
{
  int ret = OB_SUCCESS;
  switch (compressor_type) {
    case NONE_COMPRESSOR:
      compressor = &none_compressor;
      break;
    case LZ4_COMPRESSOR:
      compressor = &lz4_compressor;
      break;
    case SNAPPY_COMPRESSOR:
      compressor = &snappy_compressor;
      break;
    case ZLIB_COMPRESSOR:
      compressor = &zlib_compressor;
      break;
    case ZSTD_1_3_8_COMPRESSOR:
      compressor = &zstd_compressor_1_3_8;
      break;
    default:
      compressor = NULL;
      ret = OB_NOT_SUPPORTED;
      LIB_LOG(WDIAG, "not support compress type, ", K(ret), K(compressor_type));
  }
  return ret; 
} 

int ObCompressorPool::get_compressor_type(const char *compressor_name,
                                          ObCompressorType &compressor_type) const
{
  int ret = OB_SUCCESS;
  if (NULL == compressor_name) {
    ret = OB_INVALID_ARGUMENT;
    LIB_LOG(WDIAG, "invalid compressor name argument, ", K(ret), KP(compressor_name));
  } else if (!strcmp(compressor_name, "none")) {
    compressor_type = NONE_COMPRESSOR;
  } else if (!strcmp(compressor_name, "lz4_1.0")) {
    compressor_type = LZ4_COMPRESSOR;
  } else if (!strcmp(compressor_name, "snappy_1.0")) {
    compressor_type = SNAPPY_COMPRESSOR;
  } else if (!strcmp(compressor_name, "zlib_1.0")) {
    compressor_type = ZLIB_COMPRESSOR;
  } else if (!strcmp(compressor_name, "zstd_1.3.8")) {
    compressor_type = ZSTD_1_3_8_COMPRESSOR;
  } else {
    ret = OB_NOT_SUPPORTED;
    LIB_LOG(WDIAG, "no support compressor type, ", K(ret), K(compressor_name));
  }
  return ret;
}

} /* namespace common */
} /* namespace oceanbase */
