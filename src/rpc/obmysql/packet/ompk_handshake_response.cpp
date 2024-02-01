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

#include "rpc/obmysql/packet/ompk_handshake_response.h"

#include "rpc/obmysql/ob_mysql_util.h"

using namespace oceanbase::common;
using namespace oceanbase::obmysql;

void OMPKHandshakeResponse::reset()
{
  capability_.capability_ = 0;
  max_packet_size_ = 0;
  character_set_ = 0;
  username_.reset();
  auth_response_.reset();
  database_.reset();
  auth_plugin_name_.reset();
  connect_attrs_.reset();
}

int OMPKHandshakeResponse::decode()
{
  int ret = OB_SUCCESS;
  const char *buf = cdata_;
  const char *pos = cdata_;
  const int64_t len = hdr_.len_;
  const char *end = buf + len;
  bool maybe_connector_j = false;

  //OB_ASSERT(NULL != cdata_);
  if (NULL != cdata_) {
    if (len < 2) {
      ret = OB_ERR_UNEXPECTED;
      LOG_WDIAG("error hand shake response packet", K(len), K(ret));
    }

    if (OB_SUCC(ret) && pos < end) {
      capability_.capability_ = uint2korr(pos);
      if (4 == len && !capability_.cap_flags_.OB_CLIENT_PROTOCOL_41) {
        ret = OB_NOT_SUPPORTED;
        LOG_EDIAG("ob only support mysql client protocol 4.1", K(ret));
      } else {
        ObMySQLUtil::get_uint4(pos, capability_.capability_);
        ObMySQLUtil::get_uint4(pos, max_packet_size_); //16MB
        ObMySQLUtil::get_uint1(pos, character_set_);
        pos += HAND_SHAKE_RESPONSE_RESERVED_SIZE;//23 bytes reserved
      }
    }

    // get username
    if (OB_SUCC(ret) && pos < end) {
      username_ = ObString::make_string(pos);
      pos += strlen(pos) + 1;
    }

    // get auth response
    if (OB_SUCC(ret) && pos < end) {
      if (capability_.cap_flags_.OB_CLIENT_PLUGIN_AUTH_LENENC_CLIENT_DATA) {
        // lenenc-int  length of auth-response
        // string[n]   auth-response
        uint64_t auth_response_len = 0;
        ret = ObMySQLUtil::get_length(pos, auth_response_len);
        if (OB_SUCC(ret)) {
          auth_response_.assign_ptr(pos, static_cast<uint32_t>(auth_response_len));
          pos += auth_response_len;
        } else {
          LOG_WDIAG("fail to get len encode number", K(ret));
        }
      } else if (capability_.cap_flags_.OB_CLIENT_SECURE_CONNECTION) {
        // 1           length of auth-response
        // string[n]   auth-response
        uint8_t auth_response_len = 0;
        ObMySQLUtil::get_uint1(pos, auth_response_len);
        auth_response_.assign_ptr(pos, auth_response_len);
        pos += auth_response_len;
      } else {
        //string[NUL]    auth-response
        auth_response_ = ObString::make_string(pos);
        pos += strlen(pos) + 1;
      }
    }

    // get database name
    if (OB_SUCC(ret) && pos < end) {
      if (capability_.cap_flags_.OB_CLIENT_CONNECT_WITH_DB) {
        database_ = ObString::make_string(pos);
        pos += strlen(pos) + 1;
      } else {
        if ('\0' == *pos) {
          maybe_connector_j = true;
        }
      }
    }

    // get auth plugin name
    if (OB_SUCC(ret) && pos < end) {
      if (capability_.cap_flags_.OB_CLIENT_PLUGIN_AUTH) {
        auth_plugin_name_ = ObString::make_string(pos);
        pos += strlen(pos) + 1;
      }
    }

    // get client connect attrbutes
    if (OB_SUCC(ret) && pos < end) {
      if (capability_.cap_flags_.OB_CLIENT_CONNECT_ATTRS) {
        uint64_t all_attrs_len = 0;
        ret = ObMySQLUtil::get_length(pos, all_attrs_len);
        //OB_ASSERT(OB_SUCC(ret));
        if (OB_SUCC(ret)) {
          ObStringKV str_kv;
          while (all_attrs_len > 0 && OB_SUCC(ret) && pos < end) {
            // get key
            uint64_t key_inc_len = 0;
            uint64_t key_len = 0;
            ret = ObMySQLUtil::get_length(pos, key_len, key_inc_len);
            //OB_ASSERT(OB_SUCC(ret) && all_attrs_len > key_inc_len);
            if (OB_SUCC(ret) && all_attrs_len > key_inc_len) {
              all_attrs_len -= key_inc_len;
              str_kv.key_.assign_ptr(pos, static_cast<int32_t>(key_len));
              //OB_ASSERT(all_attrs_len > key_len);
              if (all_attrs_len > key_len) {
                all_attrs_len -= key_len;
                pos += key_len;

                if (pos > end) {
                  if (!maybe_connector_j) {
                    ret = OB_ERR_UNEXPECTED;
                    LOG_WDIAG("unexpected error pos > end", K(ret), K(pos), K(end));
                  } else {
                    ret = OB_INVALID_ARGUMENT;
                    LOG_DEBUG("unexpected error pos > end, may by connector/j", K(ret), K(pos), K(end));
                  }
                } else {
                  // get value
                  uint64_t value_inc_len = 0;
                  uint64_t value_len = 0;
                  ret = ObMySQLUtil::get_length(pos, value_len, value_inc_len);
                  //OB_ASSERT(OB_SUCC(ret) && all_attrs_len > value_inc_len);
                  if (OB_SUCC(ret) && all_attrs_len >= value_inc_len && pos <= end) {
                    all_attrs_len -= value_inc_len;
                    str_kv.value_.assign_ptr(pos, static_cast<int32_t>(value_len));
                    //OB_ASSERT(all_attrs_len >= value_len);
                    if (all_attrs_len >= value_len) {
                      all_attrs_len -= value_len;
                      if (end - pos >= value_len) {
                        pos += value_len;
                        if (OB_FAIL(connect_attrs_.push_back(str_kv))) {
                          LOG_WDIAG("fail to push back str_kv", K(str_kv), K(ret));
                        }
                      }
                    } else {
                      if (!maybe_connector_j) {
                        LOG_EDIAG("invalid packet", K(all_attrs_len), K(value_len));
                      } else {
                        LOG_DEBUG("invalid packet, may by connector/j", K(all_attrs_len), K(value_len));
                      }
                    }
                  } else {
                    if (!maybe_connector_j) {
                      LOG_EDIAG("invalid packet", K(all_attrs_len), K(value_inc_len));
                    } else {
                      LOG_DEBUG("invalid packet, may by connector/j", K(all_attrs_len), K(value_inc_len));
                    }
                  }
                }
              } else {
                if (!maybe_connector_j) {
                  LOG_EDIAG("invalid packet", K(all_attrs_len), K(key_len));
                } else {
                  LOG_DEBUG("invalid packet, may by connector/j", K(all_attrs_len), K(key_len));
                }
              }
            } else {
              if (!maybe_connector_j) {
                LOG_EDIAG("error", K(all_attrs_len), K(key_inc_len));
              } else {
                LOG_DEBUG("error, may by connector/j", K(all_attrs_len), K(key_inc_len));
              }
            }
          }
        } else {
          ret = OB_INVALID_ARGUMENT;
          if (!maybe_connector_j) {
            LOG_EDIAG("get len fail", K(ret), K(pos), K(all_attrs_len));
          } else {
            LOG_DEBUG("get len fail, may by connector/j", K(ret), K(pos), K(all_attrs_len));
          }
        }

        /* If the length is wrong, and suspect it is connector/j, let it go, bug: https://bugs.mysql.com/bug.php?id=79612 */
        if (OB_INVALID_ARGUMENT == ret && maybe_connector_j) {
          ret = OB_SUCCESS;
          connect_attrs_.reset();
        }
      }
    }
  } else {
    ret = OB_INVALID_ARGUMENT;
    LOG_EDIAG("null input", K(ret), K(cdata_));
  }
  // MySQL doesn't care whether there's bytes remain, we do so.  JDBC
  // won't set OB_CLIENT_CONNECT_WITH_DB but leaves a '\0' in the db
  // field when database name isn't specified. It can confuse us if we
  // check whether there's bytes remain in packet. So we ignore it
  // the same as what MySQL does.

  return ret;
}

int64_t OMPKHandshakeResponse::get_serialize_size() const
{
  int64_t len = 0;
  len =   4   // capability flags
        + 4   // max-packet size
        + 1   // character set
        + HAND_SHAKE_RESPONSE_RESERVED_SIZE  // reserved 23 (all [0])
        ;
  len += username_.length() + 1; // username
  if (capability_.cap_flags_.OB_CLIENT_PLUGIN_AUTH_LENENC_CLIENT_DATA) {
    len += ObMySQLUtil::get_number_store_len(auth_response_.length());
    len += auth_response_.length();
  } else if (capability_.cap_flags_.OB_CLIENT_SECURE_CONNECTION ) {
    len += 1;
    len += auth_response_.length();
  } else {
    len += auth_response_.length() + 1;
  }
  if (capability_.cap_flags_.OB_CLIENT_CONNECT_WITH_DB) {
    len += database_.length() + 1;
  }
  if (capability_.cap_flags_.OB_CLIENT_PLUGIN_AUTH) {
    len += auth_plugin_name_.length() + 1;
  }
  if (capability_.cap_flags_.OB_CLIENT_CONNECT_ATTRS) {
    uint64_t all_attr_len = get_connect_attrs_len();
    all_attr_len += ObMySQLUtil::get_number_store_len(all_attr_len);
    len += all_attr_len;
  }
  return len;
}

int OMPKHandshakeResponse::serialize(char *buffer, const int64_t length, int64_t &pos) const
{
  int ret = OB_SUCCESS;

  if (NULL == buffer || length - pos < static_cast<int64_t>(get_serialize_size())) {
    LOG_WDIAG("invalid argument", K(buffer), K(length), K(pos), "need_size", get_serialize_size());
    ret = OB_INVALID_ARGUMENT;
  } else {
    if (OB_FAIL(ObMySQLUtil::store_int4(buffer, length, capability_ .capability_, pos))) {
      LOG_WDIAG("store fail", K(ret), K(buffer), K(length), K(pos));
    } else if (OB_FAIL(ObMySQLUtil::store_int4(buffer, length, max_packet_size_, pos))) {
      LOG_WDIAG("store fail", K(ret), K(buffer), K(length), K(pos));
    } else if (OB_FAIL(ObMySQLUtil::store_int1(buffer, length, character_set_, pos))) {
      LOG_WDIAG("store fail", K(ret), K(buffer), K(length), K(pos));
    } else {
      char reserved[HAND_SHAKE_RESPONSE_RESERVED_SIZE];
      memset(reserved, 0, sizeof (reserved));
      if (OB_FAIL(ObMySQLUtil::store_str_vnzt(buffer, length, reserved, sizeof (reserved), pos))) {
        LOG_WDIAG("store fail", K(ret), K(buffer), K(length), K(pos));
      }
      if (OB_SUCC(ret)) {
        if (OB_FAIL(ObMySQLUtil::store_obstr_zt(buffer, length, username_, pos))) {
          LOG_WDIAG("store fail", K(ret), K(buffer), K(length), K(pos));
        }
      }
      if (capability_.cap_flags_.OB_CLIENT_PLUGIN_AUTH_LENENC_CLIENT_DATA) {
        if (OB_SUCC(ret)) {
          if (OB_FAIL(ObMySQLUtil::store_obstr(buffer, length, auth_response_, pos))) {
            LOG_WDIAG("store fail", K(ret), K(buffer), K(length), K(pos));
          }
        }
      } else if (capability_.cap_flags_.OB_CLIENT_SECURE_CONNECTION ) {
        if (OB_SUCC(ret)) {
          if (OB_FAIL(ObMySQLUtil::store_int1(buffer, length,
              static_cast<uint8_t>(auth_response_.length()), pos))) {
            LOG_WDIAG("store fail", K(ret), K(buffer), K(length), K(pos));
          } else if (OB_FAIL(ObMySQLUtil::store_str_vnzt(buffer, length,
              auth_response_.ptr(), auth_response_.length(), pos))) {
            LOG_WDIAG("store fail", K(ret), K(buffer), K(length), K(pos));
          }
        }
      } else {
        if (OB_SUCC(ret)) {
          if (OB_FAIL(ObMySQLUtil::store_obstr_zt(buffer, length, auth_response_, pos))) {
            LOG_WDIAG("store fail", K(ret), K(buffer), K(length), K(pos));
          }
        }
      }
      if (capability_.cap_flags_.OB_CLIENT_CONNECT_WITH_DB) {
        if (OB_SUCC(ret)) {
          if (OB_FAIL(ObMySQLUtil::store_obstr_zt(buffer, length, database_, pos))) {
            LOG_WDIAG("store fail", K(ret), K(buffer), K(length), K(pos));
          }
        }
      }
      if (capability_.cap_flags_.OB_CLIENT_PLUGIN_AUTH) {
        if (OB_SUCC(ret)) {
          if (OB_FAIL(ObMySQLUtil::store_obstr_zt(buffer, length, auth_plugin_name_, pos)))  {
            LOG_WDIAG("store fail", K(ret), K(buffer), K(length), K(pos));
          }
        }
      }
      if (capability_.cap_flags_.OB_CLIENT_CONNECT_ATTRS) {
        uint64_t all_attr_len = get_connect_attrs_len();
        if (OB_SUCC(ret)) {
          if (OB_FAIL(ObMySQLUtil::store_length(buffer, length, all_attr_len, pos))) {
            LOG_WDIAG("store fail", K(ret), K(buffer), K(length), K(pos));
          }
        }

        ObStringKV string_kv;
        for (int64_t i = 0; OB_SUCC(ret) && i <  connect_attrs_.count(); ++i) {
          string_kv = connect_attrs_.at(i);
          if (OB_FAIL(ObMySQLUtil::store_obstr(buffer, length, string_kv.key_, pos))) {
            LOG_WDIAG("store fail", K(ret), K(buffer), K(length), K(pos));
          } else if (OB_FAIL(ObMySQLUtil::store_obstr(buffer, length, string_kv.value_, pos))) {
            LOG_WDIAG("store fail", K(ret), K(buffer), K(length), K(pos));
          }
        }
      }
    }
  }
  return ret;

}

int OMPKHandshakeResponse::add_connect_attr(const ObStringKV &string_kv)
{
  int ret = OB_SUCCESS;
  if (string_kv.key_.empty()) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WDIAG("invalid input value", K(string_kv), K(ret));
  } else if (OB_FAIL(connect_attrs_.push_back(string_kv))) {
    LOG_WDIAG("fail to push back string kv", K(string_kv), K(ret));
  }
  return ret;
}

uint64_t OMPKHandshakeResponse::get_connect_attrs_len() const
{
  uint64_t all_attr_len = 0;
  ObStringKV string_kv;
  if (capability_.cap_flags_.OB_CLIENT_CONNECT_ATTRS) {
    for (int64_t i = 0; i < connect_attrs_.count(); i++) {
      string_kv = connect_attrs_.at(i);
      all_attr_len += ObMySQLUtil::get_number_store_len(string_kv.key_.length());
      all_attr_len += string_kv.key_.length();
      all_attr_len += ObMySQLUtil::get_number_store_len(string_kv.value_.length());
      all_attr_len += string_kv.value_.length();
    }
  }
  return all_attr_len;
}

bool OMPKHandshakeResponse::is_obproxy_client_mod() const
{
  bool obproxy_mod = false;
  ObStringKV mod_kv;
  mod_kv.key_.assign_ptr(OB_MYSQL_CLIENT_MODE, static_cast<int32_t>(strlen(OB_MYSQL_CLIENT_MODE)));
  mod_kv.value_.assign_ptr(OB_MYSQL_CLIENT_OBPROXY_MODE,
      static_cast<int32_t>(strlen(OB_MYSQL_CLIENT_OBPROXY_MODE)));

  ObStringKV kv;
  for (int64_t i = 0; !obproxy_mod && i < connect_attrs_.count(); ++i) {
    kv = connect_attrs_.at(i);
    if (mod_kv.key_ == kv.key_ && mod_kv.value_ == kv.value_) {
      obproxy_mod = true;
      //break;
    }
  }
  return obproxy_mod;
}
