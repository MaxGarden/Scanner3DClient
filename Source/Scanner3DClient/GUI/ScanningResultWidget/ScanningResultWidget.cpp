#include "pch.h"
#include "ScanningResultWidget.h"

#include <QOpenGLContext>
#include <QMouseEvent>

using namespace Scanner3DClient;
using namespace Scanner3DClient::GUI;

const float ScanningResultWidget::s_aspectRatio = 1.0f;

ScanningResultWidget::ScanningResultWidget(QWidget* parent) :
    QOpenGLWidget{ parent }
{
}

void ScanningResultWidget::AddPoints(std::vector<Services::ScannerService::Point3D>&& points)
{
    std::move(points.begin(), points.end(), std::back_inserter(m_points));
}

void ScanningResultWidget::ClearPoints()
{
    m_points.clear();
}

void ScanningResultWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

void ScanningResultWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
    RecalculateViewport();
}

void ScanningResultWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(m_viewport.left(), m_viewport.right(), m_viewport.bottom(), m_viewport.top(), 0.0f, 20.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -2.0f);
    glRotatef(m_pitch, 1.0f, 0.0f, 0.0f);
    glRotatef(m_yaw, 0.0f, 1.0f, 0.0f);

    glBegin(GL_POINTS);
    glColor3f(0.0f, 0.0f, 0.0f);

    for (const auto& point : m_points)
        glVertex3f(point.X, point.Y, point.Z);

    glEnd();
    glLoadIdentity();

    glFlush();
}

void ScanningResultWidget::mousePressEvent(QMouseEvent* event)
{
    if (m_affectedMouseButton == Qt::NoButton)
    {
        m_initialMousePosition = event->localPos();
        m_initialYaw = m_yaw;
        m_initialPitch = m_pitch;
        m_initialOrigin = m_origin;
        m_affectedMouseButton = event->button();
    }

    return QOpenGLWidget::mousePressEvent(event);
}

void ScanningResultWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & m_affectedMouseButton)
    {
        auto mouseDelta = event->localPos() - m_initialMousePosition;

        switch (m_affectedMouseButton)
        {
        case Qt::MiddleButton:
        case Qt::LeftButton:
            mouseDelta.rx() /= static_cast<double>(s_aspectRatio) * height();
            mouseDelta.ry() /= -height();

            m_origin = m_initialOrigin + mouseDelta * m_zoom;
            RecalculateViewport();
            break;

        case Qt::RightButton:
            m_yaw = m_initialYaw + mouseDelta.x();
            m_pitch = m_initialPitch + mouseDelta.y();
            break;
        }
    }

    return QOpenGLWidget::mouseMoveEvent(event);
}

void ScanningResultWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == m_affectedMouseButton)
        m_affectedMouseButton = Qt::NoButton;

    return QOpenGLWidget::mouseReleaseEvent(event);
}

void ScanningResultWidget::wheelEvent(QWheelEvent* event)
{
    static const auto minimumZoomLevel = -15.0f;
    static const auto maximumZoomLevel = 15.0f;
    static const auto zoomInMultipler = 1.3f;
    static const auto zoomOutMultipler = 0.77f;

    const auto localMousePosition = mapFromGlobal(event->globalPos());
    auto normalizedMousePosition = QPointF{ static_cast<float>(localMousePosition.x()) / width(), static_cast<float>(localMousePosition.y()) / height() };

    const auto normalizedPointToViewport = [](const auto& point, const auto& viewport)
    {
        auto result = point;

        result.rx() *= viewport.width();
        result.ry() *= viewport.height();

        result.rx() += viewport.left();
        result.ry() += viewport.top();

        return result;
    };

    const auto minimumZoom = pow(zoomOutMultipler, abs(minimumZoomLevel));
    const auto maximumZoom = pow(zoomInMultipler, abs(maximumZoomLevel));

    m_zoomLevel += event->angleDelta().y() / -200.0f;
    m_zoomLevel = std::max(minimumZoomLevel, std::min(m_zoomLevel, maximumZoomLevel));

    m_zoom = m_zoomLevel > 0 ? pow(zoomInMultipler, m_zoomLevel) : pow(zoomOutMultipler, abs(m_zoomLevel));

    if (normalizedMousePosition.x() >= 0.0f && normalizedMousePosition.x() <= 1.0f && normalizedMousePosition.y() >= 0.0f && normalizedMousePosition.y() <= 1.0f)
    {
        const auto newViewport = CalculateViewport(m_origin, m_zoom);

        const auto focusPointOnNewViewport = normalizedPointToViewport(normalizedMousePosition, newViewport);
        const auto focusPointOnViewport = normalizedPointToViewport(normalizedMousePosition, m_viewport);

        m_origin += (focusPointOnNewViewport - focusPointOnViewport);
    }

    RecalculateViewport();
}

QRectF ScanningResultWidget::CalculateViewport(const QPointF& origin, float zoom) const
{
    const auto windowAspectRatio = static_cast<float>(width()) / height();
    const auto halfWindowAspectRatio = windowAspectRatio * 0.5f;

    const auto halfAspectRatio = s_aspectRatio * 0.5f;
    float left, right, top, bottom;

    if (s_aspectRatio < windowAspectRatio)
    {
        top = 0.5f;
        left = -halfWindowAspectRatio;
    }
    else 
    {
        left = -halfAspectRatio;
        top = halfAspectRatio / windowAspectRatio;
    }

    right = -left;
    bottom = -top;

    left *= zoom;
    right *= zoom;
    bottom *= zoom;
    top *= zoom;

    left -= origin.x();
    top -= origin.y();
    right -= origin.x();
    bottom -= origin.y();

    return { left, top, static_cast<double>(right) - left, static_cast<double>(bottom) - top };
}

void ScanningResultWidget::RecalculateViewport()
{
    m_viewport = CalculateViewport(m_origin, m_zoom);
}