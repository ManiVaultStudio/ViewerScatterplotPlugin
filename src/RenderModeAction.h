#pragma once

#include "PluginAction.h"

#include <QActionGroup>
#include <QHBoxLayout>

using namespace hdps::gui;

class QMenu;

class RenderModeAction : public PluginAction
{
protected: // Widget

    class Widget : public WidgetActionWidget {
    public:
        Widget(QWidget* parent, RenderModeAction* renderModeAction, const std::int32_t& widgetFlags);
    };

    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this, widgetFlags);
    };

public:
    RenderModeAction(ViewerScatterplotPlugin* viewerscatterplotPlugin);

    QMenu* getContextMenu();

public: // Serialization

    /**
     * Load widget action from variant map
     * @param Variant map representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant map
     * @return Variant map representation of the widget action
     */
    QVariantMap toVariantMap() const override;

protected:
    ToggleAction    _scatterPlotAction;
    ToggleAction    _densityPlotAction;
    ToggleAction    _contourPlotAction;
    QActionGroup    _actionGroup;

    friend class Widget;
};