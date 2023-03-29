#include "PlotAction.h"
#include "ViewerScatterplotPlugin.h"
#include "ViewerScatterplotWidget.h"
#include "Application.h"

#include <QVBoxLayout>
#include <QHBoxLayout>

using namespace hdps::gui;

PlotAction::PlotAction(ViewerScatterplotPlugin* viewerscatterplotPlugin) :
    PluginAction(viewerscatterplotPlugin, viewerscatterplotPlugin, "Plot"),
    _pointPlotAction(this, viewerscatterplotPlugin),
    _densityPlotAction(this, viewerscatterplotPlugin)
{
    setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("paint-brush"));
    _pointPlotAction._sizeAction.getSourceAction().setVisible(false);
    _pointPlotAction._opacityAction.getSourceAction().setVisible(false);
    const auto updateRenderMode = [this]() -> void {
        _pointPlotAction.setVisible(getViewerScatterplotWidget().getRenderMode() == ViewerScatterplotWidget::SCATTERPLOT);
        _densityPlotAction.setVisible(getViewerScatterplotWidget().getRenderMode() != ViewerScatterplotWidget::SCATTERPLOT);
    };

    connect(&getViewerScatterplotWidget(), &ViewerScatterplotWidget::renderModeChanged, this, [this, updateRenderMode](const ViewerScatterplotWidget::RenderMode& renderMode) {
        updateRenderMode();
    });

    updateRenderMode();
}

QMenu* PlotAction::getContextMenu()
{
    switch (getViewerScatterplotWidget().getRenderMode())
    {
        case ViewerScatterplotWidget::RenderMode::SCATTERPLOT:
            return _pointPlotAction.getContextMenu();
            break;

        case ViewerScatterplotWidget::RenderMode::DENSITY:
        case ViewerScatterplotWidget::RenderMode::LANDSCAPE:
            return _densityPlotAction.getContextMenu();
            break;

        default:
            break;
    }

    return new QMenu("Plot");
}

void PlotAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    _pointPlotAction.fromParentVariantMap(variantMap);
    _densityPlotAction.fromParentVariantMap(variantMap);
}

QVariantMap PlotAction::toVariantMap() const
{
    QVariantMap variantMap = WidgetAction::toVariantMap();

    _pointPlotAction.insertIntoVariantMap(variantMap);
    _densityPlotAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

PlotAction::Widget::Widget(QWidget* parent, PlotAction* plotAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, plotAction, widgetFlags)
{
    QWidget* pointPlotWidget    = nullptr;
    QWidget* densityPlotWidget  = nullptr;

    if (widgetFlags & PopupLayout) {
        pointPlotWidget     = plotAction->_pointPlotAction.createWidget(this, WidgetActionWidget::PopupLayout);
        densityPlotWidget   = plotAction->_densityPlotAction.createWidget(this, WidgetActionWidget::PopupLayout);

        auto layout = new QVBoxLayout();

        layout->addWidget(pointPlotWidget);
        layout->addWidget(densityPlotWidget);

        setPopupLayout(layout);
    }
    else {
        pointPlotWidget = plotAction->_pointPlotAction.createWidget(this);
        densityPlotWidget = plotAction->_densityPlotAction.createWidget(this);

        auto layout = new QHBoxLayout();

        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(pointPlotWidget);
        layout->addWidget(densityPlotWidget);

        setLayout(layout);
    }

    const auto updateRenderMode = [plotAction, pointPlotWidget, densityPlotWidget]() -> void {
        const auto renderMode = plotAction->getViewerScatterplotWidget().getRenderMode();

        pointPlotWidget->setVisible(renderMode == ViewerScatterplotWidget::RenderMode::SCATTERPLOT);
        densityPlotWidget->setVisible(renderMode != ViewerScatterplotWidget::RenderMode::SCATTERPLOT);
    };

    connect(&plotAction->getViewerScatterplotWidget(), &ViewerScatterplotWidget::renderModeChanged, this, [this, updateRenderMode](const ViewerScatterplotWidget::RenderMode& renderMode) {
        updateRenderMode();
    });

    updateRenderMode();
}
