#pragma once

#include <QObject>
#include <QUrl>
#include <QVector3D>
#include <QQmlEngine>
#include <vector>

struct Triangle {
    QVector3D normal;
    QVector3D v0, v1, v2;
};

class StlLoader : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(int triangleCount READ triangleCount NOTIFY modelLoaded)
    Q_PROPERTY(QString error READ error NOTIFY errorChanged)
    Q_PROPERTY(QVector3D minBounds READ minBounds NOTIFY modelLoaded)
    Q_PROPERTY(QVector3D maxBounds READ maxBounds NOTIFY modelLoaded)

public:
    explicit StlLoader(QObject *parent = nullptr);

    QUrl source() const;
    void setSource(const QUrl &url);

    int triangleCount() const;
    QString error() const;
    QVector3D minBounds() const;
    QVector3D maxBounds() const;

    const std::vector<Triangle> &triangles() const;

signals:
    void sourceChanged();
    void modelLoaded();
    void errorChanged();

private:
    bool loadBinaryStl(const QString &path);
    bool loadAsciiStl(const QString &path);
    void computeBounds();

    QUrl m_source;
    QString m_error;
    std::vector<Triangle> m_triangles;
    QVector3D m_min;
    QVector3D m_max;
};
