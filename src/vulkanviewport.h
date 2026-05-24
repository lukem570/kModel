#pragma once

#include <QQuickRhiItem>
#include <QQmlEngine>
#include <rhi/qrhi.h>
#include <QVector3D>
#include <QMatrix4x4>

class StlLoader;

class ModelRenderer : public QQuickRhiItemRenderer
{
public:
    void initialize(QRhiCommandBuffer *cb) override;
    void synchronize(QQuickRhiItem *item) override;
    void render(QRhiCommandBuffer *cb) override;

private:
    static QShader loadShader(const QString &path);

    QRhi *m_rhi = nullptr;
    std::unique_ptr<QRhiGraphicsPipeline> m_pipeline;
    std::unique_ptr<QRhiBuffer> m_vbuf;
    std::unique_ptr<QRhiBuffer> m_ubuf;
    std::unique_ptr<QRhiShaderResourceBindings> m_srb;

    QByteArray m_vertexData;
    int m_vertexCount = 0;
    bool m_meshDirty = false;

    QMatrix4x4 m_model;
    QMatrix4x4 m_view;
    QMatrix4x4 m_projection;
    QVector3D m_eyePos;
};

class VulkanViewport : public QQuickRhiItem
{
    Q_OBJECT
    QML_NAMED_ELEMENT(VulkanViewport)
    Q_PROPERTY(StlLoader *loader READ loader WRITE setLoader NOTIFY loaderChanged)

public:
    explicit VulkanViewport(QQuickItem *parent = nullptr);

    QQuickRhiItemRenderer *createRenderer() override;

    StlLoader *loader() const { return m_loader; }
    void setLoader(StlLoader *loader);

Q_SIGNALS:
    void loaderChanged();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    friend class ModelRenderer;

    void rebuildVertexData();

    StlLoader *m_loader = nullptr;

    QByteArray m_vertexData;
    int m_vertexCount = 0;
    bool m_meshDirty = false;

    float m_azimuth = 0.6f;
    float m_elevation = 0.4f;
    float m_distance = 3.0f;
    QVector3D m_center;

    QPointF m_lastMousePos;
};
