#pragma once

#include "PluginAction.h"
#include "ColorSourceModel.h"

#include <PointData/DimensionPickerAction.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QStackedWidget>

using namespace hdps::gui;

/**
 * Coloring action class
 *
 * Action class for configuring the coloring of points
 *
 * @author Thomas Kroes
 */
class ColoringAction : public PluginAction
{
    Q_OBJECT

protected: // Widget

    /** Widget class for coloring action */
    class Widget : public WidgetActionWidget {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param coloringAction Pointer to coloring action
         */
        Widget(QWidget* parent, ColoringAction* coloringAction, const std::int32_t& widgetFlags);
    };

protected:

    /**
     * Get widget representation of the coloring action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this, widgetFlags);
    };

public:

    /**
     * Constructor
     * @param viewerscatterplotPlugin Pointer to scatter plot plugin
     */
    ColoringAction(ViewerScatterplotPlugin* viewerscatterplotPlugin);

    /**
     * Get the context menu for the action
     * @param parent Parent widget
     * @return Context menu
     */
    QMenu* getContextMenu(QWidget* parent = nullptr) override;

    /**
     * Add color dataset to the list
     * @param colorDataset Smart pointer to color dataset
     */
    void addColorDataset(const Dataset<DatasetImpl>& colorDataset);

    /** Determines whether a given color dataset is already loaded */
    bool hasColorDataset(const Dataset<DatasetImpl>& colorDataset) const;

    /** Get smart pointer to current color dataset (if any) */
    Dataset<DatasetImpl> getCurrentColorDataset() const;

    /**
     * Set the current color dataset
     * @param colorDataset Smart pointer to color dataset
     */
    void setCurrentColorDataset(const Dataset<DatasetImpl>& colorDataset);

protected:

    /** Update the color by action options */
    void updateColorByActionOptions();

    /** Update the colors of the points in the scatter plot widget */
    void updateScatterPlotWidgetColors();

protected: // Color map

    /** Updates the scalar range in the color map */
    void updateColorMapActionScalarRange();

    /** Update the scatter plot widget color map */
    void updateViewerScatterplotWidgetColorMap();

    /** Update the color map range in the scatter plot widget */
    void updateScatterPlotWidgetColorMapRange();

    /** Determine whether the color map should be enabled */
    bool shouldEnableColorMap() const;

    /** Enables/disables the color map */
    void updateColorMapActionReadOnly();

signals:
    void currentColorDatasetChanged(Dataset<DatasetImpl> currentColorDataset);

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

public: // Action getters

    OptionAction& getColorByAction() { return _colorByAction; }
    ColorAction& getConstantColorAction() { return _constantColorAction; }
    DimensionPickerAction& getDimensionAction() { return _dimensionAction; }
    ColorMapAction& getColorMapAction() { return _colorMapAction; }
    ColorMapAction& getColorMap2DAction() { return _colorMap2DAction; }

protected:
    ColorSourceModel        _colorByModel;              /** Color by model (model input for the color by action) */
    OptionAction            _colorByAction;             /** Action for picking the coloring type */
    ColorAction             _constantColorAction;       /** Action for picking the constant color */
    DimensionPickerAction   _dimensionAction;           /** Dimension picker action */
    ColorMapAction          _colorMapAction;            /** Color map action */
    ColorMapAction          _colorMap2DAction;          /** Color map 2D action */

    /** Default constant color */
    static const QColor DEFAULT_CONSTANT_COLOR;

    friend class Widget;
    friend class ViewerScatterplotPlugin;
};