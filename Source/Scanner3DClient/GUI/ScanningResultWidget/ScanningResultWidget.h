#pragma once

#include "Services/Scanner/ScannerService.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QTimer>

namespace Scanner3DClient::GUI
{
    class ScanningResultWidget final : public QOpenGLWidget, protected QOpenGLFunctions
    {
    public:
        ScanningResultWidget(QWidget* parent);
        virtual ~ScanningResultWidget() override final = default;

        void AddPoints(std::vector<Services::ScannerService::Point3D>&& points);
        void ClearPoints();

    protected:
        virtual void initializeGL() override final;
        virtual void resizeGL(int width, int height) override final;
        virtual void paintGL() override final;

        virtual void mousePressEvent(QMouseEvent* event) override final;
        virtual void mouseMoveEvent(QMouseEvent* event) override final;
        virtual void mouseReleaseEvent(QMouseEvent* event) override final;

        virtual void wheelEvent(QWheelEvent* event) override final;

    private:
        QRectF CalculateViewport(const QPointF& origin, float zoom) const;
        void RecalculateViewport();

    private:
        std::vector<Services::ScannerService::Point3D> m_points;

        QPointF m_initialMousePosition;
        QPointF m_initialOrigin;
        float m_initialYaw = 0.0f;
        float m_initialPitch = 0.0f;

        float m_yaw = 0.0f;
        float m_pitch = 0.0f;

        float m_zoomLevel = 0.0f;
        float m_zoom = 1.0f;

        QPointF m_origin;
        QRectF m_viewport;
        Qt::MouseButton m_affectedMouseButton = Qt::NoButton;

        static const float s_aspectRatio;
    };
}