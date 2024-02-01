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

#define USING_LOG_PREFIX RPC_OBMYSQL

#include "rpc/obmysql/packet/ompk_eof.h"

#include "rpc/obmysql/ob_mysql_util.h"

using namespace oceanbase::common;
using namespace oceanbase::obmysql;

OMPKEOF::OMPKEOF()
    : field_count_(0xfe),
      warning_count_(0),
      server_status_()
{}

OMPKEOF::~OMPKEOF()
{}

int OMPKEOF::serialize(char *buffer, int64_t len, int64_t &pos) const
{
  int ret = OB_SUCCESS;

  if (NULL == buffer || len - pos < static_cast<int64_t>(get_serialize_size())) {
    LOG_WDIAG("invalid argument", K(buffer), K(len), K(pos), "need_size", get_serialize_size());
    ret = OB_INVALID_ARGUMENT;
  } else {
    if (OB_FAIL(ObMySQLUtil::store_int1(buffer, len, field_count_, pos))) {
      LOG_WDIAG("store fail", K(buffer), K(len), K(pos), K(ret));
    } else if (OB_FAIL(ObMySQLUtil::store_int2(buffer, len, warning_count_, pos))) {
      LOG_WDIAG("store fail", K(buffer), K(len), K(pos), K(ret));
    } else if (OB_FAIL(ObMySQLUtil::store_int2(buffer, len, server_status_.flags_, pos))) {
      LOG_WDIAG("store fail", K(buffer), K(len), K(pos), K(ret));
    }
  }

  return ret;
}

int OMPKEOF::decode()
{
  int ret = OB_SUCCESS;
  const char *pos = cdata_;
  const int64_t len = hdr_.len_;
  const char *end = pos + len;

  if (NULL != cdata_) {
    //OB_ASSERT(NULL != cdata_);
    ObMySQLUtil::get_uint1(pos, field_count_);
    ObMySQLUtil::get_uint2(pos,  warning_count_);
    ObMySQLUtil::get_uint2(pos, server_status_.flags_);
    //OB_ASSERT(pos == end);
    if (pos != end) {
      ret = OB_ERR_UNEXPECTED;
      LOG_EDIAG("packet error, pos != end", K(pos), K(end), K(ret));
    }
  } else {
    ret = OB_INVALID_ARGUMENT;
    LOG_EDIAG("null input", K(cdata_), K(ret));
  }
  return ret;
}

int64_t OMPKEOF::get_serialize_size() const
{
  int64_t len = 0;
  len += 1;                 // field_count_
  len += 2;                 // warning_count_
  len += 2;                 // server_status_
  return len;
}

int64_t OMPKEOF::to_string(char *buf, const int64_t buf_len) const
{
  int64_t pos = 0;
  J_KV(K_(field_count),
       K_(warning_count),
       K_(server_status_.flags)
      );
  return pos;
}
