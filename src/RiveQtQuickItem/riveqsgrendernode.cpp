// SPDX-FileCopyrightText: 2023 Jeremias Bosch <jeremias.bosch@basyskom.com>
// SPDX-FileCopyrightText: 2023 basysKom GmbH
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "riveqsgrendernode.h"
#include "riveqtquickitem.h"

QRectF RiveQSGRenderNode::rect() const
{
    return m_item->boundingRect();
}

RiveQSGRenderNode::RiveQSGRenderNode(std::weak_ptr<rive::ArtboardInstance> artboardInstance, RiveQtQuickItem *item)
    : RiveQSGBaseNode(artboardInstance, item)
{
}

RiveQSGBaseNode::RiveQSGBaseNode(std::weak_ptr<rive::ArtboardInstance> artboardInstance, RiveQtQuickItem *item)
    : m_artboardInstance(artboardInstance)
    , m_item(item)
{
}

void RiveQSGBaseNode::setRect(const QRectF &bounds)
{
    m_rect = bounds;
}

QPointF RiveQSGBaseNode::topLeft() const
{
    return m_topLeftRivePosition;
}

float RiveQSGBaseNode::scaleFactorX() const
{
    return m_scaleFactorX;
}

float RiveQSGBaseNode::scaleFactorY() const
{
    return m_scaleFactorY;
}

void RiveQSGBaseNode::setArtboardRect(const QRectF &bounds)
{
    m_topLeftRivePosition = bounds.topLeft();
    m_riveSize = bounds.size();

    if (auto artboardInstance = m_artboardInstance.lock(); artboardInstance) {
        m_scaleFactorX = bounds.width() / artboardInstance->width();
        m_scaleFactorY = bounds.height() / artboardInstance->height();
    }
}
