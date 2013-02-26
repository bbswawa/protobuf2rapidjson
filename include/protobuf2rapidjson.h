#ifndef __PROTOBUF_2_RAPIDJSON_H__
#define __PROTOBUF_2_RAPIDJSON_H__

// Protobuf 2 Rapidjson - Conversion of protobuf and rapidjson
// Copyright (c) 2013, Maurizio Monge - maurizio.monge@gmail.com

// Author: maurizio.monge@gmail.com (Maurizio Monge)
//  Inspired from pb2json, but using rapidjson and adding support for decoding

#include <sstream>
#include <rapidjson/document.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

namespace Protobuf2Rapidjson {

namespace priv {

using namespace rapidjson;
using namespace google::protobuf;

template <typename Encoding, typename Allocator>
bool decode_from_json_value(const GenericValue<Encoding, Allocator>& value,
                            Message *msg,
                            std::string *error = NULL);

template <typename Encoding, typename Allocator>
void encode_to_json_value(const Message& msg,
                          GenericValue<Encoding, Allocator>* value,
                          Document::AllocatorType* alloc);

template <typename Encoding, typename Allocator>
void encode_to_json_doc(const Message& msg,
                          GenericDocument<Encoding, Allocator>* value);

std::string itos(int n) {
	std::ostringstream ss;
	ss << n;
	return ss.str();
}

template <typename Encoding, typename Allocator>
std::string value_to_string(const GenericValue<Encoding, Allocator>& value) {
	std::ostringstream ss;
	if(value.IsInt())
		ss << value.GetInt();
	else if(value.IsString())
		ss << "\"" << value.GetString() << "\"";
	else
		ss << "???";
	return ss.str();
}

bool set_error(std::string* error, const FieldDescriptor *field, const std::string& msg) {
	if(error)
		*error = "/"+field->name()+" ("+msg+")";
	return false;
}

bool set_error_array(std::string* error, int i, const FieldDescriptor *field, const std::string& msg) {
	if(error)
		*error = "/"+field->name()+"["+itos(i)+"] ("+msg+")";
	return false;
}

template <typename Encoding, typename Allocator>
bool decode_repeated_field(const GenericValue<Encoding, Allocator>& array_value,
                           Message *msg,
                           const Reflection *ref,
                           const FieldDescriptor *field,
                           std::string *error) {
	if(!array_value.IsArray())
		return set_error(error,field,"Expected array");

	int i = 0;
	switch (field->cpp_type())
	{
	case FieldDescriptor::CPPTYPE_DOUBLE:
		for(Value::ConstValueIterator it = array_value.Begin(); it != array_value.End(); ++it, ++i) {
			const Value& value = *it;
			if(!value.IsNumber())
				return set_error_array(error,i,field,"Expected number");
			ref->AddDouble(msg,field,value.GetDouble());
		}
		break;
	case FieldDescriptor::CPPTYPE_FLOAT:
		for(Value::ConstValueIterator it = array_value.Begin(); it != array_value.End(); ++it, ++i) {
			const Value& value = *it;
			if(!value.IsNumber())
				return set_error_array(error,i,field,"Expected number");
			ref->AddFloat(msg,field,value.GetDouble());
		}
		break;
	case FieldDescriptor::CPPTYPE_INT64:
		for(Value::ConstValueIterator it = array_value.Begin(); it != array_value.End(); ++it, ++i) {
			const Value& value = *it;
			if(!value.IsInt64())
				return set_error_array(error,i,field,"Expected int64");
			ref->AddInt64(msg,field,value.GetInt64());
		}
		break;
	case FieldDescriptor::CPPTYPE_UINT64:
		for(Value::ConstValueIterator it = array_value.Begin(); it != array_value.End(); ++it, ++i) {
			const Value& value = *it;
			if(!value.IsUint64())
				return set_error_array(error,i,field,"Expected uint64");
			ref->AddUInt64(msg,field,value.GetUint64());
		}
		break;
	case FieldDescriptor::CPPTYPE_INT32:
		for(Value::ConstValueIterator it = array_value.Begin(); it != array_value.End(); ++it, ++i) {
			const Value& value = *it;
			if(!value.IsInt())
				return set_error_array(error,i,field,"Expected int");
			ref->AddInt32(msg,field,value.GetInt());
		}
		break;
	case FieldDescriptor::CPPTYPE_UINT32:
		for(Value::ConstValueIterator it = array_value.Begin(); it != array_value.End(); ++it, ++i) {
			const Value& value = *it;
			if(!value.IsUint())
				return set_error_array(error,i,field,"Expected uint");
			ref->AddUInt32(msg,field,value.GetUint());
		}
		break;
	case FieldDescriptor::CPPTYPE_BOOL:
		for(Value::ConstValueIterator it = array_value.Begin(); it != array_value.End(); ++it, ++i) {
			const Value& value = *it;
			if(!value.IsBool())
				return set_error_array(error,i,field,"Expected bool");
			ref->AddBool(msg,field,value.GetBool());
		}
		break;
	case FieldDescriptor::CPPTYPE_STRING: 
		for(Value::ConstValueIterator it = array_value.Begin(); it != array_value.End(); ++it, ++i) {
			const Value& value = *it;
			if(!value.IsString())
				return set_error_array(error,i,field,"Expected string");
			ref->AddString(msg,field,value.GetString());
		}
		break;
	case FieldDescriptor::CPPTYPE_MESSAGE: 
		for(Value::ConstValueIterator it = array_value.Begin(); it != array_value.End(); ++it, ++i) {
			const Value& value = *it;
			if(!decode_from_json_value(value, ref->AddMessage(msg,field), error)) {
				if(error)
					*error = "/"+field->name()+"["+itos(i)+"]"+*error;
				return false;
			}
		}
		break;
	case FieldDescriptor::CPPTYPE_ENUM: 
		for(Value::ConstValueIterator it = array_value.Begin(); it != array_value.End(); ++it, ++i) {
			const Value& value = *it;
			if(!value.IsInt() && !value.IsString())
				return set_error_array(error,i,field,"Expected string or int for enum");
			const EnumDescriptor* edesc = field->enum_type();
			const EnumValueDescriptor *en = value.IsInt() ?
				edesc->FindValueByNumber(value.GetInt()) :
				edesc->FindValueByName(value.GetString());
			if(!en)
				return set_error_array(error,i,field,
				                       "Invalid value "+value_to_string(value)+" for enum "+edesc->name());
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
                  const FieldDescriptor *field,
                  std::string *error) {
	switch (field->cpp_type())
	{
	case FieldDescriptor::CPPTYPE_DOUBLE:
		if(!value.IsNumber())
			return set_error(error,field,"Expected number");
		ref->SetDouble(msg,field,value.GetDouble());
		break;
	case FieldDescriptor::CPPTYPE_FLOAT:
		if(!value.IsNumber())
			return set_error(error,field,"Expected number");
		ref->SetFloat(msg,field,value.GetDouble());
		break;
	case FieldDescriptor::CPPTYPE_INT64:
		if(!value.IsInt64())
			return set_error(error,field,"Expected int64");
		ref->SetInt64(msg,field,value.GetInt64());
		break;
	case FieldDescriptor::CPPTYPE_UINT64:
		if(!value.IsUint64())
			return set_error(error,field,"Expected uint64");
		ref->SetUInt64(msg,field,value.GetUint64());
		break;
	case FieldDescriptor::CPPTYPE_INT32:
		if(!value.IsInt())
			return set_error(error,field,"Expected int");			
		ref->SetInt32(msg,field,value.GetInt());
		break;
	case FieldDescriptor::CPPTYPE_UINT32:
		if(!value.IsUint())
			return set_error(error,field,"Expected uint");
		ref->SetUInt32(msg,field,value.GetUint());
		break;
	case FieldDescriptor::CPPTYPE_BOOL:
		if(!value.IsBool())
			return set_error(error,field,"Expected bool");
		ref->SetBool(msg,field,value.GetBool());
		break;
	case FieldDescriptor::CPPTYPE_STRING: {
		if(!value.IsString()) 
			return set_error(error,field,"Expected string");
		ref->SetString(msg,field,value.GetString());
		break;
	}
	case FieldDescriptor::CPPTYPE_MESSAGE: {
		if(!decode_from_json_value(value, ref->MutableMessage(msg,field), error)) {
			if(error)
				*error = "/"+field->name()+*error;
			return false;
		}
		break;
	}
	case FieldDescriptor::CPPTYPE_ENUM: {
		if(!value.IsInt() && !value.IsString())
			return set_error(error,field,"Expected string or int for enum");
		const EnumDescriptor* edesc = field->enum_type();
		const EnumValueDescriptor *en = value.IsInt() ?
			edesc->FindValueByNumber(value.GetInt()) :
			edesc->FindValueByName(value.GetString());
		if(!en)
			return set_error(error,field,"Invalid value "+value_to_string(value)+" for enum "+edesc->name());
		ref->SetEnum(msg,field,en);
		break;
	}
	default:
		break;
	}
	return true;
}

template <typename Encoding, typename Allocator>
bool decode_from_json_value(const GenericValue<Encoding, Allocator>& value,
                            Message *msg,
                            std::string *error) {

	const Descriptor *d = msg->GetDescriptor();
	const Reflection *ref = msg->GetReflection();

	if(!value.IsObject()) {
		if(error)
			*error = " (Expected object)";
		return false;
	}

	for(Value::ConstMemberIterator it = value.MemberBegin(); it != value.MemberEnd(); ++it) {
		const FieldDescriptor *field = d->FindFieldByName(it->name.GetString());
		if(field == NULL) {
			if(error)
				*error = std::string(" (No field \"")+it->name.GetString()+"\" in message "+d->name()+")";
			return false;
		}

		if(field->is_repeated()) {
			if(!decode_repeated_field(it->value, msg, ref, field, error))
				return false;
		}
		else {
			if(!decode_field(it->value, msg, ref, field, error))
				return false;
		}
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
