#pragma once
#include "pch.hpp"
#include "myplugincids.hpp"
#include "Modules.hpp"
#include "ViewFactoryBase.hpp"

namespace Kaixo
{
    template<class Ty, class CT>
    struct Attr
    {
        const char* name;
        CT(Ty::* value);
    };

    template<class Type, class AttributesType>
    class ViewFactoryBase : public ViewCreatorAdapter
    {
    public:
        constexpr static auto seq = std::make_index_sequence<std::tuple_size_v<decltype(AttributesType::Attributes)>>{};

        ViewFactoryBase() { UIViewFactory::registerViewCreator(*this); }

        IdStringPtr getViewName() const override { return AttributesType::Name; }
        IdStringPtr getBaseViewName() const override { return AttributesType::BaseView; }

        CView* create(const UIAttributes& attributes, const IUIDescription* description) const override
        {
            if constexpr (std::constructible_from<Type, CRect>)
            {
                CRect _size{ CPoint{ 45, 45 }, AttributesType::Size };
                auto* _value = new Type(_size);
                apply(_value, attributes, description);
                return _value;
            }
            return nullptr;
        }

        bool apply(CView* view, const UIAttributes& attributes, const IUIDescription* description) const override
        {
            Type* _view = dynamic_cast<Type*>(view);
            if (!_view) return false;
            ApplyExpr(*_view, attributes, seq);
            return true;
        }

        bool getAttributeNames(StringList& attributeNames) const override
        {
            AttributeNamesExp(attributeNames, seq);
            return true;
        }

        AttrType getAttributeType(const string& attributeName) const override
        {
            if (attributeName == "") return kUnknownType;
            return AttrTypeExpr(attributeName, seq);
        }

        bool getAttributeValue(CView* view, const string& attributeName, string& stringValue, const IUIDescription* desc) const override
        {
            if (attributeName == "") return false;
            Type* _view = dynamic_cast<Type*>(view);
            if (!_view) return false;
            return Assign(*_view, attributeName, stringValue, seq);
        }

        template<size_t ...Is>
        bool Assign(Type& type, const string& attributeName, string& value, std::index_sequence<Is...>) const
        {
            return ((std::get<Is>(AttributesType::Attributes).name == attributeName ?
                value = StringizeType(type.*(std::get<Is>(AttributesType::Attributes).value)), true : false) || ...);
        }

        template<size_t ...Is>
        void AttributeNamesExp(StringList& names, std::index_sequence<Is...>) const
        {
            (names.push_back(std::get<Is>(AttributesType::Attributes).name), ...);
        }

        template<size_t ...Is>
        AttrType AttrTypeExpr(const string& attributeName, std::index_sequence<Is...>) const
        {
            AttrType _type = kUnknownType;
            ((std::get<Is>(AttributesType::Attributes).name == attributeName ?
                _type = GetAttrType<std::decay_t<decltype(std::declval<Type>().*(
                    std::get<Is>(AttributesType::Attributes).value))>>(), false : true) && ...);
                
            return _type;
        }

        template<class Ty> AttrType GetAttrType() const { return kUnknownType; }
        template<> AttrType GetAttrType<double>() const { return kFloatType; }
        template<> AttrType GetAttrType<float>() const { return kFloatType; }
        template<> AttrType GetAttrType<String>() const { return kStringType; }
        template<> AttrType GetAttrType<int>() const { return kIntegerType; }

        template<size_t ...Is>
        void ApplyExpr(Type& type, const UIAttributes& attributes, std::index_sequence<Is...>) const
        {
            ((attributes.getAttributeValue(std::get<Is>(AttributesType::Attributes).name) ? (type.*(std::get<Is>(AttributesType::Attributes).value))
                = ParseType<std::decay_t<decltype(type.*(std::get<Is>(AttributesType::Attributes).value))>>(
                    attributes.getAttributeValue(std::get<Is>(AttributesType::Attributes).name)), true : false), ...);
        }

        template<class Ty>
        Ty ParseType(const std::string* val) const;

        template<>
        double ParseType(const std::string* val) const
        {
            double value = 0;
            if (!val) return value;
            UIAttributes::stringToDouble(*val, value);
            return value;
        }

        template<>
        int ParseType(const std::string* val) const
        {
            int value = 0;
            if (!val) return value;
            UIAttributes::stringToInteger(*val, value);
            return value;
        }

        template<>
        String ParseType(const std::string* val) const
        {
            String value;
            if (!val) return value;
            value = (*val).c_str();
            return value;
        }

        template<class Ty>
        std::string StringizeType(const Ty& val) const;

        template<>
        std::string StringizeType(const double& val) const
        {
            return UIAttributes::doubleToString(val);
        }

        template<>
        std::string StringizeType(const int& val) const
        {
            return UIAttributes::integerToString(val);
        }

        template<>
        std::string StringizeType(const String& val) const
        {
            return val.operator const Steinberg::char8 *();
        }

    };
}