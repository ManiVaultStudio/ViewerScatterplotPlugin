#include "RenderModeAction.h"
#include "Application.h"
#include "ViewerScatterplotPlugin.h"
#include "ViewerScatterplotWidget.h"

using namespace hdps::gui;

RenderModeAction::RenderModeAction(ViewerScatterplotPlugin* viewerscatterplotPlugin) :
    PluginAction(viewerscatterplotPlugin, viewerscatterplotPlugin, "Render mode"),
    _scatterPlotAction(this, "Scatter"),
    _densityPlotAction(this, "Density"),
    _contourPlotAction(this, "Contour"),
    _actionGroup(this)
{
    setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("image"));
    setSerializationName("RenderMode");

    _scatterPlotAction.setSerializationName("ScatterPlotToggle");
    _densityPlotAction.setSerializationName("DensityPlotToggle");
    _contourPlotAction.setSerializationName("ContourPlotToggle");

    _scatterPlotAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _densityPlotAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _contourPlotAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);

    _viewerscatterplotPlugin->getWidget().addAction(&_scatterPlotAction);
    _viewerscatterplotPlugin->getWidget().addAction(&_densityPlotAction);
    _viewerscatterplotPlugin->getWidget().addAction(&_contourPlotAction);

    _actionGroup.addAction(&_scatterPlotAction);
    _actionGroup.addAction(&_densityPlotAction);
    _actionGroup.addAction(&_contourPlotAction);

    _scatterPlotAction.setCheckable(true);
    _densityPlotAction.setCheckable(true);
    _contourPlotAction.setCheckable(true);

    _scatterPlotAction.setShortcut(QKeySequence("S"));
    _densityPlotAction.setShortcut(QKeySequence("D"));
    _contourPlotAction.setShortcut(QKeySequence("C"));

    _scatterPlotAction.setToolTip("Set render mode to scatter plot (S)");
    _densityPlotAction.setToolTip("Set render mode to density plot (D)");
    _contourPlotAction.setToolTip("Set render mode to contour plot (C)");

    /*
    const auto& fontAwesome = Application::getIconFont("FontAwesome");

    _scatterPlotAction.setIcon(fontAwesome.getIcon("braille"));
    _densityPlotAction.setIcon(fontAwesome.getIcon("cloud"));
    _contourPlotAction.setIcon(fontAwesome.getIcon("mountain"));
    */

    connect(&_scatterPlotAction, &QAction::toggled, this, [this](bool toggled) {
        if (toggled)
            getViewerScatterplotWidget().setRenderMode(ViewerScatterplotWidget::RenderMode::SCATTERPLOT);
    });

    connect(&_densityPlotAction, &QAction::toggled, this, [this](bool toggled) {
        if (toggled)
            getViewerScatterplotWidget().setRenderMode(ViewerScatterplotWidget::RenderMode::DENSITY);
    });

    connect(&_contourPlotAction, &QAction::toggled, this, [this](bool toggled) {
        if (toggled)
            getViewerScatterplotWidget().setRenderMode(ViewerScatterplotWidget::RenderMode::LANDSCAPE);
    });

    const auto updateButtons = [this]() -> void {
        const auto renderMode = getViewerScatterplotWidget().getRenderMode();

        _scatterPlotAction.setChecked(renderMode == ViewerScatterplotWidget::RenderMode::SCATTERPLOT);
        _densityPlotAction.setChecked(renderMode == ViewerScatterplotWidget::RenderMode::DENSITY);
        _contourPlotAction.setChecked(renderMode == ViewerScatterplotWidget::RenderMode::LANDSCAPE);
    };

    connect(&getViewerScatterplotWidget(), &ViewerScatterplotWidget::renderModeChanged, this, [this, updateButtons](const ViewerScatterplotWidget::RenderMode& renderMode) {
        updateButtons();
    });

    updateButtons();
}

QMenu* RenderModeAction::getContextMenu()
{
    auto menu = new QMenu("Render mode");

    menu->addAction(&_scatterPlotAction);
    menu->addAction(&_densityPlotAction);
    menu->addAction(&_contourPlotAction);

    return menu;
}

void RenderModeAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    _scatterPlotAction.fromParentVariantMap(variantMap);
    _densityPlotAction.fromParentVariantMap(variantMap);
    _contourPlotAction.fromParentVariantMap(variantMap);
}

QVariantMap RenderModeAction::toVariantMap() const
{
    QVariantMap variantMap = WidgetAction::toVariantMap();

    _scatterPlotAction.insertIntoVariantMap(variantMap);
    _densityPlotAction.insertIntoVariantMap(variantMap);
    _contourPlotAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

RenderModeAction::Widget::Widget(QWidget* parent, RenderModeAction* renderModeAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, renderModeAction, widgetFlags)
{
    auto layout = new QHBoxLayout();

    layout->setSpacing(3);

    layout->addWidget(renderModeAction->_scatterPlotAction.createWidget(this, ToggleAction::PushButton));
    layout->addWidget(renderModeAction->_densityPlotAction.createWidget(this, ToggleAction::PushButton));
    layout->addWidget(renderModeAction->_contourPlotAction.createWidget(this, ToggleAction::PushButton));

    if (widgetFlags & PopupLayout) {
        setPopupLayout(layout);
    }
    else {
        layout->setContentsMargins(0, 0, 0, 0);
        setLayout(layout);
    }
}
