#ifndef __PROTOBUF_2_RAPIDJSON_H__
#define __PROTOBUF_2_RAPIDJSON_H__

// Protobuf 2 Rapidjson - Conversion of protobuf and rapidjson
// Copyright (c) 2013, Maurizio Monge - maurizio.monge@gmail.com

// Author: maurizio.monge@gmail.com (Maurizio Monge)
//  Inspired from pb2json, but using rapidjson and adding support for decoding

#include <rapidjson/document.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

namespace Protobuf2Rapidjson {

namespace priv {

using namespace rapidjson;
using namespace google::protobuf;

template <typename Encoding, typename Allocator>
bool decode_from_json_value(const GenericValue<Encoding, Allocator>& value,
                            Message *msg);

template <typename Encoding, typename Allocator>
void encode_to_json_value(const Message& msg,
                          GenericValue<Encoding, Allocator>* value,
                          Document::AllocatorType* alloc);

template <typename Encoding, typename Allocator>
void encode_to_json_doc(const Message& msg,
                          GenericDocument<Encoding, Allocator>* value);

template <typename Encoding, typename Allocator>
bool decode_repeated_field(const GenericValue<Encoding, Allocator>& array_value,
                           Message *msg,
                           const Reflection *ref,
                           const FieldDescriptor *field) {
	if(!array_value.IsArray())
		return false;

	switch (field->cpp_type())
	{
	case FieldDescriptor::CPPTYPE_DOUBLE:
		for(Value::ConstValueIterator it = array_value.Begin(); it != array_value.End(); ++it) {
			const Value& value = *it;
			if(!value.IsNumber()) return false;
			ref->AddDouble(msg,field,value.GetDouble());
		}
		break;
	case FieldDescriptor::CPPTYPE_FLOAT:
		for(Value::ConstValueIterator it = array_value.Begin(); it != array_value.End(); ++it) {
			const Value& value = *it;
			if(!value.IsNumber()) return false;
			ref->AddFloat(msg,field,value.GetDouble());
		}
		break;
	case FieldDescriptor::CPPTYPE_INT64:
		for(Value::ConstValueIterator it = array_value.Begin(); it != array_value.End(); ++it) {
			const Value& value = *it;
			if(!value.IsInt64()) return false;
			ref->AddInt64(msg,field,value.GetInt64());
		}
		break;
	case FieldDescriptor::CPPTYPE_UINT64:
		for(Value::ConstValueIterator it = array_value.Begin(); it != array_value.End(); ++it) {
			const Value& value = *it;
			if(!value.IsUint64()) return false;
			ref->AddUInt64(msg,field,value.GetUint64());
		}
		break;
	case FieldDescriptor::CPPTYPE_INT32:
		for(Value::ConstValueIterator it = array_value.Begin(); it != array_value.End(); ++it) {
			const Value& value = *it;
			if(!value.IsInt()) return false;
			ref->AddInt32(msg,field,value.GetInt());
		}
		break;
	case FieldDescriptor::CPPTYPE_UINT32:
		for(Value::ConstValueIterator it = array_value.Begin(); it != array_value.End(); ++it) {
			const Value& value = *it;
			if(!value.IsUint()) return false;
			ref->AddUInt32(msg,field,value.GetUint());
		}
		break;
	case FieldDescriptor::CPPTYPE_BOOL:
		for(Value::ConstValueIterator it = array_value.Begin(); it != array_value.End(); ++it) {
			const Value& value = *it;
			if(!value.IsBool()) return false;
			ref->AddBool(msg,field,value.GetBool());
		}
		break;
	case FieldDescriptor::CPPTYPE_STRING: 
		for(Value::ConstValueIterator it = array_value.Begin(); it != array_value.End(); ++it) {
			const Value& value = *it;
			if(!value.IsString()) return false;
			ref->AddString(msg,field,value.GetString());
		}
		break;
	case FieldDescriptor::CPPTYPE_MESSAGE: 
		for(Value::ConstValueIterator it = array_value.Begin(); it != array_value.End(); ++it) {
			const Value& value = *it;
			if(!decode_from_json_value(value, ref->AddMessage(msg,field)))
				return false;
		}
		break;
	case FieldDescriptor::CPPTYPE_ENUM: 
		for(Value::ConstValueIterator it = array_value.Begin(); it != array_value.End(); ++it) {
			const Value& value = *it;
			if(!value.IsInt()) return false;
			const EnumDescriptor* edesc = field->enum_type();
			const EnumValueDescriptor *en = edesc->FindValueByNumber(value.GetInt());
			if(!en)	return false;
			ref->AddEnum(msg,field,en);
		}
		break;
	default:
		break;
	}
	return true;
}

template <typename Encoding, typename Allocator>
bool decode_field(const GenericValue<Encoding, Allocator>& value,
                  Message *msg,
                  const Reflection *ref,
                  const FieldDescriptor *field) {
	switch (field->cpp_type())
	{
	case FieldDescriptor::CPPTYPE_DOUBLE:
		if(!value.IsNumber()) return false;
		ref->SetDouble(msg,field,value.GetDouble());
		break;
	case FieldDescriptor::CPPTYPE_FLOAT:
		if(!value.IsNumber()) return false;
		ref->SetFloat(msg,field,value.GetDouble());
		break;
	case FieldDescriptor::CPPTYPE_INT64:
		if(!value.IsInt64()) return false;
		ref->SetInt64(msg,field,value.GetInt64());
		break;
	case FieldDescriptor::CPPTYPE_UINT64:
		if(!value.IsUint64()) return false;
		ref->SetUInt64(msg,field,value.GetUint64());
		break;
	case FieldDescriptor::CPPTYPE_INT32:
		if(!value.IsInt()) return false;
		ref->SetInt32(msg,field,value.GetInt());
		break;
	case FieldDescriptor::CPPTYPE_UINT32:
		if(!value.IsUint()) return false;
		ref->SetUInt32(msg,field,value.GetUint());
		break;
	case FieldDescriptor::CPPTYPE_BOOL:
		if(!value.IsBool()) return false;
		ref->SetBool(msg,field,value.GetBool());
		break;
	case FieldDescriptor::CPPTYPE_STRING: {
		if(!value.IsString()) return false;
		ref->SetString(msg,field,value.GetString());
		break;
	}
	case FieldDescriptor::CPPTYPE_MESSAGE: {
		if(!decode_from_json_value(value, ref->MutableMessage(msg,field)))
			return false;
		break;
	}
	case FieldDescriptor::CPPTYPE_ENUM: {
		if(!value.IsInt()) return false;
		const EnumDescriptor* edesc = field->enum_type();
		const EnumValueDescriptor *en = edesc->FindValueByNumber(value.GetInt());
		if(!en)	return false;
		ref->SetEnum(msg,field,en);
		break;
	}
	default:
		break;
	}
	return true;
}

template <typename Encoding, typename Allocator>
void encode_repeated_field(const Message *msg,
                           const Reflection *ref,
                           const FieldDescriptor *field,
                           GenericValue<Encoding, Allocator>* value,
                           Document::AllocatorType* alloc) {

	value->SetArray();

	Value tmp;
	size_t count = ref->FieldSize(*msg,field);

	switch (field->cpp_type())
	{
	case FieldDescriptor::CPPTYPE_DOUBLE:
		for(size_t i = 0 ; i != count ; ++i) {
			tmp.SetDouble(ref->GetRepeatedDouble(*msg,field,i));
			value->PushBack(tmp, *alloc);
		}
		break;
	case FieldDescriptor::CPPTYPE_FLOAT:
		for(size_t i = 0 ; i != count ; ++i) {
			tmp.SetDouble(ref->GetRepeatedFloat(*msg,field,i));
			value->PushBack(tmp, *alloc);
		}
		break;
	case FieldDescriptor::CPPTYPE_INT64:
		for(size_t i = 0 ; i != count ; ++i) {
			tmp.SetInt64(ref->GetRepeatedInt64(*msg,field,i));
			value->PushBack(tmp, *alloc);
		}
		break;
	case FieldDescriptor::CPPTYPE_UINT64:
		for(size_t i = 0 ; i != count ; ++i) {
			tmp.SetUint64(ref->GetRepeatedUInt64(*msg,field,i));
			value->PushBack(tmp, *alloc);
		}
		break;
	case FieldDescriptor::CPPTYPE_INT32:
		for(size_t i = 0 ; i != count ; ++i) {
			tmp.SetInt(ref->GetRepeatedInt32(*msg,field,i));
			value->PushBack(tmp, *alloc);
		}	
		break;
	case FieldDescriptor::CPPTYPE_UINT32:
		for(size_t i = 0 ; i != count ; ++i) {
			tmp.SetUint(ref->GetRepeatedUInt32(*msg,field,i));
			value->PushBack(tmp, *alloc);
		}
		break;
	case FieldDescriptor::CPPTYPE_BOOL:
		for(size_t i = 0 ; i != count ; ++i) {
			tmp.SetBool(ref->GetRepeatedBool(*msg,field,i));
			value->PushBack(tmp, *alloc);
		}
		break;
	case FieldDescriptor::CPPTYPE_STRING: {
		for(size_t i = 0 ; i != count ; ++i) {
			std::string tmps;
			const std::string& str = ref->GetRepeatedStringReference(*msg,field,i,&tmps);
			tmp.SetString(str.c_str(), *alloc);
			value->PushBack(tmp, *alloc);
		}
		break;
	}
	case FieldDescriptor::CPPTYPE_MESSAGE:
		for(size_t i = 0 ; i != count ; ++i) {
			encode_to_json_value(ref->GetRepeatedMessage(*msg,field,i), &tmp, alloc);
			value->PushBack(tmp, *alloc);
		}
		break;
	case FieldDescriptor::CPPTYPE_ENUM: {
		for(size_t i = 0 ; i != count ; ++i) {
			const EnumValueDescriptor *en = ref->GetRepeatedEnum(*msg,field,i);
			tmp.SetInt(en->number());
			value->PushBack(tmp, *alloc);
		}
		break;
	}
	default:
		break;
	}
}

template <typename Encoding, typename Allocator>
void encode_field(const Message *msg,
                  const Reflection *ref,
                  const FieldDescriptor *field,
                  GenericValue<Encoding, Allocator>* value,
                  Document::AllocatorType* alloc) {
	switch (field->cpp_type())
	{
	case FieldDescriptor::CPPTYPE_DOUBLE:
		value->SetDouble(ref->GetDouble(*msg,field));
		break;
	case FieldDescriptor::CPPTYPE_FLOAT:
		value->SetDouble(ref->GetFloat(*msg,field));
		break;
	case FieldDescriptor::CPPTYPE_INT64:
		value->SetInt64(ref->GetInt64(*msg,field));
		break;
	case FieldDescriptor::CPPTYPE_UINT64:
		value->SetUint64(ref->GetUInt64(*msg,field));
		break;
	case FieldDescriptor::CPPTYPE_INT32:
		value->SetInt(ref->GetInt32(*msg,field));	
		break;
	case FieldDescriptor::CPPTYPE_UINT32:
		value->SetUint(ref->GetUInt32(*msg,field));
		break;
	case FieldDescriptor::CPPTYPE_BOOL:
		value->SetBool(ref->GetBool(*msg,field));
		break;
	case FieldDescriptor::CPPTYPE_STRING: {
		std::string tmp;
		const std::string& str = ref->GetStringReference(*msg,field,&tmp);
		value->SetString(str.c_str(), *alloc);
		break;
	}
	case FieldDescriptor::CPPTYPE_MESSAGE:
		encode_to_json_value(ref->GetMessage(*msg,field), value, alloc);
		break;
	case FieldDescriptor::CPPTYPE_ENUM: {
		const EnumValueDescriptor *en = ref->GetEnum(*msg,field);
		value->SetInt(en->number());
		break;
	}
	default:
		break;
	}
}

template <typename Encoding, typename Allocator>
bool decode_from_json_value(const GenericValue<Encoding, Allocator>& value,
                            Message *msg) {

	const Descriptor *d = msg->GetDescriptor();
	if(!d) return false;

	const Reflection *ref = msg->GetReflection();
	if(!ref) return false;

	if(!value.IsObject())
		return false;

	for(Value::ConstMemberIterator it = value.MemberBegin(); it != value.MemberEnd(); ++it) {
		const FieldDescriptor *field = d->FindFieldByName(it->name.GetString());
		if(field == NULL) return false;

		if(field->is_repeated()) {
			if(!decode_repeated_field(it->value, msg, ref, field))
				return false;
		}
		else {
			if(!decode_field(it->value, msg, ref, field))
				return false;
		}
	}

	return true;
}

template <typename Encoding, typename Allocator>
void encode_to_json_value(const Message& msg,
                          GenericValue<Encoding, Allocator>* value,
                          Document::AllocatorType* alloc) {

	const Descriptor *d = msg.GetDescriptor();
	const Reflection *ref = msg.GetReflection();

	value->SetObject();

	size_t count = d->field_count();
	for (size_t i = 0; i != count ; ++i) {
		const FieldDescriptor *field = d->field(i);

		Value member_value;

		if(field->is_repeated()) {
			encode_repeated_field(&msg, ref, field, &member_value, alloc);
			value->AddMember(field->name().c_str(), *alloc, member_value, *alloc);
		}
		else if(field->is_required() || ref->HasField(msg,field)) {
			encode_field(&msg, ref, field, &member_value, alloc);
			value->AddMember(field->name().c_str(), *alloc, member_value, *alloc);
		}
	}
}

template <typename Encoding, typename Allocator>
void encode_to_json_doc(const Message& msg,
                        GenericDocument<Encoding, Allocator>* doc) {
	encode_to_json_value(msg, doc, &doc->GetAllocator());
}

} //end namespace priv

using priv::decode_from_json_value;
using priv::encode_to_json_value;
using priv::encode_to_json_doc;

} //end namespace Protobuf2Rapidjson

#endif //__PROTOBUF_2_RAPIDJSON_H__

// Local Variables:
// mode: c++
// End:
