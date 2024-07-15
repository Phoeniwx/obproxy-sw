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

#ifndef OCEANBASE_RPC_OBRPC_OB_RPC_RESULT_CODE_
#define OCEANBASE_RPC_OBRPC_OB_RPC_RESULT_CODE_

#include "lib/ob_define.h"
#include "lib/utility/ob_unify_serialize.h"
#include "lib/utility/ob_print_utils.h"
#include "lib/container/ob_se_array.h"
#include "lib/oblog/ob_warning_buffer.h"
namespace oceanbase
{
namespace obrpc
{

struct ObRpcResultCode
{
  OB_UNIS_VERSION(1);

public:
  ObRpcResultCode() : rcode_(0)
  {
    msg_[0] = '\0';
    warnings_.reset();
  }

  void reset()
  {
    rcode_ = 0;
    msg_[0] = '\0';
    warnings_.reset();
  }

  TO_STRING_KV("code", rcode_, "msg", msg_, K_(warnings));

  int32_t rcode_;
  char msg_[common::OB_MAX_ERROR_MSG_LEN];
  common::ObSEArray<common::ObWarningBuffer::WarningItem, 4> warnings_;
};

struct ObRpcResultCodeSimplified
{
  OB_UNIS_VERSION(1);
public:
  static const int SIMPLE_RESULT_CODE_MAX_LEN = 64; // 9 + 9 + 5 = 23, left buffer, so give 64

  ObRpcResultCodeSimplified() : rcode_(0)
  {
  }

  void reset()
  {
    rcode_ = 0;
  }

  TO_STRING_KV("code", rcode_);

  int32_t rcode_;
};

} // end of namespace obrpc
} // end of namespace oceanbase

#endif //OCEANBASE_RPC_OBRPC_OB_RPC_RESULT_CODE_
