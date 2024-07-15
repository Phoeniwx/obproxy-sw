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

#ifndef OBPROXY_RPC_REQ_PARALLEL_CONT_H
#define OBPROXY_RPC_REQ_PARALLEL_CONT_H

#include "obutils/ob_async_common_task.h"
#include "ob_rpc_req_parallel_processor.h"

namespace oceanbase
{
namespace obproxy
{
namespace executor
{

// class ObProxyRpcReqParallelExecuteCont;
class ObProxyRpcReqParallelCont : public obutils::ObAsyncCommonTask
{
public:
  ObProxyRpcReqParallelCont(event::ObContinuation *cb_cont, event::ObEThread *submit_thread);

  ~ObProxyRpcReqParallelCont();

  int do_open(event::ObAction *&action, common::ObIArray<ObProxyRpcParallelParam> &parallel_param,
              common::ObIAllocator *allocator, bool is_stream_fetch, const int64_t timeout_ms = 0);
  int do_fetch(event::ObAction *&action, ObProxyRpcParallelParam &parallel_param, ObIAllocator *allocator,
               const int64_t timeout_ms, const int64_t cond_index);

  int main_handler(int event, void *data);

  virtual void destroy();
  virtual int schedule_timeout();
  virtual int handle_timeout();

private:
  int handle_parallel_task(common::ObIArray<ObProxyRpcParallelParam> &parallel_param, common::ObIAllocator *allocator);
  int handle_parallel_task_complete(void *data, bool &is_need_free_data);
  int notify_caller_error();
  void cancel_timeout_action();
  void cancel_all_pending_action();

private:
  static const int64_t OB_PROXY_PARALLEL_TIMEOUT_MS = 5000;

private:
  int64_t timeout_ms_;
  int64_t buf_size_;
  int64_t target_task_count_;
  int64_t parallel_task_count_;
  event::ObAction **parallel_action_array_;
  bool handle_stream_rpc_request_;
  DISALLOW_COPY_AND_ASSIGN(ObProxyRpcReqParallelCont);
};

} // end of namespace executor
} // end of namespace obproxy
} // end of namespace oceanbase

#endif // OBPROXY_RPC_REQ_PARALLEL_CONT_H
