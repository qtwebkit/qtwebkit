/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2010 University of Szeged. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

#include "PlatformWebView.h"
#include "qquickwebpage_p.h"
#include "qquickwebview_p.h"

#include <QApplication>
#include <QEventLoop>
#include <QQmlProperty>
#include <QtQuick/QQuickView>
#include <QtQuick/private/qquickwindow_p.h>
#include <WebKit/WKImageQt.h>
#include <qpa/qwindowsysteminterface.h>

namespace WTR {

class WrapperWindow : public QQuickView {
    Q_OBJECT
public:
    WrapperWindow(QQuickWebView* view)
        : QQuickView(QUrl("data:text/plain,import QtQuick 2.0\nItem { objectName: 'root' }"))
        , m_view(view)
    {
        if (status() == QQuickView::Ready)
            handleStatusChanged(QQuickView::Ready);
        else
            connect(this, SIGNAL(statusChanged(QQuickView::Status)), SLOT(handleStatusChanged(QQuickView::Status)));
    }

private Q_SLOTS:
    void handleStatusChanged(QQuickView::Status status)
    {
        if (status != QQuickView::Ready)
            return;

        setGeometry(0, 0, 800, 600);

        setResizeMode(QQuickView::SizeRootObjectToView);
        m_view->setParentItem(rootObject());
        QQmlProperty::write(m_view, "anchors.fill", qVariantFromValue(rootObject()));

        if (PlatformWebView::windowSnapshotEnabled()) {
            setSurfaceType(OpenGLSurface);
            create();
#if QT_VERSION < QT_VERSION_CHECK(5, 1, 0)
            QQuickWindowPrivate::get(this)->setRenderWithoutShowing(true);
#else
            m_view->experimental()->setRenderToOffscreenBuffer(true);
#endif
        } else
            m_view->experimental()->setRenderToOffscreenBuffer(true);

        QWindowSystemInterface::handleWindowActivated(this);
        m_view->page()->setFocus(true);
    }

private:
    QQuickWebView* m_view;
};

PlatformWebView::PlatformWebView(WKPageConfigurationRef pageConfRef,const TestOptions& options)
    : m_windowIsKey(true)
    , m_options(options)
    , m_modalEventLoop(0)
{

    m_view = new QQuickWebView(pageConfRef);
    m_view->setAllowAnyHTTPSCertificateForLocalHost(true);
    m_view->componentComplete();
    m_window = new WrapperWindow(m_view);
}

PlatformWebView::~PlatformWebView()
{
    delete m_window;
    if (m_modalEventLoop)
        m_modalEventLoop->exit();
}

void PlatformWebView::setWindowIsKey(bool isKey)
{
    m_windowIsKey = isKey;
}

void PlatformWebView::resizeTo(unsigned width, unsigned height,WebViewSizingMode)
{
    // If we do not have a platform window we will never get the necessary
    // resize event, so simulate it in that case to make sure the quickview is
    // resized to what the layout test expects.
    if (!m_window->handle()) {
        QRect newGeometry(m_window->x(), m_window->y(), width, height);
        QWindowSystemInterface::handleGeometryChange(m_window, newGeometry);
        QWindowSystemInterface::flushWindowSystemEvents();
    }

    m_window->resize(width, height);
}

WKPageRef PlatformWebView::page()
{
    return m_view->pageRef();
}

void PlatformWebView::focus()
{
    QWindowSystemInterface::handleWindowActivated(m_window);
    m_view->setFocus(true);
}

WKRect PlatformWebView::windowFrame()
{
    QRect windowRect = m_window->geometry();
    WKRect wkFrame;
    wkFrame.origin.x = windowRect.x();
    wkFrame.origin.y = windowRect.y();
    wkFrame.size.width = windowRect.size().width();
    wkFrame.size.height = windowRect.size().height();
    return wkFrame;
}

void PlatformWebView::setWindowFrame(WKRect wkRect,WebViewSizingMode)
{
    m_window->setGeometry(wkRect.origin.x, wkRect.origin.y, wkRect.size.width, wkRect.size.height);
}

bool PlatformWebView::sendEvent(QEvent* event)
{
    return QCoreApplication::sendEvent(m_window, event);
}

void PlatformWebView::postEvent(QEvent* event)
{
    QCoreApplication::postEvent(m_window, event);
}

void PlatformWebView::addChromeInputField()
{
}

void PlatformWebView::removeChromeInputField()
{
}

void PlatformWebView::makeWebViewFirstResponder()
{
}

PlatformImage PlatformWebView::windowSnapshotImage()
{
    return m_window->grabWindow();
}

bool PlatformWebView::windowSnapshotEnabled()
{
    // We need a way to disable UI side rendering for tests because it is
    // too slow without appropriate hardware.
    static bool result;
    static bool hasChecked = false;
    if (!hasChecked)
        result = qgetenv("QT_WEBKIT_DISABLE_UIPROCESS_DUMPPIXELS") != "1";
    return result;
}

void PlatformWebView::addToWindow()
{
}

void PlatformWebView::didInitializeClients()
{
}

void PlatformWebView::setNavigationGesturesEnabled(bool)
{
}

void PlatformWebView::changeWindowScaleIfNeeded(float)
{
}

} // namespace WTR

#include "PlatformWebViewQt.moc"
