/*
 * Copyright 2016 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SKSL_FUNCTIONCALL
#define SKSL_FUNCTIONCALL

#include "src/sksl/ir/SkSLExpression.h"
#include "src/sksl/ir/SkSLFunctionDeclaration.h"

namespace SkSL {

/**
 * A function invocation.
 */
struct FunctionCall : public Expression {
    FunctionCall(int offset, const Type& type, const FunctionDeclaration& function,
                 std::vector<std::unique_ptr<Expression>> arguments)
    : INHERITED(offset, kFunctionCall_Kind, type)
    , fFunction(std::move(function))
    , fArguments(std::move(arguments)) {
        ++fFunction.fCallCount;
    }

    ~FunctionCall() override {
        --fFunction.fCallCount;
    }

    bool hasProperty(Property property) const override {
        if (property == Property::kSideEffects && (fFunction.fModifiers.fFlags &
                                                   Modifiers::kHasSideEffects_Flag)) {
            return true;
        }
        for (const auto& arg : fArguments) {
            if (arg->hasProperty(property)) {
                return true;
            }
        }
        return false;
    }

    int nodeCount() const override {
        int result = 1;
        for (const auto& a : fArguments) {
            result += a->nodeCount();
        }
        return result;
    }

    std::unique_ptr<Expression> clone() const override {
        std::vector<std::unique_ptr<Expression>> cloned;
        for (const auto& arg : fArguments) {
            cloned.push_back(arg->clone());
        }
        return std::unique_ptr<Expression>(new FunctionCall(fOffset, fType, fFunction,
                                                            std::move(cloned)));
    }

    String description() const override {
        String result = String(fFunction.fName) + "(";
        String separator;
        for (size_t i = 0; i < fArguments.size(); i++) {
            result += separator;
            result += fArguments[i]->description();
            separator = ", ";
        }
        result += ")";
        return result;
    }

    const FunctionDeclaration& fFunction;
    std::vector<std::unique_ptr<Expression>> fArguments;

    typedef Expression INHERITED;
};

}  // namespace SkSL

#endif
