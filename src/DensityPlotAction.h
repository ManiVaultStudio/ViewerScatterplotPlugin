#pragma once

#include "PluginAction.h"

#include <QLabel>

using namespace hdps::gui;

class PlotAction;

class DensityPlotAction : public PluginAction
{
protected:
    class Widget : public WidgetActionWidget {
    public:
        Widget(QWidget* parent, DensityPlotAction* densityPlotAction);
    };

    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this);
    };

public:
    DensityPlotAction(PlotAction* plotAction, ViewerScatterplotPlugin* viewerscatterplotPlugin);

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
    DecimalAction       _sigmaAction;
    ToggleAction        _continuousUpdatesAction;

    static constexpr double DEFAULT_SIGMA = 0.15f;
    static constexpr bool DEFAULT_CONTINUOUS_UPDATES = true;

    friend class Widget;
    friend class PlotAction;
};