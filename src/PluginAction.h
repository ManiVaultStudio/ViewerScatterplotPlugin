#pragma once

#include "actions/Actions.h"

class ViewerScatterplotPlugin;
class ViewerScatterplotWidget;

class PluginAction : public hdps::gui::WidgetAction
{
public:
    PluginAction(QObject* parent, ViewerScatterplotPlugin* viewerscatterplotPlugin, const QString& title);

    ViewerScatterplotWidget& getViewerScatterplotWidget();

protected:
    ViewerScatterplotPlugin*  _viewerscatterplotPlugin;
};