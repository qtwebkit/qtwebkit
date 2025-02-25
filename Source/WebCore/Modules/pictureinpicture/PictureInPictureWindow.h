/*
 * Copyright (C) 2019 Carlos Eduardo Ramalho <cadubentzen@gmail.com>.
 * Copyright (C) 2019 Apple Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#if ENABLE(PICTURE_IN_PICTURE_API)

#include "ActiveDOMObject.h"
#include "EventTarget.h"
#include <wtf/RefCounted.h>

namespace WebCore {

class PictureInPictureWindow final
    : public RefCounted<PictureInPictureWindow>
    , public ActiveDOMObject
    , public EventTargetWithInlineData {
    WTF_MAKE_ISO_ALLOCATED(PictureInPictureWindow);
public:
    static Ref<PictureInPictureWindow> create(ScriptExecutionContext&, int, int);
    virtual ~PictureInPictureWindow();

    int width() const { return m_width; }
    int height() const { return m_height; }

    // ActiveDOMObject
    const char* activeDOMObjectName() const final;

    using RefCounted<PictureInPictureWindow>::ref;
    using RefCounted<PictureInPictureWindow>::deref;

private:
    PictureInPictureWindow(ScriptExecutionContext&, int, int);

    // EventTarget
    void refEventTarget() final { ref(); }
    void derefEventTarget() final { deref(); }
    EventTargetInterface eventTargetInterface() const;
    ScriptExecutionContext* scriptExecutionContext() const;

    int m_width;
    int m_height;
};

} // namespace WebCore

#endif // ENABLE(PICTURE_IN_PICTURE_API)
