/*
 * Copyright (C) 2009-2019 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#pragma once

#include "ExecutableToCodeBlockEdge.h"
#include "GlobalExecutable.h"

namespace JSC {

class ProgramExecutable final : public GlobalExecutable {
    friend class LLIntOffsetsExtractor;
public:
    using Base = GlobalExecutable;
    static constexpr unsigned StructureFlags = Base::StructureFlags | StructureIsImmortal;

    template<typename CellType, SubspaceAccess>
    static IsoSubspace* subspaceFor(VM& vm)
    {
        return &vm.programExecutableSpace.space;
    }

    static ProgramExecutable* create(ExecState* exec, const SourceCode& source)
    {
        VM& vm = exec->vm();
        ProgramExecutable* executable = new (NotNull, allocateCell<ProgramExecutable>(vm.heap)) ProgramExecutable(exec, source);
        executable->finishCreation(vm);
        return executable;
    }

    JSObject* initializeGlobalProperties(VM&, CallFrame*, JSScope*);

    static void destroy(JSCell*);

    ProgramCodeBlock* codeBlock()
    {
        return bitwise_cast<ProgramCodeBlock*>(ExecutableToCodeBlockEdge::unwrap(m_programCodeBlock.get()));
    }

    Ref<JITCode> generatedJITCode()
    {
        return generatedJITCodeForCall();
    }
        
    static Structure* createStructure(VM& vm, JSGlobalObject* globalObject, JSValue proto)
    {
        return Structure::create(vm, globalObject, proto, TypeInfo(ProgramExecutableType, StructureFlags), info());
    }
        
    DECLARE_INFO;

    ExecutableInfo executableInfo() const { return ExecutableInfo(usesEval(), isStrictMode(), false, false, ConstructorKind::None, JSParserScriptMode::Classic, SuperBinding::NotNeeded, SourceParseMode::ProgramMode, derivedContextType(), isArrowFunctionContext(), false, EvalContextType::None); }

    TemplateObjectMap& ensureTemplateObjectMap(VM&);

private:
    friend class ExecutableBase;
    friend class ScriptExecutable;

    ProgramExecutable(ExecState*, const SourceCode&);

    static void visitChildren(JSCell*, SlotVisitor&);

    WriteBarrier<UnlinkedProgramCodeBlock> m_unlinkedProgramCodeBlock;
    WriteBarrier<ExecutableToCodeBlockEdge> m_programCodeBlock;
    std::unique_ptr<TemplateObjectMap> m_templateObjectMap;
};

} // namespace JSC
