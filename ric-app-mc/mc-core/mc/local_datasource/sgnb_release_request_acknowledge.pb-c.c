/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: sgnb_release_request_acknowledge.proto */

/* Do not generate deprecated warnings for self */
#ifndef PROTOBUF_C__NO_DEPRECATED
#define PROTOBUF_C__NO_DEPRECATED
#endif

#include "sgnb_release_request_acknowledge.pb-c.h"
void   streaming_protobufs__sg_nbrelease_request_acknowledge__init
                     (StreamingProtobufs__SgNBReleaseRequestAcknowledge         *message)
{
  static const StreamingProtobufs__SgNBReleaseRequestAcknowledge init_value = STREAMING_PROTOBUFS__SG_NBRELEASE_REQUEST_ACKNOWLEDGE__INIT;
  *message = init_value;
}
size_t streaming_protobufs__sg_nbrelease_request_acknowledge__get_packed_size
                     (const StreamingProtobufs__SgNBReleaseRequestAcknowledge *message)
{
  assert(message->base.descriptor == &streaming_protobufs__sg_nbrelease_request_acknowledge__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t streaming_protobufs__sg_nbrelease_request_acknowledge__pack
                     (const StreamingProtobufs__SgNBReleaseRequestAcknowledge *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &streaming_protobufs__sg_nbrelease_request_acknowledge__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t streaming_protobufs__sg_nbrelease_request_acknowledge__pack_to_buffer
                     (const StreamingProtobufs__SgNBReleaseRequestAcknowledge *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &streaming_protobufs__sg_nbrelease_request_acknowledge__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
StreamingProtobufs__SgNBReleaseRequestAcknowledge *
       streaming_protobufs__sg_nbrelease_request_acknowledge__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (StreamingProtobufs__SgNBReleaseRequestAcknowledge *)
     protobuf_c_message_unpack (&streaming_protobufs__sg_nbrelease_request_acknowledge__descriptor,
                                allocator, len, data);
}
void   streaming_protobufs__sg_nbrelease_request_acknowledge__free_unpacked
                     (StreamingProtobufs__SgNBReleaseRequestAcknowledge *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &streaming_protobufs__sg_nbrelease_request_acknowledge__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
void   streaming_protobufs__sg_nbrelease_request_acknowledge__ies__init
                     (StreamingProtobufs__SgNBReleaseRequestAcknowledgeIEs         *message)
{
  static const StreamingProtobufs__SgNBReleaseRequestAcknowledgeIEs init_value = STREAMING_PROTOBUFS__SG_NBRELEASE_REQUEST_ACKNOWLEDGE__IES__INIT;
  *message = init_value;
}
size_t streaming_protobufs__sg_nbrelease_request_acknowledge__ies__get_packed_size
                     (const StreamingProtobufs__SgNBReleaseRequestAcknowledgeIEs *message)
{
  assert(message->base.descriptor == &streaming_protobufs__sg_nbrelease_request_acknowledge__ies__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t streaming_protobufs__sg_nbrelease_request_acknowledge__ies__pack
                     (const StreamingProtobufs__SgNBReleaseRequestAcknowledgeIEs *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &streaming_protobufs__sg_nbrelease_request_acknowledge__ies__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t streaming_protobufs__sg_nbrelease_request_acknowledge__ies__pack_to_buffer
                     (const StreamingProtobufs__SgNBReleaseRequestAcknowledgeIEs *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &streaming_protobufs__sg_nbrelease_request_acknowledge__ies__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
StreamingProtobufs__SgNBReleaseRequestAcknowledgeIEs *
       streaming_protobufs__sg_nbrelease_request_acknowledge__ies__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (StreamingProtobufs__SgNBReleaseRequestAcknowledgeIEs *)
     protobuf_c_message_unpack (&streaming_protobufs__sg_nbrelease_request_acknowledge__ies__descriptor,
                                allocator, len, data);
}
void   streaming_protobufs__sg_nbrelease_request_acknowledge__ies__free_unpacked
                     (StreamingProtobufs__SgNBReleaseRequestAcknowledgeIEs *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &streaming_protobufs__sg_nbrelease_request_acknowledge__ies__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
void   streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack_list__init
                     (StreamingProtobufs__ERABsAdmittedToBeReleasedSgNBRelReqAckList         *message)
{
  static const StreamingProtobufs__ERABsAdmittedToBeReleasedSgNBRelReqAckList init_value = STREAMING_PROTOBUFS__E__RABS__ADMITTED__TO_BE_RELEASED__SG_NBREL_REQ_ACK_LIST__INIT;
  *message = init_value;
}
size_t streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack_list__get_packed_size
                     (const StreamingProtobufs__ERABsAdmittedToBeReleasedSgNBRelReqAckList *message)
{
  assert(message->base.descriptor == &streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack_list__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack_list__pack
                     (const StreamingProtobufs__ERABsAdmittedToBeReleasedSgNBRelReqAckList *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack_list__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack_list__pack_to_buffer
                     (const StreamingProtobufs__ERABsAdmittedToBeReleasedSgNBRelReqAckList *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack_list__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
StreamingProtobufs__ERABsAdmittedToBeReleasedSgNBRelReqAckList *
       streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack_list__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (StreamingProtobufs__ERABsAdmittedToBeReleasedSgNBRelReqAckList *)
     protobuf_c_message_unpack (&streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack_list__descriptor,
                                allocator, len, data);
}
void   streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack_list__free_unpacked
                     (StreamingProtobufs__ERABsAdmittedToBeReleasedSgNBRelReqAckList *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack_list__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
void   streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack__item__init
                     (StreamingProtobufs__ERABsAdmittedToBeReleasedSgNBRelReqAckItem         *message)
{
  static const StreamingProtobufs__ERABsAdmittedToBeReleasedSgNBRelReqAckItem init_value = STREAMING_PROTOBUFS__E__RABS__ADMITTED__TO_BE_RELEASED__SG_NBREL_REQ_ACK__ITEM__INIT;
  *message = init_value;
}
size_t streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack__item__get_packed_size
                     (const StreamingProtobufs__ERABsAdmittedToBeReleasedSgNBRelReqAckItem *message)
{
  assert(message->base.descriptor == &streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack__item__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack__item__pack
                     (const StreamingProtobufs__ERABsAdmittedToBeReleasedSgNBRelReqAckItem *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack__item__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack__item__pack_to_buffer
                     (const StreamingProtobufs__ERABsAdmittedToBeReleasedSgNBRelReqAckItem *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack__item__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
StreamingProtobufs__ERABsAdmittedToBeReleasedSgNBRelReqAckItem *
       streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack__item__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (StreamingProtobufs__ERABsAdmittedToBeReleasedSgNBRelReqAckItem *)
     protobuf_c_message_unpack (&streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack__item__descriptor,
                                allocator, len, data);
}
void   streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack__item__free_unpacked
                     (StreamingProtobufs__ERABsAdmittedToBeReleasedSgNBRelReqAckItem *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack__item__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
void   streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack__item_ext_ies__init
                     (StreamingProtobufs__ERABsAdmittedToBeReleasedSgNBRelReqAckItemExtIEs         *message)
{
  static const StreamingProtobufs__ERABsAdmittedToBeReleasedSgNBRelReqAckItemExtIEs init_value = STREAMING_PROTOBUFS__E__RABS__ADMITTED__TO_BE_RELEASED__SG_NBREL_REQ_ACK__ITEM_EXT_IES__INIT;
  *message = init_value;
}
size_t streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack__item_ext_ies__get_packed_size
                     (const StreamingProtobufs__ERABsAdmittedToBeReleasedSgNBRelReqAckItemExtIEs *message)
{
  assert(message->base.descriptor == &streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack__item_ext_ies__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack__item_ext_ies__pack
                     (const StreamingProtobufs__ERABsAdmittedToBeReleasedSgNBRelReqAckItemExtIEs *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack__item_ext_ies__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack__item_ext_ies__pack_to_buffer
                     (const StreamingProtobufs__ERABsAdmittedToBeReleasedSgNBRelReqAckItemExtIEs *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack__item_ext_ies__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
StreamingProtobufs__ERABsAdmittedToBeReleasedSgNBRelReqAckItemExtIEs *
       streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack__item_ext_ies__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (StreamingProtobufs__ERABsAdmittedToBeReleasedSgNBRelReqAckItemExtIEs *)
     protobuf_c_message_unpack (&streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack__item_ext_ies__descriptor,
                                allocator, len, data);
}
void   streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack__item_ext_ies__free_unpacked
                     (StreamingProtobufs__ERABsAdmittedToBeReleasedSgNBRelReqAckItemExtIEs *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack__item_ext_ies__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
static const ProtobufCFieldDescriptor streaming_protobufs__sg_nbrelease_request_acknowledge__field_descriptors[1] =
{
  {
    "protocolIEs",
    1,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_MESSAGE,
    0,   /* quantifier_offset */
    offsetof(StreamingProtobufs__SgNBReleaseRequestAcknowledge, protocolies),
    &streaming_protobufs__sg_nbrelease_request_acknowledge__ies__descriptor,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned streaming_protobufs__sg_nbrelease_request_acknowledge__field_indices_by_name[] = {
  0,   /* field[0] = protocolIEs */
};
static const ProtobufCIntRange streaming_protobufs__sg_nbrelease_request_acknowledge__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 1 }
};
const ProtobufCMessageDescriptor streaming_protobufs__sg_nbrelease_request_acknowledge__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "streaming_protobufs.SgNBReleaseRequestAcknowledge",
  "SgNBReleaseRequestAcknowledge",
  "StreamingProtobufs__SgNBReleaseRequestAcknowledge",
  "streaming_protobufs",
  sizeof(StreamingProtobufs__SgNBReleaseRequestAcknowledge),
  1,
  streaming_protobufs__sg_nbrelease_request_acknowledge__field_descriptors,
  streaming_protobufs__sg_nbrelease_request_acknowledge__field_indices_by_name,
  1,  streaming_protobufs__sg_nbrelease_request_acknowledge__number_ranges,
  (ProtobufCMessageInit) streaming_protobufs__sg_nbrelease_request_acknowledge__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor streaming_protobufs__sg_nbrelease_request_acknowledge__ies__field_descriptors[5] =
{
  {
    "id_MeNB_UE_X2AP_ID",
    1,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_UINT32,
    0,   /* quantifier_offset */
    offsetof(StreamingProtobufs__SgNBReleaseRequestAcknowledgeIEs, id_menb_ue_x2ap_id),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "id_SgNB_UE_X2AP_ID",
    2,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_UINT32,
    0,   /* quantifier_offset */
    offsetof(StreamingProtobufs__SgNBReleaseRequestAcknowledgeIEs, id_sgnb_ue_x2ap_id),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "id_CriticalityDiagnostics",
    3,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_MESSAGE,
    0,   /* quantifier_offset */
    offsetof(StreamingProtobufs__SgNBReleaseRequestAcknowledgeIEs, id_criticalitydiagnostics),
    &streaming_protobufs__criticality_diagnostics__descriptor,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "id_MeNB_UE_X2AP_ID_Extension",
    4,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_MESSAGE,
    0,   /* quantifier_offset */
    offsetof(StreamingProtobufs__SgNBReleaseRequestAcknowledgeIEs, id_menb_ue_x2ap_id_extension),
    &google__protobuf__uint32_value__descriptor,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "id_E_RABs_Admitted_ToBeReleased_SgNBRelReqAckList",
    5,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_MESSAGE,
    0,   /* quantifier_offset */
    offsetof(StreamingProtobufs__SgNBReleaseRequestAcknowledgeIEs, id_e_rabs_admitted_tobereleased_sgnbrelreqacklist),
    &streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack_list__descriptor,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned streaming_protobufs__sg_nbrelease_request_acknowledge__ies__field_indices_by_name[] = {
  2,   /* field[2] = id_CriticalityDiagnostics */
  4,   /* field[4] = id_E_RABs_Admitted_ToBeReleased_SgNBRelReqAckList */
  0,   /* field[0] = id_MeNB_UE_X2AP_ID */
  3,   /* field[3] = id_MeNB_UE_X2AP_ID_Extension */
  1,   /* field[1] = id_SgNB_UE_X2AP_ID */
};
static const ProtobufCIntRange streaming_protobufs__sg_nbrelease_request_acknowledge__ies__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 5 }
};
const ProtobufCMessageDescriptor streaming_protobufs__sg_nbrelease_request_acknowledge__ies__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "streaming_protobufs.SgNBReleaseRequestAcknowledge_IEs",
  "SgNBReleaseRequestAcknowledgeIEs",
  "StreamingProtobufs__SgNBReleaseRequestAcknowledgeIEs",
  "streaming_protobufs",
  sizeof(StreamingProtobufs__SgNBReleaseRequestAcknowledgeIEs),
  5,
  streaming_protobufs__sg_nbrelease_request_acknowledge__ies__field_descriptors,
  streaming_protobufs__sg_nbrelease_request_acknowledge__ies__field_indices_by_name,
  1,  streaming_protobufs__sg_nbrelease_request_acknowledge__ies__number_ranges,
  (ProtobufCMessageInit) streaming_protobufs__sg_nbrelease_request_acknowledge__ies__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack_list__field_descriptors[1] =
{
  {
    "id_E_RABs_Admitted_ToBeReleased_SgNBRelReqAck_Item",
    1,
    PROTOBUF_C_LABEL_REPEATED,
    PROTOBUF_C_TYPE_MESSAGE,
    offsetof(StreamingProtobufs__ERABsAdmittedToBeReleasedSgNBRelReqAckList, n_id_e_rabs_admitted_tobereleased_sgnbrelreqack_item),
    offsetof(StreamingProtobufs__ERABsAdmittedToBeReleasedSgNBRelReqAckList, id_e_rabs_admitted_tobereleased_sgnbrelreqack_item),
    &streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack__item__descriptor,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack_list__field_indices_by_name[] = {
  0,   /* field[0] = id_E_RABs_Admitted_ToBeReleased_SgNBRelReqAck_Item */
};
static const ProtobufCIntRange streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack_list__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 1 }
};
const ProtobufCMessageDescriptor streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack_list__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "streaming_protobufs.E_RABs_Admitted_ToBeReleased_SgNBRelReqAckList",
  "ERABsAdmittedToBeReleasedSgNBRelReqAckList",
  "StreamingProtobufs__ERABsAdmittedToBeReleasedSgNBRelReqAckList",
  "streaming_protobufs",
  sizeof(StreamingProtobufs__ERABsAdmittedToBeReleasedSgNBRelReqAckList),
  1,
  streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack_list__field_descriptors,
  streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack_list__field_indices_by_name,
  1,  streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack_list__number_ranges,
  (ProtobufCMessageInit) streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack_list__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack__item__field_descriptors[3] =
{
  {
    "e_RAB_ID",
    1,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_UINT32,
    0,   /* quantifier_offset */
    offsetof(StreamingProtobufs__ERABsAdmittedToBeReleasedSgNBRelReqAckItem, e_rab_id),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "rlc_Mode_transferred",
    2,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_MESSAGE,
    0,   /* quantifier_offset */
    offsetof(StreamingProtobufs__ERABsAdmittedToBeReleasedSgNBRelReqAckItem, rlc_mode_transferred),
    &streaming_protobufs__rlcmode__descriptor,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "iE_Extensions",
    3,
    PROTOBUF_C_LABEL_REPEATED,
    PROTOBUF_C_TYPE_MESSAGE,
    offsetof(StreamingProtobufs__ERABsAdmittedToBeReleasedSgNBRelReqAckItem, n_ie_extensions),
    offsetof(StreamingProtobufs__ERABsAdmittedToBeReleasedSgNBRelReqAckItem, ie_extensions),
    &streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack__item_ext_ies__descriptor,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack__item__field_indices_by_name[] = {
  0,   /* field[0] = e_RAB_ID */
  2,   /* field[2] = iE_Extensions */
  1,   /* field[1] = rlc_Mode_transferred */
};
static const ProtobufCIntRange streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack__item__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 3 }
};
const ProtobufCMessageDescriptor streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack__item__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "streaming_protobufs.E_RABs_Admitted_ToBeReleased_SgNBRelReqAck_Item",
  "ERABsAdmittedToBeReleasedSgNBRelReqAckItem",
  "StreamingProtobufs__ERABsAdmittedToBeReleasedSgNBRelReqAckItem",
  "streaming_protobufs",
  sizeof(StreamingProtobufs__ERABsAdmittedToBeReleasedSgNBRelReqAckItem),
  3,
  streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack__item__field_descriptors,
  streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack__item__field_indices_by_name,
  1,  streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack__item__number_ranges,
  (ProtobufCMessageInit) streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack__item__init,
  NULL,NULL,NULL    /* reserved[123] */
};
#define streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack__item_ext_ies__field_descriptors NULL
#define streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack__item_ext_ies__field_indices_by_name NULL
#define streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack__item_ext_ies__number_ranges NULL
const ProtobufCMessageDescriptor streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack__item_ext_ies__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "streaming_protobufs.E_RABs_Admitted_ToBeReleased_SgNBRelReqAck_ItemExtIEs",
  "ERABsAdmittedToBeReleasedSgNBRelReqAckItemExtIEs",
  "StreamingProtobufs__ERABsAdmittedToBeReleasedSgNBRelReqAckItemExtIEs",
  "streaming_protobufs",
  sizeof(StreamingProtobufs__ERABsAdmittedToBeReleasedSgNBRelReqAckItemExtIEs),
  0,
  streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack__item_ext_ies__field_descriptors,
  streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack__item_ext_ies__field_indices_by_name,
  0,  streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack__item_ext_ies__number_ranges,
  (ProtobufCMessageInit) streaming_protobufs__e__rabs__admitted__to_be_released__sg_nbrel_req_ack__item_ext_ies__init,
  NULL,NULL,NULL    /* reserved[123] */
};
