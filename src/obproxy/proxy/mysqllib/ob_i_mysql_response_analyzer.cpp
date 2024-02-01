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
#include "proxy/mysqllib/ob_i_mysql_response_analyzer.h"
#include "proxy/mysqllib/ob_protocol_diagnosis.h"
namespace oceanbase
{
namespace obproxy
{
namespace proxy
{
ObIMysqlRespAnalyzer::~ObIMysqlRespAnalyzer() {
  DEC_SHARED_REF(protocol_diagnosis_);
}

ObProtocolDiagnosis *&ObIMysqlRespAnalyzer::get_protocol_diagnosis_ref() {
  return protocol_diagnosis_;
}
ObProtocolDiagnosis *ObIMysqlRespAnalyzer::get_protocol_diagnosis() {
  return protocol_diagnosis_;
}
const ObProtocolDiagnosis *ObIMysqlRespAnalyzer::get_protocol_diagnosis() const{
  return protocol_diagnosis_;
}
}
}
}