#include "stlloader.h"

#include <QFile>
#include <QDataStream>
#include <QTextStream>
#include <QFileInfo>

#include <cmath>
#include <limits>

StlLoader::StlLoader(QObject *parent)
    : QObject(parent)
    , m_min(std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max())
    , m_max(std::numeric_limits<float>::lowest(),
            std::numeric_limits<float>::lowest(),
            std::numeric_limits<float>::lowest())
{
}

QUrl StlLoader::source() const { return m_source; }

void StlLoader::setSource(const QUrl &url)
{
    if (m_source == url)
        return;

    m_source = url;
    emit sourceChanged();

    m_error.clear();
    m_triangles.clear();
    emit errorChanged();

    const QString path = url.toLocalFile();
    if (path.isEmpty()) {
        m_error = QStringLiteral("Invalid file path");
        emit errorChanged();
        return;
    }

    // Try binary first (most common), fall back to ASCII
    if (!loadBinaryStl(path) && !loadAsciiStl(path)) {
        if (m_error.isEmpty())
            m_error = QStringLiteral("Failed to parse STL file");
        emit errorChanged();
        return;
    }

    computeBounds();
    emit modelLoaded();
}

int StlLoader::triangleCount() const { return static_cast<int>(m_triangles.size()); }
QString StlLoader::error() const { return m_error; }
QVector3D StlLoader::minBounds() const { return m_min; }
QVector3D StlLoader::maxBounds() const { return m_max; }
const std::vector<Triangle> &StlLoader::triangles() const { return m_triangles; }

bool StlLoader::loadBinaryStl(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    // Binary STL: 80-byte header + 4-byte triangle count + triangles
    if (file.size() < 84)
        return false;

    file.seek(80);
    QDataStream stream(&file);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);

    quint32 numTriangles = 0;
    stream >> numTriangles;

    // Sanity check: each triangle is 50 bytes (12 floats + 2 byte attribute)
    const qint64 expectedSize = 84 + static_cast<qint64>(numTriangles) * 50;
    if (file.size() < expectedSize)
        return false;

    m_triangles.reserve(numTriangles);

    for (quint32 i = 0; i < numTriangles; ++i) {
        Triangle tri;
        float nx, ny, nz;
        stream >> nx >> ny >> nz;
        tri.normal = QVector3D(nx, ny, nz);

        float x, y, z;
        stream >> x >> y >> z;
        tri.v0 = QVector3D(x, y, z);
        stream >> x >> y >> z;
        tri.v1 = QVector3D(x, y, z);
        stream >> x >> y >> z;
        tri.v2 = QVector3D(x, y, z);

        quint16 attrByteCount;
        stream >> attrByteCount;

        m_triangles.push_back(tri);
    }

    return true;
}

bool StlLoader::loadAsciiStl(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&file);
    const QString firstLine = in.readLine().trimmed();
    if (!firstLine.startsWith(QStringLiteral("solid")))
        return false;

    Triangle tri;
    int vertexIndex = 0;

    while (!in.atEnd()) {
        const QString line = in.readLine().trimmed();

        if (line.startsWith(QStringLiteral("facet normal"))) {
            const QStringList parts = line.split(QLatin1Char(' '), Qt::SkipEmptyParts);
            if (parts.size() >= 5) {
                tri.normal = QVector3D(parts[2].toFloat(),
                                       parts[3].toFloat(),
                                       parts[4].toFloat());
            }
            vertexIndex = 0;
        } else if (line.startsWith(QStringLiteral("vertex"))) {
            const QStringList parts = line.split(QLatin1Char(' '), Qt::SkipEmptyParts);
            if (parts.size() >= 4) {
                QVector3D v(parts[1].toFloat(), parts[2].toFloat(), parts[3].toFloat());
                if (vertexIndex == 0) tri.v0 = v;
                else if (vertexIndex == 1) tri.v1 = v;
                else if (vertexIndex == 2) tri.v2 = v;
                vertexIndex++;
            }
        } else if (line.startsWith(QStringLiteral("endfacet"))) {
            m_triangles.push_back(tri);
        }
    }

    return !m_triangles.empty();
}

void StlLoader::computeBounds()
{
    m_min = QVector3D(std::numeric_limits<float>::max(),
                      std::numeric_limits<float>::max(),
                      std::numeric_limits<float>::max());
    m_max = QVector3D(std::numeric_limits<float>::lowest(),
                      std::numeric_limits<float>::lowest(),
                      std::numeric_limits<float>::lowest());

    for (const auto &tri : m_triangles) {
        for (const auto &v : {tri.v0, tri.v1, tri.v2}) {
            m_min.setX(std::min(m_min.x(), v.x()));
            m_min.setY(std::min(m_min.y(), v.y()));
            m_min.setZ(std::min(m_min.z(), v.z()));
            m_max.setX(std::max(m_max.x(), v.x()));
            m_max.setY(std::max(m_max.y(), v.y()));
            m_max.setZ(std::max(m_max.z(), v.z()));
        }
    }
}
