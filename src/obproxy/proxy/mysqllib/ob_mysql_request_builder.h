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

#ifndef OBPROXY_MYSQL_REQUEST_BUILDER_H
#define OBPROXY_MYSQL_REQUEST_BUILDER_H

#include "utils/ob_proxy_lib.h"
#include "packet/ob_mysql_packet_writer.h"
#include "proxy/mysqllib/ob_mysql_ob20_packet_write.h"
#include "proxy/mysql/ob_mysql_server_session.h"
#include "proxy/mysqllib/ob_compressed_header_param.h"

namespace oceanbase
{
namespace common
{
class ObSqlString;
}
namespace obproxy
{
namespace event
{
class ObMIOBuffer;
}
namespace proxy
{
class ObMysqlSM;

typedef int (*BuildFunc)(ObMysqlSM *sm, event::ObMIOBuffer &, ObClientSessionInfo &, ObMysqlServerSession *,
                         const ObProxyProtocol ob_proxy_protocol);

class ObMysqlRequestBuilder
{
public:
  static int build_request_packet(ObString sql,
                                  obmysql::ObMySQLCmd cmd,
                                  ObMysqlSM *sm,
                                  event::ObMIOBuffer &mio_buf,
                                  ObMysqlServerSession *server_session,
                                  const ObProxyProtocol ob_proxy_protocol);
  // build login packet to send first login request
  static int build_first_login_packet(ObMysqlSM *sm,
                                      event::ObMIOBuffer &mio_buf,
                                      ObClientSessionInfo &client_info,
                                      ObMysqlServerSession *server_session,
                                      const ObProxyProtocol ob_proxy_protocol);

  // build login packet to send orig login request
  static int build_orig_login_packet(ObMysqlSM *sm,
                                     event::ObMIOBuffer &mio_buf,
                                     ObClientSessionInfo &client_info,
                                     ObMysqlServerSession *server_session,
                                     const ObProxyProtocol ob_proxy_protocol);
  
  // build binlog login request
  static int build_binlog_login_packet(ObMysqlSM *sm,
                                       event::ObMIOBuffer &mio_buf,
                                       ObClientSessionInfo &client_info,
                                       ObMysqlServerSession *server_session,
                                       const ObProxyProtocol ob_proxy_protocol);

  // build saved login packet to send saved login request
  static int build_saved_login_packet(ObMysqlSM *sm,
                                      event::ObMIOBuffer &mio_buf,
                                      ObClientSessionInfo &client_info,
                                      ObMysqlServerSession *server_session,
                                      const ObProxyProtocol ob_proxy_protocol);

  static int build_ssl_request_packet(ObMysqlSM *sm,
                                      event::ObMIOBuffer &mio_buf,
                                      ObClientSessionInfo &client_info,
                                      ObMysqlServerSession *server_session,
                                      const ObProxyProtocol ob_proxy_protocol);

  // build packet to sync all session vars
  static int build_all_session_vars_sync_packet(ObMysqlSM *sm,
                                                event::ObMIOBuffer &mio_buf,
                                                ObClientSessionInfo &client_info,
                                                ObMysqlServerSession *server_session,
                                                const ObProxyProtocol ob_proxy_protocol);

  // build OB_MYSQL_COM_INIT_DB packet to sync database name
  static int build_database_sync_packet(ObMysqlSM *sm,
                                        event::ObMIOBuffer &mio_buf,
                                        ObClientSessionInfo &client_info,
                                        ObMysqlServerSession *server_session,
                                        const ObProxyProtocol ob_proxy_protocol);

  // build OB_MYSQL_COM_QUERY packet to sync session vars
  static int build_session_vars_sync_packet(ObMysqlSM *sm,
                                            event::ObMIOBuffer &mio_buf,
                                            ObClientSessionInfo &client_info,
                                            ObMysqlServerSession *server_session,
                                            const ObProxyProtocol ob_proxy_protocol);

  // build OB_MYSQL_COM_QUERY packet to sync session user vars
  static int build_session_user_vars_sync_packet(ObMysqlSM *sm,
                                                 event::ObMIOBuffer &mio_buf,
                                                 ObClientSessionInfo &client_info,
                                                 ObMysqlServerSession *server_session,
                                                 const ObProxyProtocol ob_proxy_protocol);

  // build start transaction request packet
  static int build_start_trans_request(ObMysqlSM *sm,
                                       event::ObMIOBuffer &mio_buf,
                                       ObClientSessionInfo &client_info,
                                       ObMysqlServerSession *server_session,
                                       const ObProxyProtocol ob_proxy_protocol);
  
  static int build_xa_start_request(ObMysqlSM *sm,
                                    event::ObMIOBuffer &mio_buf,
                                    ObClientSessionInfo &client_info,
                                    ObMysqlServerSession *server_session,
                                    const ObProxyProtocol ob_proxy_protocol);

  // build mysql request packet
  static int build_mysql_request(event::ObMIOBuffer &mio_buf,
                                 const obmysql::ObMySQLCmd cmd,
                                 const common::ObString &sql,
                                 const bool need_compress,
                                 const bool is_checksum_on, 
                                 const int64_t compression_level);

  // build mysql prepare request packet
  static int build_prepare_request(ObMysqlSM *sm,
                                   event::ObMIOBuffer &mio_buf,
                                   ObClientSessionInfo &client_info,
                                   ObMysqlServerSession *server_session,
                                   const ObProxyProtocol ob_proxy_protocol);

  // build mysql text_ps prepare request packet
  static int build_text_ps_prepare_request(ObMysqlSM *sm,
                                           event::ObMIOBuffer &mio_buf,
                                           ObClientSessionInfo &client_info,
                                           ObMysqlServerSession *server_session,
                                           const ObProxyProtocol ob_proxy_protocol);

  // build mysql init sql request packet
  static int build_init_sql_request_packet(ObMysqlSM *sm,
                                           event::ObMIOBuffer &mio_buf,
                                           ObClientSessionInfo &client_info,
                                           ObMysqlServerSession *server_session,
                                           const ObProxyProtocol ob_proxy_protocol);
};

inline int ObMysqlRequestBuilder::build_first_login_packet(ObMysqlSM *sm,
                                                           event::ObMIOBuffer &mio_buf,
                                                           ObClientSessionInfo &client_info,
                                                           ObMysqlServerSession *server_session,
                                                           const ObProxyProtocol ob_proxy_protocol)

{
  UNUSED(sm);
  UNUSED(server_session);
  UNUSED(ob_proxy_protocol); // auth request no need compress
  ObMysqlAuthRequest &auth_req = client_info.get_login_req();
  ObHSRResult &hsr = auth_req.get_hsr_result();
  ObServerSessionInfo &ss_info = server_session->get_session_info();
  obmysql::ObMySQLCapabilityFlags capability(ss_info.get_compatible_capability_flags().capability_ & hsr.response_.get_capability_flags().capability_);
  ss_info.save_compatible_capability_flags(capability);
  common::ObString &packet_str = auth_req.get_auth_request();
  return packet::ObMysqlPacketWriter::write_raw_packet(mio_buf, packet_str);
}

inline int ObMysqlRequestBuilder::build_orig_login_packet(ObMysqlSM *sm,
                                                          event::ObMIOBuffer &mio_buf,
                                                          ObClientSessionInfo &client_info,
                                                          ObMysqlServerSession *server_session,
                                                          const ObProxyProtocol ob_proxy_protocol)

{
  UNUSED(sm);
  UNUSED(server_session);
  UNUSED(ob_proxy_protocol); // auth request no need compress
  ObMysqlAuthRequest &auth_req = client_info.get_login_req();
  ObHSRResult &hsr = auth_req.get_hsr_result();
  ObServerSessionInfo &ss_info = server_session->get_session_info();
  obmysql::ObMySQLCapabilityFlags capability(ss_info.get_compatible_capability_flags().capability_ & hsr.response_.get_capability_flags().capability_);
  ss_info.save_compatible_capability_flags(capability);
  common::ObString &packet_str = auth_req.get_auth_request();
  return packet::ObMysqlPacketWriter::write_raw_packet(mio_buf, packet_str);
}

inline int ObMysqlRequestBuilder::build_saved_login_packet(ObMysqlSM *sm,
                                                           event::ObMIOBuffer &mio_buf,
                                                           ObClientSessionInfo &client_info,
                                                           ObMysqlServerSession *server_session,
                                                           const ObProxyProtocol ob_proxy_protocol)
{
  UNUSED(sm);
  UNUSED(ob_proxy_protocol); // auth request no need compress
  ObMysqlAuthRequest &auth_req = client_info.get_login_req();
  ObHSRResult &hsr = auth_req.get_hsr_result();
  ObServerSessionInfo &ss_info = server_session->get_session_info();
  obmysql::ObMySQLCapabilityFlags capability(ss_info.get_compatible_capability_flags().capability_ & hsr.response_.get_capability_flags().capability_);
  ss_info.save_compatible_capability_flags(capability);
  common::ObString &packet_str = auth_req.get_auth_request();
  return packet::ObMysqlPacketWriter::write_raw_packet(mio_buf, packet_str);
}

inline int ObMysqlRequestBuilder::build_mysql_request(event::ObMIOBuffer &mio_buf,
                                                      const obmysql::ObMySQLCmd cmd,
                                                      const common::ObString &sql,
                                                      const bool need_compress,
                                                      const bool is_checksum_on,
                                                      const int64_t compression_level)
{
  proxy::ObCompressedHeaderParam param(0, is_checksum_on, compression_level);
  return packet::ObMysqlPacketWriter::write_request_packet(mio_buf, cmd, sql, need_compress, param);
}

} // end of namespace proxy
} // end of namespace obproxy
} // end of namespace oceanbase

#endif // OBPROXY_MYSQL_REQUEST_BUILDER_H
