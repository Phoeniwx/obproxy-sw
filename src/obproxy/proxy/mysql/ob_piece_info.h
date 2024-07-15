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

#ifndef OBPROXY_PIECE_INFO_H
#define OBPROXY_PIECE_INFO_H

#include "lib/hash/ob_build_in_hashmap.h"
#include "iocore/net/ob_inet.h"

namespace oceanbase
{
namespace obproxy
{
namespace proxy
{

class ObPieceInfo
{
public:
  ObPieceInfo() : ps_id_(0), addr_() {}
  ~ObPieceInfo() {}
  bool is_valid() const { return 0 != ps_id_ && addr_.is_valid(); }
  uint32_t get_ps_id() const { return ps_id_; }
  net::ObIpEndpoint &get_addr() { return addr_; }
  void set_ps_id(const uint32_t ps_id) { ps_id_ = ps_id; }
  void set_addr(const struct sockaddr &addr) { addr_.assign(addr); }
  int64_t to_string(char *buf, const int64_t buf_len) const
  {
    int64_t pos = 0;
    J_OBJ_START();
    J_KV(K_(ps_id),
         K_(addr));
    J_OBJ_END();
    return pos;
  }

public:
  LINK(ObPieceInfo, piece_info_link_);
private:
  uint32_t ps_id_;
  net::ObIpEndpoint addr_;
};

struct ObPieceInfoHashing
{
  typedef uint32_t Key;
  typedef ObPieceInfo Value;
  typedef ObDLList(ObPieceInfo, piece_info_link_) ListHead;

  static uint64_t hash(Key key) { return key; }
  static Key key(Value *value) { return value->get_ps_id(); }
  static bool equal(Key lhs, Key rhs) { return lhs == rhs; }
};

typedef common::hash::ObBuildInHashMap<ObPieceInfoHashing> ObPieceInfoMap;

} // end proxy
} // end obproxy
} // end oceanbase

#endif
