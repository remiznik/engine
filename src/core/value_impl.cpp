#include "core/types.h"
#include "core/assert.h"
#include "core/value_impl.h"
#include "core/value.h"
#include <array>

namespace core
{
    namespace detail 
    {
        class ValueImplNone : public ValueImpl
        {
        public:
            virtual void freeValue(AnyValue& value) const override
            {
                value.reset();
            }
            virtual void copyValue(const AnyValue& from, AnyValue& value) const override
            {                
            }

			virtual string toChars(const AnyValue& value) const override
			{
				return "None Type";
			}

            virtual double toDouble(const AnyValue& value) const override
			{
				return 0;
			}

            virtual bool toBool(const AnyValue& value) const override
			{
				return false;
			}

			virtual bool equal(const Value& self, const Value& other) const override
			{
				return other.type() == ValueType::None;
			}

			virtual int toInt(const AnyValue& value) const override
			{
				return 0;
			}

			virtual Value plus(const Value& self, const Value& other) const override
			{
				ASSERT(false, "Value None plus");
				return Value();
			}

			virtual shared_ptr<Object> toObj(const AnyValue& value) const override
			{
				ASSERT(false, "None is not object");
				return nullptr;
			}
        };

        class ValueImplBool : public ValueImpl
        {
        public:
            virtual void freeValue(AnyValue& value) const override
            {
                value.reset();
            }
            virtual void copyValue(const AnyValue& from, AnyValue& value) const override
            {                
            }

			virtual string toChars(const AnyValue& value) const override
			{
				return value.b ? "true" : "false";
			}

            virtual double toDouble(const AnyValue& value) const override
			{
				return value.b ? 1.0 : 0.0;
			}

            virtual bool toBool(const AnyValue& value) const override
			{
				return value.b;
			}

			virtual bool equal(const Value& self, const Value& other) const override
			{
				if (other.type() == ValueType::Bool)
				{
					return self.get<bool>() == other.get<bool>();
				}
				return false;
			}

			virtual int toInt(const AnyValue& value) const override
			{
				return int(toDouble(value));
			}

			virtual Value plus(const Value& self, const Value& other) const override
			{
				ASSERT(false, "Value bool plus");
				return Value();
			}

			virtual shared_ptr<Object> toObj(const AnyValue& value) const override
			{
				ASSERT(false, "Bool is not object");
				return nullptr;
			}
        };

        class ValueImplInt : public ValueImpl
        {
        public:
            virtual void freeValue(AnyValue& value) const override
            {
                value.i = 0;
            }

            virtual void copyValue(const AnyValue& from, AnyValue& value) const override
            {
                value.i = from.i;
            }

			virtual string toChars(const AnyValue& value) const override
			{
				return std::to_string(value.i);
			}

            virtual double toDouble(const AnyValue& value) const override
			{
				return double(value.i);
			}

            virtual bool toBool(const AnyValue& value) const override
			{
				return value.i != 0;
			}
			virtual bool equal(const Value& self, const Value& other) const override
			{
				if (other.type() == ValueType::Integer)
				{
					return self.get<int>() == other.get<int>();
				}
				return false;
			}

			virtual int toInt(const AnyValue& value) const override
			{
				return value.i;
			}

			virtual Value plus(const Value& self, const Value& other) const override
			{
				ASSERT(other.type() == ValueType::Integer, "Value Int pluse");
				return self.get<int>() + other.get<int>();
			}
			
			virtual shared_ptr<Object> toObj(const AnyValue& value) const override
			{
				ASSERT(false, "Int is not object");
				return nullptr;
			}
        };

        class ValueImplDouble : public ValueImpl
        {
        public:
            virtual void freeValue(AnyValue& value) const override
            {
                value.d = 0;
            }
            virtual void copyValue(const AnyValue& from, AnyValue& value) const override
            {
                value.d = from.d;
            }

			virtual string toChars(const AnyValue& value) const override
			{
				return std::to_string(value.d);
			}

            virtual double toDouble(const AnyValue& value) const override
			{
				return value.d;
			}

            virtual bool toBool(const AnyValue& value) const override
			{
				return value.d != 0.0;
			}

			virtual bool equal(const Value& self, const Value& other) const override
			{
				if (other.type() == ValueType::Double)
				{
					return self.get<double>() == other.get<double>();
				}
				return false;
			}

			virtual int toInt(const AnyValue& value) const override
			{
				return int(toDouble(value));
			}

			virtual Value plus(const Value& self, const Value& other) const override
			{
				ASSERT(other.type() == ValueType::Double, "Value double plus");
				return self.get<double>() + other.get<double>();
			}

			virtual shared_ptr<Object> toObj(const AnyValue& value) const override
			{
				ASSERT(false, "Double is not object");
				return nullptr;
			}

        };

        class ValueImplSting : public ValueImpl
        {
        public:
            virtual void freeValue(AnyValue& value) const override
            {
                value.s.clear();
            }
            virtual void copyValue(const AnyValue& from, AnyValue& value) const override
            {
                value.s = from.s;
            }
			virtual string toChars(const AnyValue& value) const override
			{
				return value.s;
			}

            virtual double toDouble(const AnyValue& value) const override
			{
				return std::stod(value.s);
			}

            virtual bool toBool(const AnyValue& value) const override
			{
				return !value.s.empty();
			}

			virtual bool equal(const Value& self, const Value& other) const override
			{
				if (other.type() == ValueType::String)
				{
					return self.get<string>() == other.get<string>();
				}
				return false;
			}

			virtual int toInt(const AnyValue& value) const override
			{
				return int(toDouble(value));
			}

			virtual Value plus(const Value& self, const Value& other) const override
			{
				ASSERT(other.type() == ValueType::String, "Value string plus");
				return self.get<string>() + other.get<string>();
			}

			virtual shared_ptr<Object> toObj(const AnyValue& value) const override
			{
				ASSERT(false, "String is not object");
				return nullptr;
			}
        };


		class ValueImplObject : public ValueImpl
        {
        public:
            virtual void freeValue(AnyValue& value) const override
            {
                value.obj = nullptr;
            }
            virtual void copyValue(const AnyValue& from, AnyValue& value) const override
            {                
				value.obj = from.obj;
            }

			virtual string toChars(const AnyValue& value) const override
			{
				return "Obj Type";
			}

            virtual double toDouble(const AnyValue& value) const override
			{
				ASSERT(false, "Can`t cast object to doubel");
				return 0;
			}

            virtual bool toBool(const AnyValue& value) const override
			{
				ASSERT(false, "Can`t cast object to bool");
				return false;
			}

			virtual bool equal(const Value& self, const Value& other) const override
			{
				ASSERT(false, "Cant equal objects");
				return false;
			}

			virtual int toInt(const AnyValue& value) const override
			{
				ASSERT(false, "Can`t cast object to int");
				return 0;
			}

			virtual Value plus(const Value& self, const Value& other) const override
			{
				ASSERT(false, "Object cant plus");
				return Value();
			}

			virtual shared_ptr<Object> toObj(const AnyValue& value) const override
			{
				return value.obj;
			}

        };


        static std::array<ValueImpl*, 6> g_imps =
            {
                new ValueImplNone,
                new ValueImplBool,
                new ValueImplInt,
                new ValueImplDouble,
                new ValueImplSting,
				new ValueImplObject
            };
    }

    ValueImpl* getValueImpl(ValueType type)
    {
        return detail::g_imps[static_cast<int>(type)];
    }

}
