#include "MapStyleEvaluator_P.h"
#include "MapStyleEvaluator.h"

#include <cassert>

#include "MapStyle.h"
#include "MapStyle_P.h"
#include "MapStyleValueDefinition.h"
#include "MapStyleEvaluationResult.h"
#include "MapStyleEvaluationResult_P.h"
#include "MapStyleValue.h"
#include "MapStyleRule.h"
#include "MapStyleRule_P.h"
#include "MapObject.h"

OsmAnd::MapStyleEvaluator_P::MapStyleEvaluator_P( MapStyleEvaluator* owner_ )
    : owner(owner_)
    , _builtinValueDefs(MapStyle::getBuiltinValueDefinitions())
{
}

OsmAnd::MapStyleEvaluator_P::~MapStyleEvaluator_P()
{
}

bool OsmAnd::MapStyleEvaluator_P::evaluate(MapStyleEvaluationResult* const outResultStorage, bool evaluateChildren)
{
    if(owner->singleRule)
    {
        auto evaluationResult = evaluate(owner->singleRule, outResultStorage, evaluateChildren);
        if(evaluationResult)
            return true;

        return false;
    }
    else
    {
        const auto tagKey = _inputValues[_builtinValueDefs->id_INPUT_TAG].asUInt;
        const auto valueKey = _inputValues[_builtinValueDefs->id_INPUT_VALUE].asUInt;

        auto evaluationResult = evaluate(tagKey, valueKey, outResultStorage, evaluateChildren);
        if(evaluationResult)
            return true;

        evaluationResult = evaluate(tagKey, 0, outResultStorage, evaluateChildren);
        if(evaluationResult)
            return true;

        evaluationResult = evaluate(0, 0, outResultStorage, evaluateChildren);
        if(evaluationResult)
            return true;

        return false;
    }
}

bool OsmAnd::MapStyleEvaluator_P::evaluate(const uint32_t tagKey, const uint32_t valueKey, MapStyleEvaluationResult* const outResultStorage, bool evaluateChildren)
{
    _inputValues[_builtinValueDefs->id_INPUT_TAG].asUInt = tagKey;
    _inputValues[_builtinValueDefs->id_INPUT_VALUE].asUInt = valueKey;

    const auto& rules = owner->style->_d->obtainRules(owner->ruleset);
    uint64_t ruleId = MapStyle_P::encodeRuleId(tagKey, valueKey);
    auto itRule = rules.constFind(ruleId);
    if(itRule == rules.cend())
        return false;

    const auto evaluationResult = evaluate(*itRule, outResultStorage, evaluateChildren);
    return evaluationResult;
}

bool OsmAnd::MapStyleEvaluator_P::evaluate(const std::shared_ptr<const MapStyleRule>& rule, MapStyleEvaluationResult* const outResultStorage, bool evaluateChildren)
{
    // Check all values of a rule until all are checked.
    for(auto itRuleValueEntry = rule->_d->_values.cbegin(); itRuleValueEntry != rule->_d->_values.cend(); ++itRuleValueEntry)
    {
        const auto& valueDef = itRuleValueEntry.key();

        // Test only input values, the ones that start with INPUT_*
        if(valueDef->valueClass != MapStyleValueClass::Input)
            continue;

        const auto& ruleValue = *itRuleValueEntry.value();
        const auto& inputValue = _inputValues[valueDef->id];

        bool evaluationResult = false;
        if(valueDef->id == _builtinValueDefs->id_INPUT_MINZOOM)
        {
            assert(!ruleValue.isComplex);
            evaluationResult = (ruleValue.asSimple.asInt <= inputValue.asInt);
        }
        else if(valueDef->id == _builtinValueDefs->id_INPUT_MAXZOOM)
        {
            assert(!ruleValue.isComplex);
            evaluationResult = (ruleValue.asSimple.asInt >= inputValue.asInt);
        }
        else if(valueDef->id == _builtinValueDefs->id_INPUT_ADDITIONAL)
        {
            if(!owner->mapObject)
                evaluationResult = true;
            else
            {
                assert(!ruleValue.isComplex);
                const auto& strValue = owner->style->_d->lookupStringValue(ruleValue.asSimple.asUInt);
                auto equalSignIdx = strValue.indexOf('=');
                if(equalSignIdx >= 0)
                {
                    const auto& tag = strValue.mid(0, equalSignIdx);
                    const auto& value = strValue.mid(equalSignIdx + 1);
                    evaluationResult = owner->mapObject->containsTypeSlow(tag, value, true);
                }
                else
                    evaluationResult = false;
            }
        }
        else if(valueDef->dataType == MapStyleValueDataType::Float)
        {
            const auto lvalue = ruleValue.isComplex ? ruleValue.asComplex.asFloat.evaluate(owner->displayDensityFactor) : ruleValue.asSimple.asFloat;

            evaluationResult = qFuzzyCompare(lvalue, inputValue.asFloat);
        }
        else
        {
            const auto lvalue = ruleValue.isComplex ? ruleValue.asComplex.asInt.evaluate(owner->displayDensityFactor) : ruleValue.asSimple.asInt;

            evaluationResult = (lvalue == inputValue.asInt);
        }

        // If at least one value of rule does not match, it's failure
        if(!evaluationResult)
            return false;
    }

    // Fill output values from rule to result storage, if requested
    if(outResultStorage)
    {
        for(auto itRuleValueEntry = rule->_d->_values.cbegin(); itRuleValueEntry != rule->_d->_values.cend(); ++itRuleValueEntry)
        {
            const auto& valueDef = itRuleValueEntry.key();
            if(valueDef->valueClass != MapStyleValueClass::Output)
                continue;

            const auto& ruleValue = *itRuleValueEntry;

            switch(valueDef->dataType)
            {
            case MapStyleValueDataType::Boolean:
                assert(!ruleValue->isComplex);
                outResultStorage->_d->_podValues[valueDef->id].asUInt = ruleValue->asSimple.asUInt;
                break;
            case MapStyleValueDataType::Integer:
                outResultStorage->_d->_podValues[valueDef->id].asInt =
                    ruleValue->isComplex
                    ? ruleValue->asComplex.asInt.evaluate(owner->displayDensityFactor)
                    : ruleValue->asSimple.asInt;
                break;
            case MapStyleValueDataType::Float:
                outResultStorage->_d->_podValues[valueDef->id].asFloat =
                    ruleValue->isComplex
                    ? ruleValue->asComplex.asFloat.evaluate(owner->displayDensityFactor)
                    : ruleValue->asSimple.asFloat;
                break;
            case MapStyleValueDataType::String:
                // Save value of a string instead of it's id
                outResultStorage->_d->_stringValues[valueDef->id] =
                    owner->style->_d->lookupStringValue(itRuleValueEntry.value()->asSimple.asUInt);
                break;
            case MapStyleValueDataType::Color:
                assert(!ruleValue->isComplex);
                outResultStorage->_d->_podValues[valueDef->id].asUInt = ruleValue->asSimple.asUInt;
                break;
            }
        }
    }

    if(evaluateChildren)
    {
        for(auto itChild = rule->_d->_ifElseChildren.cbegin(); itChild != rule->_d->_ifElseChildren.cend(); ++itChild)
        {
            auto evaluationResult = evaluate(*itChild, outResultStorage, true);
            if(evaluationResult)
                break;
        }

        for(auto itChild = rule->_d->_ifChildren.cbegin(); itChild != rule->_d->_ifChildren.cend(); ++itChild)
        {
            evaluate(*itChild, outResultStorage, true);
        }
    }

    return true;
}
