// SPDX-FileCopyrightText: 2023 Jeremias Bosch <jeremias.bosch@basyskom.com>
// SPDX-FileCopyrightText: 2023 basysKom GmbH
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QElapsedTimer>
#include <QQuickItem>
#include <QQuickPaintedItem>
#include <QSGRenderNode>
#include <QSGTextureProvider>
#include <QImage>
#include <QPainter>
#include <QTimer>
#include <QtCore/QtGlobal>

#include <rive/artboard.hpp>
#include <rive/listener_type.hpp>

#include "riveqtstatemachineinputmap.h"
#include "datatypes.h"
#include "renderer/riveqtfactory.h"

#if defined(RIVEQTQUICKITEM_LIBRARY)
#    define RIVEQTQUICKITEM_EXPORT Q_DECL_EXPORT
#else
#    define RIVEQTQUICKITEM_EXPORT Q_DECL_IMPORT
#endif

class RiveQSGRenderNode;
class RiveQSGRHIRenderNode;

class RiveQtQuickItem : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QString fileSource READ fileSource WRITE setFileSource NOTIFY fileSourceChanged)
    Q_PROPERTY(LoadingStatus loadingStatus READ loadingStatus NOTIFY loadingStatusChanged)
    Q_PROPERTY(QVector<ArtBoardInfo> artboards READ artboards NOTIFY artboardsChanged)
    Q_PROPERTY(QVector<AnimationInfo> animations READ animations NOTIFY animationsChanged)
    Q_PROPERTY(QVector<StateMachineInfo> stateMachines READ stateMachines NOTIFY stateMachinesChanged)

    Q_PROPERTY(int currentArtboardIndex READ currentArtboardIndex WRITE setCurrentArtboardIndex NOTIFY currentArtboardIndexChanged)
    Q_PROPERTY(int currentAnimationIndex READ currentAnimationIndex WRITE triggerAnimation NOTIFY currentAnimationIndexChanged)
    Q_PROPERTY(
        int currentStateMachineIndex READ currentStateMachineIndex WRITE setCurrentStateMachineIndex NOTIFY currentStateMachineIndexChanged)

    Q_PROPERTY(bool interactive READ interactive WRITE setInteractive NOTIFY interactiveChanged)

    Q_PROPERTY(RiveQtStateMachineInputMap *stateMachineInterface READ stateMachineInterface NOTIFY stateMachineInterfaceChanged)

    Q_PROPERTY(RiveRenderSettings::RenderQuality renderQuality READ renderQuality WRITE setRenderQuality NOTIFY renderQualityChanged)
    Q_PROPERTY(RiveRenderSettings::FillMode fillMode READ fillMode WRITE setFillMode NOTIFY fillModeChanged)

    Q_PROPERTY(int frameRate READ frameRate NOTIFY frameRateChanged)

    QML_ELEMENT

public:
    enum LoadingStatus
    {
        Idle,
        Loading,
        Loaded,
        Error,
        Unloading,
    };
    Q_ENUM(LoadingStatus)

    RiveQtQuickItem(QQuickItem *parent = nullptr);
    ~RiveQtQuickItem();

    Q_INVOKABLE void triggerAnimation(int id);

    bool isTextureProvider() const override { return true; }
    QSGTextureProvider *textureProvider() const override { return m_textureProvider.data(); }

    QString fileSource() const { return m_fileSource; }
    void setFileSource(const QString &source);

    LoadingStatus loadingStatus() const { return m_loadingStatus; }

    int currentAnimationIndex() const;
    int currentArtboardIndex() const;
    void setCurrentArtboardIndex(const int newCurrentArtboardIndex);

    const QVector<ArtBoardInfo> &artboards() const;
    const QVector<StateMachineInfo> &stateMachines() const { return m_stateMachineList; }
    const QVector<AnimationInfo> &animations() const;

    int currentStateMachineIndex() const;
    void setCurrentStateMachineIndex(const int newCurrentStateMachineIndex);

    RiveQtStateMachineInputMap *stateMachineInterface() const;

    bool interactive() const;
    void setInteractive(bool newInteractive);

    RiveRenderSettings::RenderQuality renderQuality() const { return m_renderSettings.renderQuality; }
    void setRenderQuality(const RiveRenderSettings::RenderQuality quality)
    {
        m_renderSettings.renderQuality = quality;
        emit renderQualityChanged();
    }

    RiveRenderSettings::FillMode fillMode() const { return m_renderSettings.fillMode; }
    void setFillMode(const RiveRenderSettings::FillMode fillMode)
    {
        m_renderSettings.fillMode = fillMode;
        emit fillModeChanged();
    }

    int frameRate() { return m_frameRate; }

signals:
    void animationsChanged();
    void artboardsChanged();
    void stateMachinesChanged();

    void fileSourceChanged();
    void loadingStatusChanged();

    void currentArtboardIndexChanged();
    void currentAnimationIndexChanged();
    void currentStateMachineIndexChanged();

    void interactiveChanged();

    void loadFileAfterUnloading(QString fileName);
    void internalArtboardChanged();
    void internalStateMachineChanged();
    void stateMachineInterfaceChanged();

    void renderQualityChanged();
    void fillModeChanged();

    void frameRateChanged();

protected:
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;
#else
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;
#endif

    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    void hoverMoveEvent(QHoverEvent *event) override;
    void hoverEnterEvent(QHoverEvent *event) override;
    void hoverLeaveEvent(QHoverEvent *event) override;

private slots:
    void updateStateMachineInputMap();

private:
    void loadRiveFile(const QString &source);

    void updateInternalArtboard();
    void updateAnimations();
    void updateStateMachines();
    void updateCurrentArtboardIndex();
    void updateCurrentStateMachineIndex();

    QRectF artboardRect();

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    friend RiveQSGRHIRenderNode;
    void renderOffscreen();
#endif

    bool hitTest(const QPointF &pos, const rive::ListenerType &type);

    QVector<ArtBoardInfo> m_artboardInfoList;
    QVector<AnimationInfo> m_animationList;
    QVector<StateMachineInfo> m_stateMachineList;

    std::unique_ptr<rive::File> m_riveFile;

    mutable QScopedPointer<QSGTextureProvider> m_textureProvider;

    QString m_fileSource;
    LoadingStatus m_loadingStatus { Idle };

    std::shared_ptr<rive::ArtboardInstance> m_currentArtboardInstance { nullptr };
    std::unique_ptr<rive::LinearAnimationInstance> m_animationInstance { nullptr };
    std::shared_ptr<rive::StateMachineInstance> m_currentStateMachineInstance { nullptr };

    bool m_scheduleArtboardChange { false };
    bool m_scheduleStateMachineChange { false };

    int m_currentArtboardIndex { -1 };
    int m_currentAnimationIndex { -1 };
    int m_initialArtboardIndex { -1 };
    int m_currentStateMachineIndex { -1 };
    int m_initialStateMachineIndex { -1 };

    RiveQtStateMachineInputMap *m_stateMachineInputMap { nullptr };

    RiveRenderSettings m_renderSettings;

    RiveQtFactory m_riveQtFactory { m_renderSettings };

    QElapsedTimer m_elapsedTimer;
    qint64 m_lastUpdateTime;
    bool m_geometryChanged { true };

    bool m_hasValidRenderNode { false };
    float m_lastMouseX { 0.f };
    float m_lastMouseY { 0.f };

    int m_frameRate { 0 };

    RiveQSGRenderNode *m_renderNode { nullptr };
};
