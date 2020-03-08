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

        };

        static std::array<ValueImpl*, sizeof(ValueType)> g_imps =
            {
                new ValueImplNone,
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