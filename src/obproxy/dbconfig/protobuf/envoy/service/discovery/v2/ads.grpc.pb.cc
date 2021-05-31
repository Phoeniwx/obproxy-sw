// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: ads.proto

#include "ads.pb.h"
#include "ads.grpc.pb.h"

#include <functional>
#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/channel_interface.h>
#include <grpcpp/impl/codegen/client_unary_call.h>
#include <grpcpp/impl/codegen/client_callback.h>
#include <grpcpp/impl/codegen/method_handler_impl.h>
#include <grpcpp/impl/codegen/rpc_service_method.h>
#include <grpcpp/impl/codegen/server_callback.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/sync_stream.h>
namespace envoy {
namespace service {
namespace discovery {
namespace v2 {

static const char* AggregatedDiscoveryService_method_names[] = {
  "/envoy.service.discovery.v2.AggregatedDiscoveryService/StreamAggregatedResources",
  "/envoy.service.discovery.v2.AggregatedDiscoveryService/IncrementalAggregatedResources",
};

std::unique_ptr< AggregatedDiscoveryService::Stub> AggregatedDiscoveryService::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< AggregatedDiscoveryService::Stub> stub(new AggregatedDiscoveryService::Stub(channel));
  return stub;
}

AggregatedDiscoveryService::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel)
  : channel_(channel), rpcmethod_StreamAggregatedResources_(AggregatedDiscoveryService_method_names[0], ::grpc::internal::RpcMethod::BIDI_STREAMING, channel)
  , rpcmethod_IncrementalAggregatedResources_(AggregatedDiscoveryService_method_names[1], ::grpc::internal::RpcMethod::BIDI_STREAMING, channel)
  {}

::grpc::ClientReaderWriter< ::envoy::api::v2::DiscoveryRequest, ::envoy::api::v2::DiscoveryResponse>* AggregatedDiscoveryService::Stub::StreamAggregatedResourcesRaw(::grpc::ClientContext* context) {
  return ::grpc::internal::ClientReaderWriterFactory< ::envoy::api::v2::DiscoveryRequest, ::envoy::api::v2::DiscoveryResponse>::Create(channel_.get(), rpcmethod_StreamAggregatedResources_, context);
}

void AggregatedDiscoveryService::Stub::experimental_async::StreamAggregatedResources(::grpc::ClientContext* context, ::grpc::experimental::ClientBidiReactor< ::envoy::api::v2::DiscoveryRequest,::envoy::api::v2::DiscoveryResponse>* reactor) {
  ::grpc::internal::ClientCallbackReaderWriterFactory< ::envoy::api::v2::DiscoveryRequest,::envoy::api::v2::DiscoveryResponse>::Create(stub_->channel_.get(), stub_->rpcmethod_StreamAggregatedResources_, context, reactor);
}

::grpc::ClientAsyncReaderWriter< ::envoy::api::v2::DiscoveryRequest, ::envoy::api::v2::DiscoveryResponse>* AggregatedDiscoveryService::Stub::AsyncStreamAggregatedResourcesRaw(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq, void* tag) {
  return ::grpc::internal::ClientAsyncReaderWriterFactory< ::envoy::api::v2::DiscoveryRequest, ::envoy::api::v2::DiscoveryResponse>::Create(channel_.get(), cq, rpcmethod_StreamAggregatedResources_, context, true, tag);
}

::grpc::ClientAsyncReaderWriter< ::envoy::api::v2::DiscoveryRequest, ::envoy::api::v2::DiscoveryResponse>* AggregatedDiscoveryService::Stub::PrepareAsyncStreamAggregatedResourcesRaw(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncReaderWriterFactory< ::envoy::api::v2::DiscoveryRequest, ::envoy::api::v2::DiscoveryResponse>::Create(channel_.get(), cq, rpcmethod_StreamAggregatedResources_, context, false, nullptr);
}

::grpc::ClientReaderWriter< ::envoy::api::v2::IncrementalDiscoveryRequest, ::envoy::api::v2::IncrementalDiscoveryResponse>* AggregatedDiscoveryService::Stub::IncrementalAggregatedResourcesRaw(::grpc::ClientContext* context) {
  return ::grpc::internal::ClientReaderWriterFactory< ::envoy::api::v2::IncrementalDiscoveryRequest, ::envoy::api::v2::IncrementalDiscoveryResponse>::Create(channel_.get(), rpcmethod_IncrementalAggregatedResources_, context);
}

void AggregatedDiscoveryService::Stub::experimental_async::IncrementalAggregatedResources(::grpc::ClientContext* context, ::grpc::experimental::ClientBidiReactor< ::envoy::api::v2::IncrementalDiscoveryRequest,::envoy::api::v2::IncrementalDiscoveryResponse>* reactor) {
  ::grpc::internal::ClientCallbackReaderWriterFactory< ::envoy::api::v2::IncrementalDiscoveryRequest,::envoy::api::v2::IncrementalDiscoveryResponse>::Create(stub_->channel_.get(), stub_->rpcmethod_IncrementalAggregatedResources_, context, reactor);
}

::grpc::ClientAsyncReaderWriter< ::envoy::api::v2::IncrementalDiscoveryRequest, ::envoy::api::v2::IncrementalDiscoveryResponse>* AggregatedDiscoveryService::Stub::AsyncIncrementalAggregatedResourcesRaw(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq, void* tag) {
  return ::grpc::internal::ClientAsyncReaderWriterFactory< ::envoy::api::v2::IncrementalDiscoveryRequest, ::envoy::api::v2::IncrementalDiscoveryResponse>::Create(channel_.get(), cq, rpcmethod_IncrementalAggregatedResources_, context, true, tag);
}

::grpc::ClientAsyncReaderWriter< ::envoy::api::v2::IncrementalDiscoveryRequest, ::envoy::api::v2::IncrementalDiscoveryResponse>* AggregatedDiscoveryService::Stub::PrepareAsyncIncrementalAggregatedResourcesRaw(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncReaderWriterFactory< ::envoy::api::v2::IncrementalDiscoveryRequest, ::envoy::api::v2::IncrementalDiscoveryResponse>::Create(channel_.get(), cq, rpcmethod_IncrementalAggregatedResources_, context, false, nullptr);
}

AggregatedDiscoveryService::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      AggregatedDiscoveryService_method_names[0],
      ::grpc::internal::RpcMethod::BIDI_STREAMING,
      new ::grpc::internal::BidiStreamingHandler< AggregatedDiscoveryService::Service, ::envoy::api::v2::DiscoveryRequest, ::envoy::api::v2::DiscoveryResponse>(
          std::mem_fn(&AggregatedDiscoveryService::Service::StreamAggregatedResources), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      AggregatedDiscoveryService_method_names[1],
      ::grpc::internal::RpcMethod::BIDI_STREAMING,
      new ::grpc::internal::BidiStreamingHandler< AggregatedDiscoveryService::Service, ::envoy::api::v2::IncrementalDiscoveryRequest, ::envoy::api::v2::IncrementalDiscoveryResponse>(
          std::mem_fn(&AggregatedDiscoveryService::Service::IncrementalAggregatedResources), this)));
}

AggregatedDiscoveryService::Service::~Service() {
}

::grpc::Status AggregatedDiscoveryService::Service::StreamAggregatedResources(::grpc::ServerContext* context, ::grpc::ServerReaderWriter< ::envoy::api::v2::DiscoveryResponse, ::envoy::api::v2::DiscoveryRequest>* stream) {
  (void) context;
  (void) stream;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status AggregatedDiscoveryService::Service::IncrementalAggregatedResources(::grpc::ServerContext* context, ::grpc::ServerReaderWriter< ::envoy::api::v2::IncrementalDiscoveryResponse, ::envoy::api::v2::IncrementalDiscoveryRequest>* stream) {
  (void) context;
  (void) stream;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace envoy
}  // namespace service
}  // namespace discovery
}  // namespace v2
