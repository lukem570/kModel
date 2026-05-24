#include "vulkanviewport.h"
#include "stlloader.h"

#include <QFile>
#include <QMouseEvent>
#include <QtMath>

// --- VulkanViewport ----------------------------------------------------------

VulkanViewport::VulkanViewport(QQuickItem *parent)
    : QQuickRhiItem(parent)
{
    setAcceptedMouseButtons(Qt::AllButtons);
    setSampleCount(4);
}

QQuickRhiItemRenderer *VulkanViewport::createRenderer()
{
    return new ModelRenderer;
}

void VulkanViewport::setLoader(StlLoader *loader)
{
    if (m_loader == loader)
        return;

    if (m_loader)
        disconnect(m_loader, nullptr, this, nullptr);

    m_loader = loader;

    if (m_loader) {
        connect(m_loader, &StlLoader::modelLoaded,
                this, &VulkanViewport::rebuildVertexData);
        if (m_loader->triangleCount() > 0)
            rebuildVertexData();
    }

    Q_EMIT loaderChanged();
}

void VulkanViewport::rebuildVertexData()
{
    if (!m_loader)
        return;

    const auto &triangles = m_loader->triangles();
    m_vertexCount = static_cast<int>(triangles.size()) * 3;

    // Interleaved layout: vec3 position + vec3 normal per vertex
    const int stride = 6 * sizeof(float);
    m_vertexData.resize(m_vertexCount * stride);
    auto *dst = reinterpret_cast<float *>(m_vertexData.data());

    for (const auto &tri : triangles) {
        const QVector3D *verts[] = {&tri.v0, &tri.v1, &tri.v2};
        for (const auto *v : verts) {
            *dst++ = v->x();
            *dst++ = v->y();
            *dst++ = v->z();
            *dst++ = tri.normal.x();
            *dst++ = tri.normal.y();
            *dst++ = tri.normal.z();
        }
    }

    // Center camera on model
    m_center = (m_loader->minBounds() + m_loader->maxBounds()) / 2.0f;
    float extent = (m_loader->maxBounds() - m_loader->minBounds()).length();
    m_distance = extent * 1.5f;

    m_meshDirty = true;
    update();
}

void VulkanViewport::mousePressEvent(QMouseEvent *event)
{
    m_lastMousePos = event->position();
    forceActiveFocus();
    event->accept();
}

void VulkanViewport::mouseMoveEvent(QMouseEvent *event)
{
    const QPointF delta = event->position() - m_lastMousePos;
    m_lastMousePos = event->position();

    if (event->buttons() & Qt::LeftButton) {
        // Orbit
        m_azimuth += static_cast<float>(delta.x()) * 0.005f;
        m_elevation -= static_cast<float>(delta.y()) * -0.005f;
        m_elevation = qBound(-1.5f, m_elevation, 1.5f);
    } else if (event->buttons() & Qt::MiddleButton) {
        // Pan
        const float panSpeed = m_distance * 0.001f;
        QVector3D eye = m_center + m_distance * QVector3D(
            qCos(m_elevation) * qCos(m_azimuth),
            qSin(m_elevation),
            qCos(m_elevation) * qSin(m_azimuth));
        QVector3D forward = (m_center - eye).normalized();
        QVector3D right = QVector3D::crossProduct(forward, QVector3D(0, 1, 0)).normalized();
        QVector3D up = QVector3D::crossProduct(right, forward).normalized();
        m_center -= right * static_cast<float>(delta.x()) * panSpeed;
        m_center += up * static_cast<float>(delta.y()) * panSpeed;
    } else if (event->buttons() & Qt::RightButton) {
        // Zoom via drag
        float factor = 1.0f + static_cast<float>(delta.y()) * 0.005f;
        m_distance *= factor;
        m_distance = qMax(0.001f, m_distance);
    }

    update();
    event->accept();
}

void VulkanViewport::wheelEvent(QWheelEvent *event)
{
    float factor = 1.0f - event->angleDelta().y() * 0.001f;
    m_distance *= factor;
    m_distance = qMax(0.001f, m_distance);
    update();
    event->accept();
}

// --- ModelRenderer -----------------------------------------------------------

QShader ModelRenderer::loadShader(const QString &path)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) {
        qWarning("Failed to open shader: %s", qPrintable(path));
        return {};
    }
    return QShader::fromSerialized(f.readAll());
}

void ModelRenderer::initialize(QRhiCommandBuffer *cb)
{
    Q_UNUSED(cb);

    if (m_rhi != rhi()) {
        m_pipeline.reset();
        m_vbuf.reset();
        m_rhi = rhi();
    }

    if (!m_pipeline) {
        // Uniform buffer: mat4 mvp (64) + mat4 model (64)
        //   + vec4 eyePos (16) + vec4 objectColor (16) = 160 bytes
        m_ubuf.reset(m_rhi->newBuffer(QRhiBuffer::Dynamic,
                                       QRhiBuffer::UniformBuffer, 160));
        m_ubuf->create();

        m_srb.reset(m_rhi->newShaderResourceBindings());
        m_srb->setBindings({
            QRhiShaderResourceBinding::uniformBuffer(
                0,
                QRhiShaderResourceBinding::VertexStage |
                    QRhiShaderResourceBinding::FragmentStage,
                m_ubuf.get())
        });
        m_srb->create();

        m_pipeline.reset(m_rhi->newGraphicsPipeline());

        m_pipeline->setShaderStages({
            {QRhiShaderStage::Vertex,
             loadShader(QStringLiteral(":/shaders/model.vert.qsb"))},
            {QRhiShaderStage::Fragment,
             loadShader(QStringLiteral(":/shaders/model.frag.qsb"))}
        });

        QRhiVertexInputLayout inputLayout;
        inputLayout.setBindings({{6 * sizeof(float)}});
        inputLayout.setAttributes({
            {0, 0, QRhiVertexInputAttribute::Float3, 0},
            {0, 1, QRhiVertexInputAttribute::Float3, 3 * sizeof(float)},
        });
        m_pipeline->setVertexInputLayout(inputLayout);

        m_pipeline->setDepthTest(true);
        m_pipeline->setDepthWrite(true);
        m_pipeline->setCullMode(QRhiGraphicsPipeline::None);
        m_pipeline->setShaderResourceBindings(m_srb.get());
        m_pipeline->setRenderPassDescriptor(
            renderTarget()->renderPassDescriptor());
        m_pipeline->setSampleCount(renderTarget()->sampleCount());
        m_pipeline->create();

        // Re-upload mesh if we already have data (e.g. after context loss)
        if (!m_vertexData.isEmpty())
            m_meshDirty = true;
    }
}

void ModelRenderer::synchronize(QQuickRhiItem *item)
{
    auto *vp = static_cast<VulkanViewport *>(item);

    if (vp->m_meshDirty) {
        m_vertexData = vp->m_vertexData;
        m_vertexCount = vp->m_vertexCount;
        m_meshDirty = true;
        vp->m_meshDirty = false;
    }

    // Compute camera matrices directly from current values
    m_eyePos = vp->m_center + vp->m_distance * QVector3D(
        qCos(vp->m_elevation) * qCos(vp->m_azimuth),
        qSin(vp->m_elevation),
        qCos(vp->m_elevation) * qSin(vp->m_azimuth));

    m_view.setToIdentity();
    m_view.lookAt(m_eyePos, vp->m_center, QVector3D(0, 1, 0));

    m_model.setToIdentity();

    const QSize sz = renderTarget()->pixelSize();
    const float aspect = sz.isEmpty()
        ? 1.0f
        : static_cast<float>(sz.width()) / static_cast<float>(sz.height());

    m_projection.setToIdentity();
    m_projection.perspective(45.0f, aspect,
                              0.01f * vp->m_distance,
                              100.0f * vp->m_distance);
}

void ModelRenderer::render(QRhiCommandBuffer *cb)
{
    QRhiResourceUpdateBatch *updates = m_rhi->nextResourceUpdateBatch();

    // Upload new mesh data if needed
    if (m_meshDirty && !m_vertexData.isEmpty()) {
        const quint32 requiredSize = static_cast<quint32>(m_vertexData.size());
        if (!m_vbuf || m_vbuf->size() < requiredSize) {
            m_vbuf.reset(m_rhi->newBuffer(QRhiBuffer::Immutable,
                                           QRhiBuffer::VertexBuffer,
                                           requiredSize));
            m_vbuf->create();
        }
        updates->uploadStaticBuffer(m_vbuf.get(), 0, requiredSize,
                                     m_vertexData.constData());
        m_meshDirty = false;
    }

    // Update uniform buffer
    QMatrix4x4 mvp = m_rhi->clipSpaceCorrMatrix() * m_projection * m_view * m_model;
    updates->updateDynamicBuffer(m_ubuf.get(), 0, 64, mvp.constData());
    updates->updateDynamicBuffer(m_ubuf.get(), 64, 64, m_model.constData());

    float eyePos[] = {m_eyePos.x(), m_eyePos.y(), m_eyePos.z(), 0.0f};
    updates->updateDynamicBuffer(m_ubuf.get(), 128, 16, eyePos);

    float objectColor[] = {0.55f, 0.58f, 0.63f, 0.0f};
    updates->updateDynamicBuffer(m_ubuf.get(), 144, 16, objectColor);

    // Render pass
    const QColor clearColor = QColor::fromRgbF(0.10f, 0.10f, 0.12f, 1.0f);
    cb->beginPass(renderTarget(), clearColor, {1.0f, 0}, updates);

    if (m_vbuf && m_vertexCount > 0) {
        cb->setGraphicsPipeline(m_pipeline.get());
        const QSize sz = renderTarget()->pixelSize();
        cb->setViewport({0, 0,
                         static_cast<float>(sz.width()),
                         static_cast<float>(sz.height())});
        cb->setShaderResources();
        const QRhiCommandBuffer::VertexInput vbufBinding(m_vbuf.get(), 0);
        cb->setVertexInput(0, 1, &vbufBinding);
        cb->draw(m_vertexCount);
    }

    cb->endPass();
}
