// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: capabilities.proto

#include "capabilities.pb.h"

#include <algorithm>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>

PROTOBUF_PRAGMA_INIT_SEG

namespace _pb = ::PROTOBUF_NAMESPACE_ID;
namespace _pbi = _pb::internal;

namespace otbr {
PROTOBUF_CONSTEXPR Capabilities::Capabilities(
    ::_pbi::ConstantInitialized): _impl_{
    /*decltype(_impl_._has_bits_)*/{}
  , /*decltype(_impl_._cached_size_)*/{}
  , /*decltype(_impl_.nat64_)*/false
  , /*decltype(_impl_.dhcp6_pd_)*/false} {}
struct CapabilitiesDefaultTypeInternal {
  PROTOBUF_CONSTEXPR CapabilitiesDefaultTypeInternal()
      : _instance(::_pbi::ConstantInitialized{}) {}
  ~CapabilitiesDefaultTypeInternal() {}
  union {
    Capabilities _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 CapabilitiesDefaultTypeInternal _Capabilities_default_instance_;
}  // namespace otbr
namespace otbr {

// ===================================================================

class Capabilities::_Internal {
 public:
  using HasBits = decltype(std::declval<Capabilities>()._impl_._has_bits_);
  static void set_has_nat64(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
  static void set_has_dhcp6_pd(HasBits* has_bits) {
    (*has_bits)[0] |= 2u;
  }
};

Capabilities::Capabilities(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                         bool is_message_owned)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(arena, is_message_owned) {
  SharedCtor(arena, is_message_owned);
  // @@protoc_insertion_point(arena_constructor:otbr.Capabilities)
}
Capabilities::Capabilities(const Capabilities& from)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite() {
  Capabilities* const _this = this; (void)_this;
  new (&_impl_) Impl_{
      decltype(_impl_._has_bits_){from._impl_._has_bits_}
    , /*decltype(_impl_._cached_size_)*/{}
    , decltype(_impl_.nat64_){}
    , decltype(_impl_.dhcp6_pd_){}};

  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::memcpy(&_impl_.nat64_, &from._impl_.nat64_,
    static_cast<size_t>(reinterpret_cast<char*>(&_impl_.dhcp6_pd_) -
    reinterpret_cast<char*>(&_impl_.nat64_)) + sizeof(_impl_.dhcp6_pd_));
  // @@protoc_insertion_point(copy_constructor:otbr.Capabilities)
}

inline void Capabilities::SharedCtor(
    ::_pb::Arena* arena, bool is_message_owned) {
  (void)arena;
  (void)is_message_owned;
  new (&_impl_) Impl_{
      decltype(_impl_._has_bits_){}
    , /*decltype(_impl_._cached_size_)*/{}
    , decltype(_impl_.nat64_){false}
    , decltype(_impl_.dhcp6_pd_){false}
  };
}

Capabilities::~Capabilities() {
  // @@protoc_insertion_point(destructor:otbr.Capabilities)
  if (auto *arena = _internal_metadata_.DeleteReturnArena<std::string>()) {
  (void)arena;
    return;
  }
  SharedDtor();
}

inline void Capabilities::SharedDtor() {
  GOOGLE_DCHECK(GetArenaForAllocation() == nullptr);
}

void Capabilities::SetCachedSize(int size) const {
  _impl_._cached_size_.Set(size);
}

void Capabilities::Clear() {
// @@protoc_insertion_point(message_clear_start:otbr.Capabilities)
  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  ::memset(&_impl_.nat64_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&_impl_.dhcp6_pd_) -
      reinterpret_cast<char*>(&_impl_.nat64_)) + sizeof(_impl_.dhcp6_pd_));
  _impl_._has_bits_.Clear();
  _internal_metadata_.Clear<std::string>();
}

const char* Capabilities::_InternalParse(const char* ptr, ::_pbi::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    uint32_t tag;
    ptr = ::_pbi::ReadTag(ptr, &tag);
    switch (tag >> 3) {
      // optional bool nat64 = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 8)) {
          _Internal::set_has_nat64(&has_bits);
          _impl_.nat64_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // optional bool dhcp6_pd = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 16)) {
          _Internal::set_has_dhcp6_pd(&has_bits);
          _impl_.dhcp6_pd_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      default:
        goto handle_unusual;
    }  // switch
  handle_unusual:
    if ((tag == 0) || ((tag & 7) == 4)) {
      CHK_(ptr);
      ctx->SetLastTag(tag);
      goto message_done;
    }
    ptr = UnknownFieldParse(
        tag,
        _internal_metadata_.mutable_unknown_fields<std::string>(),
        ptr, ctx);
    CHK_(ptr != nullptr);
  }  // while
message_done:
  _impl_._has_bits_.Or(has_bits);
  return ptr;
failure:
  ptr = nullptr;
  goto message_done;
#undef CHK_
}

uint8_t* Capabilities::_InternalSerialize(
    uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:otbr.Capabilities)
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _impl_._has_bits_[0];
  // optional bool nat64 = 1;
  if (cached_has_bits & 0x00000001u) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteBoolToArray(1, this->_internal_nat64(), target);
  }

  // optional bool dhcp6_pd = 2;
  if (cached_has_bits & 0x00000002u) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteBoolToArray(2, this->_internal_dhcp6_pd(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = stream->WriteRaw(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).data(),
        static_cast<int>(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size()), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:otbr.Capabilities)
  return target;
}

size_t Capabilities::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:otbr.Capabilities)
  size_t total_size = 0;

  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  cached_has_bits = _impl_._has_bits_[0];
  if (cached_has_bits & 0x00000003u) {
    // optional bool nat64 = 1;
    if (cached_has_bits & 0x00000001u) {
      total_size += 1 + 1;
    }

    // optional bool dhcp6_pd = 2;
    if (cached_has_bits & 0x00000002u) {
      total_size += 1 + 1;
    }

  }
  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    total_size += _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size();
  }
  int cached_size = ::_pbi::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void Capabilities::CheckTypeAndMergeFrom(
    const ::PROTOBUF_NAMESPACE_ID::MessageLite& from) {
  MergeFrom(*::_pbi::DownCast<const Capabilities*>(
      &from));
}

void Capabilities::MergeFrom(const Capabilities& from) {
  Capabilities* const _this = this;
  // @@protoc_insertion_point(class_specific_merge_from_start:otbr.Capabilities)
  GOOGLE_DCHECK_NE(&from, _this);
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = from._impl_._has_bits_[0];
  if (cached_has_bits & 0x00000003u) {
    if (cached_has_bits & 0x00000001u) {
      _this->_impl_.nat64_ = from._impl_.nat64_;
    }
    if (cached_has_bits & 0x00000002u) {
      _this->_impl_.dhcp6_pd_ = from._impl_.dhcp6_pd_;
    }
    _this->_impl_._has_bits_[0] |= cached_has_bits;
  }
  _this->_internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
}

void Capabilities::CopyFrom(const Capabilities& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:otbr.Capabilities)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Capabilities::IsInitialized() const {
  return true;
}

void Capabilities::InternalSwap(Capabilities* other) {
  using std::swap;
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  swap(_impl_._has_bits_[0], other->_impl_._has_bits_[0]);
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(Capabilities, _impl_.dhcp6_pd_)
      + sizeof(Capabilities::_impl_.dhcp6_pd_)
      - PROTOBUF_FIELD_OFFSET(Capabilities, _impl_.nat64_)>(
          reinterpret_cast<char*>(&_impl_.nat64_),
          reinterpret_cast<char*>(&other->_impl_.nat64_));
}

std::string Capabilities::GetTypeName() const {
  return "otbr.Capabilities";
}


// @@protoc_insertion_point(namespace_scope)
}  // namespace otbr
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::otbr::Capabilities*
Arena::CreateMaybeMessage< ::otbr::Capabilities >(Arena* arena) {
  return Arena::CreateMessageInternal< ::otbr::Capabilities >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
