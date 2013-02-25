// Protobuf 2 Rapidjson - Conversion of protobuf and rapidjson
// Copyright (c) 2013, Maurizio Monge - maurizio.monge@gmail.com

// Author: maurizio.monge@gmail.com (Maurizio Monge)
//  Inspired from pb2json, but using rapidjson and adding support for decoding

#include <rapidjson/document.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

namespace Protobuf2Rapidjson {

bool decode_from_json_value(const rapidjson::Value& value,
                            google::protobuf::Message *msg);

bool priv_decode_repeated_field(const rapidjson::Value& array_value,
                                google::protobuf::Message *msg,
                                const google::protobuf::Reflection *ref,
                                const google::protobuf::FieldDescriptor *field) {
	if(!array_value.IsArray())
		return false;

	switch (field->cpp_type())
	{
	case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
		for(rapidjson::Value::ConstValueIterator it = array_value.Begin(); it != array_value.End(); ++it) {
			const rapidjson::Value& value = *it;
			if(!value.IsNumber()) return false;
			ref->AddDouble(msg,field,value.GetDouble());
		}
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
		for(rapidjson::Value::ConstValueIterator it = array_value.Begin(); it != array_value.End(); ++it) {
			const rapidjson::Value& value = *it;
			if(!value.IsNumber()) return false;
			ref->AddFloat(msg,field,value.GetDouble());
		}
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
		for(rapidjson::Value::ConstValueIterator it = array_value.Begin(); it != array_value.End(); ++it) {
			const rapidjson::Value& value = *it;
			if(!value.IsInt64()) return false;
			ref->AddInt64(msg,field,value.GetInt64());
		}
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
		for(rapidjson::Value::ConstValueIterator it = array_value.Begin(); it != array_value.End(); ++it) {
			const rapidjson::Value& value = *it;
			if(!value.IsUint64()) return false;
			ref->AddUInt64(msg,field,value.GetUint64());
		}
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
		for(rapidjson::Value::ConstValueIterator it = array_value.Begin(); it != array_value.End(); ++it) {
			const rapidjson::Value& value = *it;
			if(!value.IsInt()) return false;
			ref->AddInt32(msg,field,value.GetInt());
		}
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
		for(rapidjson::Value::ConstValueIterator it = array_value.Begin(); it != array_value.End(); ++it) {
			const rapidjson::Value& value = *it;
			if(!value.IsUint()) return false;
			ref->AddUInt32(msg,field,value.GetUint());
		}
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
		for(rapidjson::Value::ConstValueIterator it = array_value.Begin(); it != array_value.End(); ++it) {
			const rapidjson::Value& value = *it;
			if(!value.IsBool()) return false;
			ref->AddBool(msg,field,value.GetBool());
		}
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_STRING: 
		for(rapidjson::Value::ConstValueIterator it = array_value.Begin(); it != array_value.End(); ++it) {
			const rapidjson::Value& value = *it;
			if(!value.IsString()) return false;
			ref->AddString(msg,field,value.GetString());
		}
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE: 
		for(rapidjson::Value::ConstValueIterator it = array_value.Begin(); it != array_value.End(); ++it) {
			const rapidjson::Value& value = *it;
			if(!decode_from_json_value(value, ref->AddMessage(msg,field)))
				return false;
		}
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_ENUM: 
		for(rapidjson::Value::ConstValueIterator it = array_value.Begin(); it != array_value.End(); ++it) {
			const rapidjson::Value& value = *it;
			if(!value.IsInt()) return false;
			const google::protobuf::EnumDescriptor* edesc = field->enum_type();
			const google::protobuf::EnumValueDescriptor *en = edesc->FindValueByNumber(value.GetInt());
			if(!en)	return false;
			ref->AddEnum(msg,field,en);
		}
		break;
	default:
		break;
	}
	return true;
}

bool priv_decode_field(const rapidjson::Value& value,
                       google::protobuf::Message *msg,
                       const google::protobuf::Reflection *ref,
                       const google::protobuf::FieldDescriptor *field) {
	switch (field->cpp_type())
	{
	case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
		if(!value.IsNumber()) return false;
		ref->SetDouble(msg,field,value.GetDouble());
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
		if(!value.IsNumber()) return false;
		ref->SetFloat(msg,field,value.GetDouble());
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
		if(!value.IsInt64()) return false;
		ref->SetInt64(msg,field,value.GetInt64());
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
		if(!value.IsUint64()) return false;
		ref->SetUInt64(msg,field,value.GetUint64());
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
		if(!value.IsInt()) return false;
		ref->SetInt32(msg,field,value.GetInt());
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
		if(!value.IsUint()) return false;
		ref->SetUInt32(msg,field,value.GetUint());
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
		if(!value.IsBool()) return false;
		ref->SetBool(msg,field,value.GetBool());
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_STRING: {
		if(!value.IsString()) return false;
		ref->SetString(msg,field,value.GetString());
		break;
	}
	case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE: {
		if(!decode_from_json_value(value, ref->MutableMessage(msg,field)))
			return false;
		break;
	}
	case google::protobuf::FieldDescriptor::CPPTYPE_ENUM: {
		if(!value.IsInt()) return false;
		const google::protobuf::EnumDescriptor* edesc = field->enum_type();
		const google::protobuf::EnumValueDescriptor *en = edesc->FindValueByNumber(value.GetInt());
		if(!en)	return false;
		ref->SetEnum(msg,field,en);
		break;
	}
	default:
		break;
	}
	return true;
}

bool decode_from_json_value(const rapidjson::Value& value,
                            google::protobuf::Message *msg) {

	const google::protobuf::Descriptor *d = msg->GetDescriptor();
	if(!d) return false;

	const google::protobuf::Reflection *ref = msg->GetReflection();
	if(!ref) return false;

	if(!value.IsObject())
		return false;

	for(rapidjson::Value::ConstMemberIterator it = value.MemberBegin(); it != value.MemberEnd(); ++it) {
		const google::protobuf::FieldDescriptor *field = d->FindFieldByName(it->name.GetString());
		if(field == NULL) return false;

		if(field->is_repeated()) {
			if(!priv_decode_repeated_field(it->value, msg, ref, field))
				return false;
		}
		else {
			if(!priv_decode_field(it->value, msg, ref, field))
				return false;
		}
	}

	return true;
}

bool encode_to_json_value(const google::protobuf::Message& msg,
                          rapidjson::Value* value,
                          rapidjson::Document::AllocatorType* alloc);

void priv_encode_repeated_field(const google::protobuf::Message *msg,
                                const google::protobuf::Reflection *ref,
                                const google::protobuf::FieldDescriptor *field,
                                rapidjson::Value* value,
                                rapidjson::Document::AllocatorType* alloc) {

	value->SetArray();

	rapidjson::Value tmp;
	size_t count = ref->FieldSize(*msg,field);

	switch (field->cpp_type())
	{
	case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
		for(size_t i = 0 ; i != count ; ++i) {
			tmp.SetDouble(ref->GetRepeatedDouble(*msg,field,i));
			value->PushBack(tmp, *alloc);
		}
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
		for(size_t i = 0 ; i != count ; ++i) {
			tmp.SetDouble(ref->GetRepeatedFloat(*msg,field,i));
			value->PushBack(tmp, *alloc);
		}
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
		for(size_t i = 0 ; i != count ; ++i) {
			tmp.SetInt64(ref->GetRepeatedInt64(*msg,field,i));
			value->PushBack(tmp, *alloc);
		}
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
		for(size_t i = 0 ; i != count ; ++i) {
			tmp.SetUint64(ref->GetRepeatedUInt64(*msg,field,i));
			value->PushBack(tmp, *alloc);
		}
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
		for(size_t i = 0 ; i != count ; ++i) {
			tmp.SetInt(ref->GetRepeatedInt32(*msg,field,i));
			value->PushBack(tmp, *alloc);
		}	
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
		for(size_t i = 0 ; i != count ; ++i) {
			tmp.SetUint(ref->GetRepeatedUInt32(*msg,field,i));
			value->PushBack(tmp, *alloc);
		}
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
		for(size_t i = 0 ; i != count ; ++i) {
			tmp.SetBool(ref->GetRepeatedBool(*msg,field,i));
			value->PushBack(tmp, *alloc);
		}
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_STRING: {
		for(size_t i = 0 ; i != count ; ++i) {
			std::string tmps;
			const std::string& str = ref->GetRepeatedStringReference(*msg,field,i,&tmps);
			tmp.SetString(str.c_str(), *alloc);
			value->PushBack(tmp, *alloc);
		}
		break;
	}
	case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
		for(size_t i = 0 ; i != count ; ++i) {
			encode_to_json_value(ref->GetRepeatedMessage(*msg,field,i), &tmp, alloc);
			value->PushBack(tmp, *alloc);
		}
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_ENUM: {
		for(size_t i = 0 ; i != count ; ++i) {
			const google::protobuf::EnumValueDescriptor *en = ref->GetRepeatedEnum(*msg,field,i);
			tmp.SetInt(en->number());
			value->PushBack(tmp, *alloc);
		}
		break;
	}
	default:
		break;
	}
}

void priv_encode_field(const google::protobuf::Message *msg,
                       const google::protobuf::Reflection *ref,
                       const google::protobuf::FieldDescriptor *field,
                       rapidjson::Value* value,
                       rapidjson::Document::AllocatorType* alloc) {
	switch (field->cpp_type())
	{
	case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
		value->SetDouble(ref->GetDouble(*msg,field));
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
		value->SetDouble(ref->GetFloat(*msg,field));
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
		value->SetInt64(ref->GetInt64(*msg,field));
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
		value->SetUint64(ref->GetUInt64(*msg,field));
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
		value->SetInt(ref->GetInt32(*msg,field));	
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
		value->SetUint(ref->GetUInt32(*msg,field));
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
		value->SetBool(ref->GetBool(*msg,field));
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_STRING: {
		std::string tmp;
		const std::string& str = ref->GetStringReference(*msg,field,&tmp);
		value->SetString(str.c_str(), *alloc);
		break;
	}
	case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
		encode_to_json_value(ref->GetMessage(*msg,field), value, alloc);
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_ENUM: {
		const google::protobuf::EnumValueDescriptor *en = ref->GetEnum(*msg,field);
		value->SetInt(en->number());
		break;
	}
	default:
		break;
	}
}

bool encode_to_json_value(const google::protobuf::Message& msg,
                          rapidjson::Value* value,
                          rapidjson::Document::AllocatorType* alloc) {
	
	const google::protobuf::Descriptor *d = msg.GetDescriptor();
	if(!d)return false;

	const google::protobuf::Reflection *ref = msg.GetReflection();
	if(!ref)return false;

	value->SetObject();

	size_t count = d->field_count();
	for (size_t i = 0; i != count ; ++i) {
		const google::protobuf::FieldDescriptor *field = d->field(i);
		if(!field)return false;

		rapidjson::Value member_value;

		if(field->is_repeated()) {
			priv_encode_repeated_field(&msg, ref, field, &member_value, alloc);
			value->AddMember(field->name().c_str(), *alloc, member_value, *alloc);
		}
		else if(field->is_required() || ref->HasField(msg,field)) {
			priv_encode_field(&msg, ref, field, &member_value, alloc);
			value->AddMember(field->name().c_str(), *alloc, member_value, *alloc);
		}
	}

	return true;
}

} //end namespace Protobuf2Rapidjson
