#include "core/value_impl.h"
#include <array>

namespace core
{
    namespace detail 
    {
        class ValueImplNone : public ValueImpl
        {
        public:
            virtual void freeValue(AnyValue& value) override
            {
                value.reset();
            }
            virtual void copyValue(const AnyValue& from, AnyValue& value) override
            {                
            }

			virtual string toChars(const AnyValue& value) override
			{
				return "None Type";
			}
        };

        class ValueImplBool : public ValueImpl
        {
        public:
            virtual void freeValue(AnyValue& value) override
            {
                value.reset();
            }
            virtual void copyValue(const AnyValue& from, AnyValue& value) override
            {                
            }

			virtual string toChars(const AnyValue& value) override
			{
				return value.b ? "true" : "false";
			}
        };

        class ValueImplInt : public ValueImpl
        {
        public:
            virtual void freeValue(AnyValue& value) override
            {
                value.i = 0;
            }

            virtual void copyValue(const AnyValue& from, AnyValue& value) override
            {
                value.i = from.i;
            }

			virtual string toChars(const AnyValue& value) override
			{
				return std::to_string(value.i);
			}

        };

        class ValueImplDouble : public ValueImpl
        {
        public:
            virtual void freeValue(AnyValue& value) override
            {
                value.d = 0;
            }
            virtual void copyValue(const AnyValue& from, AnyValue& value) override
            {
                value.d = from.d;
            }

			virtual string toChars(const AnyValue& value) override
			{
				return std::to_string(value.i);
			}
        };

        class ValueImplSting : public ValueImpl
        {
        public:
            virtual void freeValue(AnyValue& value) override
            {
                value.s.clear();
            }
            virtual void copyValue(const AnyValue& from, AnyValue& value) override
            {
                value.s = from.s;
            }
			virtual string toChars(const AnyValue& value) override
			{
				return value.s;
			}

        };

        static std::array<ValueImpl*, 5> g_imps =
            {
                new ValueImplNone,
                new ValueImplBool,
                new ValueImplInt,
                new ValueImplDouble,
                new ValueImplSting
            };
    }

    ValueImpl* getValueImpl(ValueType type)
    {
        return detail::g_imps[static_cast<int>(type)];
    }

}