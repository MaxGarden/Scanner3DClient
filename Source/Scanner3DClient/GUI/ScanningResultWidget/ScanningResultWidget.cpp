#include "pch.h"
#include "ScanningResultWidget.h"

#include <QOpenGLContext>
#include <QMouseEvent>

using namespace Scanner3DClient;
using namespace Scanner3DClient::GUI;

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
}

void ScanningResultWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 20.0f);
    glScalef(m_translation, m_translation, 1.0f);

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
    if (!(event->buttons() & Qt::LeftButton))
        return QOpenGLWidget::mousePressEvent(event);

    m_initialMousePosition = event->localPos();
    m_initialYaw = m_yaw;
    m_initialPitch = m_pitch;
}

void ScanningResultWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (!(event->buttons() & Qt::LeftButton))
        return QOpenGLWidget::mouseMoveEvent(event);

    const auto mouseDelta = event->localPos() - m_initialMousePosition;

    m_yaw = m_initialYaw + mouseDelta.x();
    m_pitch = m_initialPitch + mouseDelta.y();
}

void ScanningResultWidget::wheelEvent(QWheelEvent* event)
{
    m_translation += event->angleDelta().y() / 200.0f;

    if (m_translation <= 0.0f)
        m_translation = 0.0f;
}