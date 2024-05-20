// Code generated by protoc-gen-go. DO NOT EDIT.
// source: secondary_rat_data_usage_report.proto

package streaming_protobufs

import (
	fmt "fmt"
	proto "github.com/golang/protobuf/proto"
	wrappers "github.com/golang/protobuf/ptypes/wrappers"
	math "math"
)

// Reference imports to suppress errors if they are not otherwise used.
var _ = proto.Marshal
var _ = fmt.Errorf
var _ = math.Inf

// This is a compile-time assertion to ensure that this generated file
// is compatible with the proto package it is being compiled against.
// A compilation error at this line likely means your copy of the
// proto package needs to be updated.
const _ = proto.ProtoPackageIsVersion3 // please upgrade the proto package

type SecondaryRATUsageReport_Item_SecondaryRATType int32

const (
	SecondaryRATUsageReport_Item_protobuf_unspecified SecondaryRATUsageReport_Item_SecondaryRATType = 0
	SecondaryRATUsageReport_Item_nr                   SecondaryRATUsageReport_Item_SecondaryRATType = 1
)

var SecondaryRATUsageReport_Item_SecondaryRATType_name = map[int32]string{
	0: "protobuf_unspecified",
	1: "nr",
}

var SecondaryRATUsageReport_Item_SecondaryRATType_value = map[string]int32{
	"protobuf_unspecified": 0,
	"nr":                   1,
}

func (x SecondaryRATUsageReport_Item_SecondaryRATType) String() string {
	return proto.EnumName(SecondaryRATUsageReport_Item_SecondaryRATType_name, int32(x))
}

func (SecondaryRATUsageReport_Item_SecondaryRATType) EnumDescriptor() ([]byte, []int) {
	return fileDescriptor_6cfaa22c59a9cea1, []int{4, 0}
}

type SecondaryRATDataUsageReport struct {
	ProtocolIEs          *SecondaryRATDataUsageReport_IEs `protobuf:"bytes,1,opt,name=protocolIEs,proto3" json:"protocolIEs,omitempty"`
	XXX_NoUnkeyedLiteral struct{}                         `json:"-"`
	XXX_unrecognized     []byte                           `json:"-"`
	XXX_sizecache        int32                            `json:"-"`
}

func (m *SecondaryRATDataUsageReport) Reset()         { *m = SecondaryRATDataUsageReport{} }
func (m *SecondaryRATDataUsageReport) String() string { return proto.CompactTextString(m) }
func (*SecondaryRATDataUsageReport) ProtoMessage()    {}
func (*SecondaryRATDataUsageReport) Descriptor() ([]byte, []int) {
	return fileDescriptor_6cfaa22c59a9cea1, []int{0}
}

func (m *SecondaryRATDataUsageReport) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_SecondaryRATDataUsageReport.Unmarshal(m, b)
}
func (m *SecondaryRATDataUsageReport) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_SecondaryRATDataUsageReport.Marshal(b, m, deterministic)
}
func (m *SecondaryRATDataUsageReport) XXX_Merge(src proto.Message) {
	xxx_messageInfo_SecondaryRATDataUsageReport.Merge(m, src)
}
func (m *SecondaryRATDataUsageReport) XXX_Size() int {
	return xxx_messageInfo_SecondaryRATDataUsageReport.Size(m)
}
func (m *SecondaryRATDataUsageReport) XXX_DiscardUnknown() {
	xxx_messageInfo_SecondaryRATDataUsageReport.DiscardUnknown(m)
}

var xxx_messageInfo_SecondaryRATDataUsageReport proto.InternalMessageInfo

func (m *SecondaryRATDataUsageReport) GetProtocolIEs() *SecondaryRATDataUsageReport_IEs {
	if m != nil {
		return m.ProtocolIEs
	}
	return nil
}

type SecondaryRATDataUsageReport_IEs struct {
	Id_MeNB_UE_X2AP_ID             uint32                       `protobuf:"varint,1,opt,name=id_MeNB_UE_X2AP_ID,json=idMeNBUEX2APID,proto3" json:"id_MeNB_UE_X2AP_ID,omitempty"`
	Id_SgNB_UE_X2AP_ID             uint32                       `protobuf:"varint,2,opt,name=id_SgNB_UE_X2AP_ID,json=idSgNBUEX2APID,proto3" json:"id_SgNB_UE_X2AP_ID,omitempty"`
	Id_SecondaryRATUsageReportList *SecondaryRATUsageReportList `protobuf:"bytes,3,opt,name=id_SecondaryRATUsageReportList,json=idSecondaryRATUsageReportList,proto3" json:"id_SecondaryRATUsageReportList,omitempty"`
	Id_MeNB_UE_X2AP_ID_Extension   *wrappers.UInt32Value        `protobuf:"bytes,4,opt,name=id_MeNB_UE_X2AP_ID_Extension,json=idMeNBUEX2APIDExtension,proto3" json:"id_MeNB_UE_X2AP_ID_Extension,omitempty"`
	XXX_NoUnkeyedLiteral           struct{}                     `json:"-"`
	XXX_unrecognized               []byte                       `json:"-"`
	XXX_sizecache                  int32                        `json:"-"`
}

func (m *SecondaryRATDataUsageReport_IEs) Reset()         { *m = SecondaryRATDataUsageReport_IEs{} }
func (m *SecondaryRATDataUsageReport_IEs) String() string { return proto.CompactTextString(m) }
func (*SecondaryRATDataUsageReport_IEs) ProtoMessage()    {}
func (*SecondaryRATDataUsageReport_IEs) Descriptor() ([]byte, []int) {
	return fileDescriptor_6cfaa22c59a9cea1, []int{1}
}

func (m *SecondaryRATDataUsageReport_IEs) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_SecondaryRATDataUsageReport_IEs.Unmarshal(m, b)
}
func (m *SecondaryRATDataUsageReport_IEs) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_SecondaryRATDataUsageReport_IEs.Marshal(b, m, deterministic)
}
func (m *SecondaryRATDataUsageReport_IEs) XXX_Merge(src proto.Message) {
	xxx_messageInfo_SecondaryRATDataUsageReport_IEs.Merge(m, src)
}
func (m *SecondaryRATDataUsageReport_IEs) XXX_Size() int {
	return xxx_messageInfo_SecondaryRATDataUsageReport_IEs.Size(m)
}
func (m *SecondaryRATDataUsageReport_IEs) XXX_DiscardUnknown() {
	xxx_messageInfo_SecondaryRATDataUsageReport_IEs.DiscardUnknown(m)
}

var xxx_messageInfo_SecondaryRATDataUsageReport_IEs proto.InternalMessageInfo

func (m *SecondaryRATDataUsageReport_IEs) GetId_MeNB_UE_X2AP_ID() uint32 {
	if m != nil {
		return m.Id_MeNB_UE_X2AP_ID
	}
	return 0
}

func (m *SecondaryRATDataUsageReport_IEs) GetId_SgNB_UE_X2AP_ID() uint32 {
	if m != nil {
		return m.Id_SgNB_UE_X2AP_ID
	}
	return 0
}

func (m *SecondaryRATDataUsageReport_IEs) GetId_SecondaryRATUsageReportList() *SecondaryRATUsageReportList {
	if m != nil {
		return m.Id_SecondaryRATUsageReportList
	}
	return nil
}

func (m *SecondaryRATDataUsageReport_IEs) GetId_MeNB_UE_X2AP_ID_Extension() *wrappers.UInt32Value {
	if m != nil {
		return m.Id_MeNB_UE_X2AP_ID_Extension
	}
	return nil
}

type SecondaryRATUsageReportList struct {
	Items                []*SecondaryRATUsageReport_ItemIEs `protobuf:"bytes,1,rep,name=items,proto3" json:"items,omitempty"`
	XXX_NoUnkeyedLiteral struct{}                           `json:"-"`
	XXX_unrecognized     []byte                             `json:"-"`
	XXX_sizecache        int32                              `json:"-"`
}

func (m *SecondaryRATUsageReportList) Reset()         { *m = SecondaryRATUsageReportList{} }
func (m *SecondaryRATUsageReportList) String() string { return proto.CompactTextString(m) }
func (*SecondaryRATUsageReportList) ProtoMessage()    {}
func (*SecondaryRATUsageReportList) Descriptor() ([]byte, []int) {
	return fileDescriptor_6cfaa22c59a9cea1, []int{2}
}

func (m *SecondaryRATUsageReportList) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_SecondaryRATUsageReportList.Unmarshal(m, b)
}
func (m *SecondaryRATUsageReportList) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_SecondaryRATUsageReportList.Marshal(b, m, deterministic)
}
func (m *SecondaryRATUsageReportList) XXX_Merge(src proto.Message) {
	xxx_messageInfo_SecondaryRATUsageReportList.Merge(m, src)
}
func (m *SecondaryRATUsageReportList) XXX_Size() int {
	return xxx_messageInfo_SecondaryRATUsageReportList.Size(m)
}
func (m *SecondaryRATUsageReportList) XXX_DiscardUnknown() {
	xxx_messageInfo_SecondaryRATUsageReportList.DiscardUnknown(m)
}

var xxx_messageInfo_SecondaryRATUsageReportList proto.InternalMessageInfo

func (m *SecondaryRATUsageReportList) GetItems() []*SecondaryRATUsageReport_ItemIEs {
	if m != nil {
		return m.Items
	}
	return nil
}

type SecondaryRATUsageReport_ItemIEs struct {
	Id_SecondaryRATUsageReport_Item *SecondaryRATUsageReport_Item `protobuf:"bytes,1,opt,name=id_SecondaryRATUsageReport_Item,json=idSecondaryRATUsageReportItem,proto3" json:"id_SecondaryRATUsageReport_Item,omitempty"`
	XXX_NoUnkeyedLiteral            struct{}                      `json:"-"`
	XXX_unrecognized                []byte                        `json:"-"`
	XXX_sizecache                   int32                         `json:"-"`
}

func (m *SecondaryRATUsageReport_ItemIEs) Reset()         { *m = SecondaryRATUsageReport_ItemIEs{} }
func (m *SecondaryRATUsageReport_ItemIEs) String() string { return proto.CompactTextString(m) }
func (*SecondaryRATUsageReport_ItemIEs) ProtoMessage()    {}
func (*SecondaryRATUsageReport_ItemIEs) Descriptor() ([]byte, []int) {
	return fileDescriptor_6cfaa22c59a9cea1, []int{3}
}

func (m *SecondaryRATUsageReport_ItemIEs) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_SecondaryRATUsageReport_ItemIEs.Unmarshal(m, b)
}
func (m *SecondaryRATUsageReport_ItemIEs) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_SecondaryRATUsageReport_ItemIEs.Marshal(b, m, deterministic)
}
func (m *SecondaryRATUsageReport_ItemIEs) XXX_Merge(src proto.Message) {
	xxx_messageInfo_SecondaryRATUsageReport_ItemIEs.Merge(m, src)
}
func (m *SecondaryRATUsageReport_ItemIEs) XXX_Size() int {
	return xxx_messageInfo_SecondaryRATUsageReport_ItemIEs.Size(m)
}
func (m *SecondaryRATUsageReport_ItemIEs) XXX_DiscardUnknown() {
	xxx_messageInfo_SecondaryRATUsageReport_ItemIEs.DiscardUnknown(m)
}

var xxx_messageInfo_SecondaryRATUsageReport_ItemIEs proto.InternalMessageInfo

func (m *SecondaryRATUsageReport_ItemIEs) GetId_SecondaryRATUsageReport_Item() *SecondaryRATUsageReport_Item {
	if m != nil {
		return m.Id_SecondaryRATUsageReport_Item
	}
	return nil
}

type SecondaryRATUsageReport_Item struct {
	E_RAB_ID             uint32                                        `protobuf:"varint,1,opt,name=e_RAB_ID,json=eRABID,proto3" json:"e_RAB_ID,omitempty"`
	SecondaryRATType     SecondaryRATUsageReport_Item_SecondaryRATType `protobuf:"varint,2,opt,name=secondaryRATType,proto3,enum=streaming_protobufs.SecondaryRATUsageReport_Item_SecondaryRATType" json:"secondaryRATType,omitempty"`
	E_RABUsageReportList *E_RABUsageReportList                         `protobuf:"bytes,3,opt,name=e_RABUsageReportList,json=eRABUsageReportList,proto3" json:"e_RABUsageReportList,omitempty"`
	XXX_NoUnkeyedLiteral struct{}                                      `json:"-"`
	XXX_unrecognized     []byte                                        `json:"-"`
	XXX_sizecache        int32                                         `json:"-"`
}

func (m *SecondaryRATUsageReport_Item) Reset()         { *m = SecondaryRATUsageReport_Item{} }
func (m *SecondaryRATUsageReport_Item) String() string { return proto.CompactTextString(m) }
func (*SecondaryRATUsageReport_Item) ProtoMessage()    {}
func (*SecondaryRATUsageReport_Item) Descriptor() ([]byte, []int) {
	return fileDescriptor_6cfaa22c59a9cea1, []int{4}
}

func (m *SecondaryRATUsageReport_Item) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_SecondaryRATUsageReport_Item.Unmarshal(m, b)
}
func (m *SecondaryRATUsageReport_Item) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_SecondaryRATUsageReport_Item.Marshal(b, m, deterministic)
}
func (m *SecondaryRATUsageReport_Item) XXX_Merge(src proto.Message) {
	xxx_messageInfo_SecondaryRATUsageReport_Item.Merge(m, src)
}
func (m *SecondaryRATUsageReport_Item) XXX_Size() int {
	return xxx_messageInfo_SecondaryRATUsageReport_Item.Size(m)
}
func (m *SecondaryRATUsageReport_Item) XXX_DiscardUnknown() {
	xxx_messageInfo_SecondaryRATUsageReport_Item.DiscardUnknown(m)
}

var xxx_messageInfo_SecondaryRATUsageReport_Item proto.InternalMessageInfo

func (m *SecondaryRATUsageReport_Item) GetE_RAB_ID() uint32 {
	if m != nil {
		return m.E_RAB_ID
	}
	return 0
}

func (m *SecondaryRATUsageReport_Item) GetSecondaryRATType() SecondaryRATUsageReport_Item_SecondaryRATType {
	if m != nil {
		return m.SecondaryRATType
	}
	return SecondaryRATUsageReport_Item_protobuf_unspecified
}

func (m *SecondaryRATUsageReport_Item) GetE_RABUsageReportList() *E_RABUsageReportList {
	if m != nil {
		return m.E_RABUsageReportList
	}
	return nil
}

type E_RABUsageReportList struct {
	Items                []*E_RABUsageReport_ItemIEs `protobuf:"bytes,1,rep,name=items,proto3" json:"items,omitempty"`
	XXX_NoUnkeyedLiteral struct{}                    `json:"-"`
	XXX_unrecognized     []byte                      `json:"-"`
	XXX_sizecache        int32                       `json:"-"`
}

func (m *E_RABUsageReportList) Reset()         { *m = E_RABUsageReportList{} }
func (m *E_RABUsageReportList) String() string { return proto.CompactTextString(m) }
func (*E_RABUsageReportList) ProtoMessage()    {}
func (*E_RABUsageReportList) Descriptor() ([]byte, []int) {
	return fileDescriptor_6cfaa22c59a9cea1, []int{5}
}

func (m *E_RABUsageReportList) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_E_RABUsageReportList.Unmarshal(m, b)
}
func (m *E_RABUsageReportList) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_E_RABUsageReportList.Marshal(b, m, deterministic)
}
func (m *E_RABUsageReportList) XXX_Merge(src proto.Message) {
	xxx_messageInfo_E_RABUsageReportList.Merge(m, src)
}
func (m *E_RABUsageReportList) XXX_Size() int {
	return xxx_messageInfo_E_RABUsageReportList.Size(m)
}
func (m *E_RABUsageReportList) XXX_DiscardUnknown() {
	xxx_messageInfo_E_RABUsageReportList.DiscardUnknown(m)
}

var xxx_messageInfo_E_RABUsageReportList proto.InternalMessageInfo

func (m *E_RABUsageReportList) GetItems() []*E_RABUsageReport_ItemIEs {
	if m != nil {
		return m.Items
	}
	return nil
}

type E_RABUsageReport_ItemIEs struct {
	Id_E_RABUsageReport_Item *E_RABUsageReport_Item `protobuf:"bytes,1,opt,name=id_E_RABUsageReport_Item,json=idERABUsageReportItem,proto3" json:"id_E_RABUsageReport_Item,omitempty"`
	XXX_NoUnkeyedLiteral     struct{}               `json:"-"`
	XXX_unrecognized         []byte                 `json:"-"`
	XXX_sizecache            int32                  `json:"-"`
}

func (m *E_RABUsageReport_ItemIEs) Reset()         { *m = E_RABUsageReport_ItemIEs{} }
func (m *E_RABUsageReport_ItemIEs) String() string { return proto.CompactTextString(m) }
func (*E_RABUsageReport_ItemIEs) ProtoMessage()    {}
func (*E_RABUsageReport_ItemIEs) Descriptor() ([]byte, []int) {
	return fileDescriptor_6cfaa22c59a9cea1, []int{6}
}

func (m *E_RABUsageReport_ItemIEs) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_E_RABUsageReport_ItemIEs.Unmarshal(m, b)
}
func (m *E_RABUsageReport_ItemIEs) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_E_RABUsageReport_ItemIEs.Marshal(b, m, deterministic)
}
func (m *E_RABUsageReport_ItemIEs) XXX_Merge(src proto.Message) {
	xxx_messageInfo_E_RABUsageReport_ItemIEs.Merge(m, src)
}
func (m *E_RABUsageReport_ItemIEs) XXX_Size() int {
	return xxx_messageInfo_E_RABUsageReport_ItemIEs.Size(m)
}
func (m *E_RABUsageReport_ItemIEs) XXX_DiscardUnknown() {
	xxx_messageInfo_E_RABUsageReport_ItemIEs.DiscardUnknown(m)
}

var xxx_messageInfo_E_RABUsageReport_ItemIEs proto.InternalMessageInfo

func (m *E_RABUsageReport_ItemIEs) GetId_E_RABUsageReport_Item() *E_RABUsageReport_Item {
	if m != nil {
		return m.Id_E_RABUsageReport_Item
	}
	return nil
}

type E_RABUsageReport_Item struct {
	StartTimeStamp       uint64   `protobuf:"varint,1,opt,name=startTimeStamp,proto3" json:"startTimeStamp,omitempty"`
	EndTimeStamp         uint64   `protobuf:"varint,2,opt,name=endTimeStamp,proto3" json:"endTimeStamp,omitempty"`
	UsageCountUL         uint64   `protobuf:"varint,3,opt,name=usageCountUL,proto3" json:"usageCountUL,omitempty"`
	UsageCountDL         uint64   `protobuf:"varint,4,opt,name=usageCountDL,proto3" json:"usageCountDL,omitempty"`
	XXX_NoUnkeyedLiteral struct{} `json:"-"`
	XXX_unrecognized     []byte   `json:"-"`
	XXX_sizecache        int32    `json:"-"`
}

func (m *E_RABUsageReport_Item) Reset()         { *m = E_RABUsageReport_Item{} }
func (m *E_RABUsageReport_Item) String() string { return proto.CompactTextString(m) }
func (*E_RABUsageReport_Item) ProtoMessage()    {}
func (*E_RABUsageReport_Item) Descriptor() ([]byte, []int) {
	return fileDescriptor_6cfaa22c59a9cea1, []int{7}
}

func (m *E_RABUsageReport_Item) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_E_RABUsageReport_Item.Unmarshal(m, b)
}
func (m *E_RABUsageReport_Item) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_E_RABUsageReport_Item.Marshal(b, m, deterministic)
}
func (m *E_RABUsageReport_Item) XXX_Merge(src proto.Message) {
	xxx_messageInfo_E_RABUsageReport_Item.Merge(m, src)
}
func (m *E_RABUsageReport_Item) XXX_Size() int {
	return xxx_messageInfo_E_RABUsageReport_Item.Size(m)
}
func (m *E_RABUsageReport_Item) XXX_DiscardUnknown() {
	xxx_messageInfo_E_RABUsageReport_Item.DiscardUnknown(m)
}

var xxx_messageInfo_E_RABUsageReport_Item proto.InternalMessageInfo

func (m *E_RABUsageReport_Item) GetStartTimeStamp() uint64 {
	if m != nil {
		return m.StartTimeStamp
	}
	return 0
}

func (m *E_RABUsageReport_Item) GetEndTimeStamp() uint64 {
	if m != nil {
		return m.EndTimeStamp
	}
	return 0
}

func (m *E_RABUsageReport_Item) GetUsageCountUL() uint64 {
	if m != nil {
		return m.UsageCountUL
	}
	return 0
}

func (m *E_RABUsageReport_Item) GetUsageCountDL() uint64 {
	if m != nil {
		return m.UsageCountDL
	}
	return 0
}

func init() {
	proto.RegisterEnum("streaming_protobufs.SecondaryRATUsageReport_Item_SecondaryRATType", SecondaryRATUsageReport_Item_SecondaryRATType_name, SecondaryRATUsageReport_Item_SecondaryRATType_value)
	proto.RegisterType((*SecondaryRATDataUsageReport)(nil), "streaming_protobufs.SecondaryRATDataUsageReport")
	proto.RegisterType((*SecondaryRATDataUsageReport_IEs)(nil), "streaming_protobufs.SecondaryRATDataUsageReport_IEs")
	proto.RegisterType((*SecondaryRATUsageReportList)(nil), "streaming_protobufs.SecondaryRATUsageReportList")
	proto.RegisterType((*SecondaryRATUsageReport_ItemIEs)(nil), "streaming_protobufs.SecondaryRATUsageReport_ItemIEs")
	proto.RegisterType((*SecondaryRATUsageReport_Item)(nil), "streaming_protobufs.SecondaryRATUsageReport_Item")
	proto.RegisterType((*E_RABUsageReportList)(nil), "streaming_protobufs.E_RABUsageReportList")
	proto.RegisterType((*E_RABUsageReport_ItemIEs)(nil), "streaming_protobufs.E_RABUsageReport_ItemIEs")
	proto.RegisterType((*E_RABUsageReport_Item)(nil), "streaming_protobufs.E_RABUsageReport_Item")
}

func init() {
	proto.RegisterFile("secondary_rat_data_usage_report.proto", fileDescriptor_6cfaa22c59a9cea1)
}

var fileDescriptor_6cfaa22c59a9cea1 = []byte{
	// 577 bytes of a gzipped FileDescriptorProto
	0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xff, 0x94, 0x54, 0xef, 0x6e, 0xda, 0x3e,
	0x14, 0xfd, 0x41, 0xfb, 0x43, 0xd3, 0xed, 0x86, 0x90, 0x4b, 0xb5, 0x68, 0x63, 0xed, 0x14, 0x69,
	0xd3, 0x56, 0x89, 0xb0, 0x42, 0x5f, 0x00, 0x9a, 0x7c, 0xc8, 0xc4, 0xa6, 0xc9, 0x40, 0x35, 0xad,
	0x93, 0x2c, 0x37, 0x71, 0x23, 0x4b, 0x90, 0x44, 0xb6, 0xa3, 0x8e, 0x4f, 0x7b, 0x91, 0xbd, 0xc1,
	0x5e, 0x61, 0xef, 0xb0, 0x57, 0x9a, 0x62, 0xfe, 0x14, 0x43, 0x4a, 0xe9, 0x37, 0x74, 0xef, 0xb9,
	0xe7, 0x5c, 0xdf, 0x73, 0x08, 0xbc, 0x91, 0x2c, 0x48, 0xe2, 0x90, 0x8a, 0x29, 0x11, 0x54, 0x91,
	0x90, 0x2a, 0x4a, 0x32, 0x49, 0x23, 0x46, 0x04, 0x4b, 0x13, 0xa1, 0x9c, 0x54, 0x24, 0x2a, 0x41,
	0x87, 0x52, 0x09, 0x46, 0x27, 0x3c, 0x8e, 0x88, 0x2e, 0x5c, 0x67, 0x37, 0xf2, 0xc5, 0x71, 0x94,
	0x24, 0xd1, 0x98, 0xb5, 0x16, 0x95, 0xd6, 0xad, 0xa0, 0x69, 0xca, 0x84, 0x9c, 0x0d, 0xd9, 0x19,
	0xbc, 0x1c, 0x2c, 0xd8, 0x71, 0x77, 0xe8, 0x52, 0x45, 0x47, 0x39, 0x35, 0xd6, 0xcc, 0xe8, 0x12,
	0x0e, 0x34, 0x2e, 0x48, 0xc6, 0xbe, 0x27, 0xad, 0xd2, 0xeb, 0xd2, 0xbb, 0x83, 0xf6, 0xb9, 0x53,
	0xa0, 0xe4, 0x6c, 0xa1, 0x21, 0xbe, 0x27, 0xf1, 0x2a, 0x91, 0xfd, 0xb7, 0x0c, 0x27, 0x0f, 0x0c,
	0xa0, 0x53, 0x40, 0x3c, 0x24, 0x9f, 0xd8, 0xe7, 0x1e, 0x19, 0x79, 0xe4, 0x6b, 0xbb, 0xfb, 0x85,
	0xf8, 0xae, 0x5e, 0xe1, 0x19, 0xae, 0xf2, 0x30, 0x6f, 0x8c, 0xbc, 0xbc, 0xec, 0xbb, 0x73, 0xec,
	0x20, 0x32, 0xb1, 0xe5, 0x05, 0x36, 0x6f, 0x2c, 0xb1, 0xb7, 0x70, 0x9c, 0x63, 0x57, 0xd4, 0x57,
	0x94, 0xfb, 0x5c, 0x2a, 0x6b, 0x4f, 0x3f, 0xf3, 0xc3, 0x83, 0xcf, 0x5c, 0x9b, 0xc3, 0xaf, 0x78,
	0xb8, 0xa5, 0x8d, 0xae, 0xa0, 0xb1, 0xf9, 0x20, 0xe2, 0xfd, 0x50, 0x2c, 0x96, 0x3c, 0x89, 0xad,
	0x7d, 0x2d, 0xdb, 0x70, 0x66, 0x96, 0x39, 0x0b, 0x4d, 0x67, 0xe4, 0xc7, 0xaa, 0xd3, 0xbe, 0xa4,
	0xe3, 0x8c, 0xe1, 0xe7, 0xe6, 0xc3, 0x97, 0xc3, 0x36, 0x37, 0x8d, 0x5c, 0xd7, 0xfe, 0x08, 0xff,
	0x73, 0xc5, 0x26, 0xb9, 0x85, 0x7b, 0x3b, 0x59, 0x68, 0xb8, 0xa1, 0xd8, 0x24, 0xb7, 0x70, 0x46,
	0x61, 0xff, 0x2a, 0x99, 0xe6, 0x15, 0x40, 0xd1, 0x14, 0x4e, 0xee, 0x3f, 0xb2, 0x46, 0xcd, 0xc3,
	0x74, 0xf6, 0xe8, 0x4d, 0xb6, 0x9c, 0x39, 0x6f, 0xdb, 0x7f, 0xca, 0xd0, 0xd8, 0x36, 0x8f, 0x2c,
	0x78, 0xc2, 0x08, 0xee, 0xf6, 0xee, 0xe2, 0x54, 0x61, 0xb8, 0xdb, 0xf3, 0x5d, 0x14, 0x43, 0x4d,
	0xae, 0x4c, 0x0e, 0xa7, 0x29, 0xd3, 0x21, 0xaa, 0xb6, 0x7b, 0x8f, 0x5e, 0xd3, 0x68, 0xe6, 0x4c,
	0x78, 0x83, 0x1b, 0x7d, 0x87, 0xba, 0xde, 0xa4, 0x38, 0x80, 0xef, 0x0b, 0x35, 0xbd, 0x82, 0x01,
	0x7c, 0xc8, 0x36, 0x8b, 0xf6, 0x39, 0xd4, 0xd6, 0x77, 0x40, 0x16, 0xd4, 0x17, 0x54, 0x24, 0x8b,
	0x65, 0xca, 0x02, 0x7e, 0xc3, 0x59, 0x58, 0xfb, 0x0f, 0x55, 0xa0, 0x1c, 0x8b, 0x5a, 0xc9, 0xbe,
	0x82, 0x7a, 0x91, 0x04, 0xba, 0x30, 0x13, 0xd4, 0xdc, 0x69, 0xb9, 0xf5, 0xe8, 0xfc, 0x04, 0xeb,
	0x3e, 0x08, 0x0a, 0xc0, 0xe2, 0x21, 0x29, 0x6c, 0xcf, 0xb3, 0x72, 0xba, 0xbb, 0x26, 0x3e, 0xe2,
	0xa1, 0x67, 0xd6, 0x75, 0x38, 0x7e, 0x97, 0xe0, 0xa8, 0x70, 0x00, 0xbd, 0x85, 0xaa, 0x54, 0x54,
	0xa8, 0x21, 0x9f, 0xb0, 0x81, 0xa2, 0x93, 0x54, 0x8b, 0xee, 0xe3, 0xb5, 0x2a, 0xb2, 0xe1, 0x29,
	0x8b, 0xc3, 0x3b, 0x54, 0x59, 0xa3, 0x8c, 0x5a, 0x8e, 0xd1, 0x1f, 0xe8, 0x8b, 0x24, 0x8b, 0xd5,
	0xa8, 0xaf, 0xfd, 0xdc, 0xc7, 0x46, 0xcd, 0xc4, 0xb8, 0x7d, 0xfd, 0xef, 0x37, 0x30, 0x6e, 0xbf,
	0xd7, 0xf9, 0x76, 0x16, 0x31, 0x21, 0xb8, 0x72, 0x92, 0xa6, 0xa0, 0x71, 0x53, 0x06, 0x4e, 0x22,
	0xa2, 0x96, 0x68, 0x09, 0x1e, 0x34, 0xd3, 0xb1, 0x6a, 0x2d, 0xef, 0xd1, 0x5c, 0xde, 0xe3, 0xba,
	0xa2, 0x7f, 0x76, 0xfe, 0x05, 0x00, 0x00, 0xff, 0xff, 0x68, 0x44, 0x27, 0x17, 0x37, 0x06, 0x00,
	0x00,
}
