#include "PluginAction.h"

#include "ViewerScatterplotPlugin.h"
#include "ViewerScatterplotWidget.h"

using namespace hdps::gui;

PluginAction::PluginAction(QObject* parent, ViewerScatterplotPlugin* viewerscatterplotPlugin, const QString& title) :
    WidgetAction(parent),
    _viewerscatterplotPlugin(viewerscatterplotPlugin)
{
    _viewerscatterplotPlugin->getWidget().addAction(this);

    setText(title);
    setToolTip(title);
}

ViewerScatterplotWidget& PluginAction::getViewerScatterplotWidget()
{
    Q_ASSERT(_viewerscatterplotPlugin != nullptr);

    return _viewerscatterplotPlugin->getViewerScatterplotWidget();
}