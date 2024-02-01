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

#ifndef OB_FUNC_EXPR_PARSER_UTILS_H
#define OB_FUNC_EXPR_PARSER_UTILS_H
#include "lib/utility/ob_print_utils.h"
#include "opsql/func_expr_parser/ob_func_expr_parse_result.h"
#define VBUF_PRINTF(args...) (void)::oceanbase::common::databuff_printf(buf, buf_len, pos, ##args)

namespace oceanbase
{
namespace obproxy
{
namespace opsql
{

class ObFuncExprParseResultPrintWrapper
{
public:
  explicit ObFuncExprParseResultPrintWrapper(const ObFuncExprParseResult &result) :result_(result) {}

  inline void print_indent(char *buf, const int64_t buf_len, int64_t &pos, const int64_t level) const
  {
    // ignore return here
    for (int64_t i = 0; i < level; ++i) {
      VBUF_PRINTF("  ");
    }
  }
  int64_t to_string(char* buf, const int64_t buf_len) const
  {
    int64_t pos = 0;
    int64_t level = 0;
    VBUF_PRINTF("{\n");
    if (OB_NOT_NULL(result_.param_node_) &&
        OB_NOT_NULL(result_.param_node_->func_expr_node_) &&
        OB_NOT_NULL(result_.param_node_->func_expr_node_->func_name_.str_) &&
        result_.param_node_->func_expr_node_->func_name_.str_len_ >= 0 &&
        OB_NOT_NULL(result_.param_node_->func_expr_node_->child_)) {
      ObFuncExprNode *node = result_.param_node_->func_expr_node_;
      ObProxyParamNodeList *child = node->child_;
      VBUF_PRINTF(" function:%.*s, param_num:%ld\n", node->func_name_.str_len_, node->func_name_.str_, child->child_num_);
      ObProxyParamNode *param = child->head_;
      for (int64_t i = 0; i < child->child_num_; ++i) {
        if (OB_NOT_NULL(param)) {
          if (PARAM_COLUMN == param->type_) {
            VBUF_PRINTF(" param[%ld]:[type:%s, value:%.*s], \n", i,
                        get_func_param_type(param->type_),
                        param->col_name_.str_len_, param->col_name_.str_);
          } else if (PARAM_INT_VAL == param->type_) {
            VBUF_PRINTF(" param[%ld]:[type:%s, value:%ld], \n", i,
                        get_func_param_type(param->type_),
                        param->int_value_);
          } else if (PARAM_STR_VAL == param->type_) {
            VBUF_PRINTF(" param[%ld]:[type:%s, value:%.*s], \n", i,
                        get_func_param_type(param->type_),
                        param->str_value_.str_len_, param->str_value_.str_);
          } else if (PARAM_FUNC == param->type_) {
            VBUF_PRINTF(" param[%ld]:[type:%s, child_num:%ld], \n", i,
                        get_func_param_type(param->type_),
                        param->func_expr_node_->child_->child_num_);
            print_func_expr(buf, buf_len, param->func_expr_node_, pos, level+1);
          }
        }
        param = param->next_;
      }
    }
    VBUF_PRINTF("}\n");
    return pos;
  }
  void print_func_expr(char *buf, const int64_t buf_len, ObFuncExprNode *func, int64_t &pos, const int64_t level) const
  {
    if (OB_NOT_NULL(func) && OB_NOT_NULL(func->child_) &&
        OB_NOT_NULL(func->func_name_.str_) &&
        func->func_name_.str_len_ >= 0) {
      ObProxyParamNodeList *child = func->child_;
      print_indent(buf, buf_len, pos, level);
      VBUF_PRINTF("{\n");
      print_indent(buf, buf_len, pos, level);
      VBUF_PRINTF("function:%.*s, param_num:%ld\n", func->func_name_.str_len_, func->func_name_.str_, child->child_num_);
      ObProxyParamNode *param = child->head_;
      for (int64_t i = 0; i < child->child_num_; ++i) {
        if (OB_NOT_NULL(param)) {
          if (PARAM_COLUMN == param->type_) {
            print_indent(buf, buf_len, pos, level);
            VBUF_PRINTF(" param[%ld]:[type:%s, value:%.*s], \n", i,
                        get_func_param_type(param->type_),
                        param->col_name_.str_len_, param->col_name_.str_);
          } else if (PARAM_INT_VAL == param->type_) {
            print_indent(buf, buf_len, pos, level);
            VBUF_PRINTF(" param[%ld]:[type:%s, value:%ld], \n", i,
                        get_func_param_type(param->type_),
                        param->int_value_);
          } else if (PARAM_STR_VAL == param->type_) {
            print_indent(buf, buf_len, pos, level);
            VBUF_PRINTF(" param[%ld]:[type:%s, value:%.*s], \n", i,
                        get_func_param_type(param->type_),
                        param->str_value_.str_len_, param->str_value_.str_);
          } else if (PARAM_FUNC == param->type_) {
            print_indent(buf, buf_len, pos, level);
            VBUF_PRINTF(" param[%ld]:[type:%s], \n", i,
                        get_func_param_type(param->type_));
            print_func_expr(buf, buf_len, param->func_expr_node_,pos,level+1);
          }
        }
        param = param->next_;
      }
      print_indent(buf, buf_len, pos, level);
      VBUF_PRINTF("}\n");
    }
  }
private:
  DISALLOW_COPY_AND_ASSIGN(ObFuncExprParseResultPrintWrapper);
private:
  const ObFuncExprParseResult &result_;
};


} // end namespace opsql
} // end namespace obproxy
} // end namespace oceanbase

#endif /* OB_FUNC_EXPR_PARSER_UTILS_H */
