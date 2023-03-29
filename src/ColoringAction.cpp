#include "ColoringAction.h"
#include "Application.h"

#include "ViewerScatterplotPlugin.h"
#include "ViewerScatterplotWidget.h"
#include "DataHierarchyItem.h"

#include "PointData.h"
#include "ClusterData.h"

using namespace hdps::gui;

const QColor ColoringAction::DEFAULT_CONSTANT_COLOR = qRgb(93, 93, 225);

ColoringAction::ColoringAction(ViewerScatterplotPlugin* viewerscatterplotPlugin) :
    PluginAction(viewerscatterplotPlugin, viewerscatterplotPlugin, "Coloring"),
    _colorByModel(this),
    _colorByAction(this, "Color by"),
    _constantColorAction(this, "Constant color", DEFAULT_CONSTANT_COLOR, DEFAULT_CONSTANT_COLOR),
    _dimensionAction(this, "Dim"),
    _colorMapAction(this, "Color map"),
    _colorMap2DAction(this, "Color map 2D", ColorMap::Type::TwoDimensional, "example_c", "example_c")
{
    _colorMapAction.getSettingsAction().setDisabled(true);
    _colorMapAction.getSettingsAction().setVisible(false);
    _colorMapAction.getCustomColorMapAction().setChecked(true);
    setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("palette"));
    setSerializationName("Coloring");

    const auto globalColorMapName = "ViewerScatterplot::GlobalColorMap";

    _colorMapAction.setConnectionPermissionsFlag(WidgetAction::ConnectionPermissionFlag::All);

    if (viewerscatterplotPlugin->getFactory()->getNumberOfInstances() == 0)
        _colorMapAction.publish(globalColorMapName);
    else
        _colorMapAction.connectToPublicActionByName(globalColorMapName);

    _colorByAction.setSerializationName("ColorBy");
    _constantColorAction.setSerializationName("ConstantColor");
    _dimensionAction.setSerializationName("ColorDimension");
    _colorMapAction.setSerializationName("ColorMap");
    _colorMap2DAction.setSerializationName("ColorMap 2D");

    _viewerscatterplotPlugin->getWidget().addAction(&_colorByAction);
    _viewerscatterplotPlugin->getWidget().addAction(&_dimensionAction);

    _colorByAction.setCustomModel(&_colorByModel);
    _colorByAction.setToolTip("Color by");

    _colorMapAction.setVisible(false);
    _colorMap2DAction.setVisible(false);

    _colorMapAction.setConnectionPermissionsFlag(ConnectionPermissionFlag::All);

    // Update dataset picker when the position dataset changes
    connect(&_viewerscatterplotPlugin->getPositionDataset(), &Dataset<Points>::changed, this, [this]() {

        // Get reference to position dataset
        const auto positionDataset = _viewerscatterplotPlugin->getPositionDataset();

        // Do not update if no position dataset is loaded
        if (!positionDataset.isValid())
            return;

        // Reset the color datasets
        _colorByModel.removeAllDatasets();

        // Add the position dataset
        addColorDataset(positionDataset);

        // Get smart pointer to position source dataset
        const auto positionSourceDataset = _viewerscatterplotPlugin->getPositionSourceDataset();

        // Add source position dataset (if position dataset is derived)
        if (positionSourceDataset.isValid())
            addColorDataset(positionSourceDataset);

        // Update the color by action
        updateColorByActionOptions();

        // Reset the color by option
        _colorByAction.setCurrentIndex(0);
    });

    // Update dataset picker when the position source dataset changes
    connect(&_viewerscatterplotPlugin->getPositionSourceDataset(), &Dataset<Points>::changed, this, [this]() {

        // Get smart pointer to position source dataset
        const auto positionSourceDataset = _viewerscatterplotPlugin->getPositionSourceDataset();

        // Add source position dataset (if position dataset is derived)
        if (positionSourceDataset.isValid())
            addColorDataset(positionSourceDataset);

        // Update the color by action
        updateColorByActionOptions();
    });

    // Update when the color by option is changed
    connect(&_colorByAction, &OptionAction::currentIndexChanged, this, [this](const std::int32_t& currentIndex) {

        // Update scatter plot widget coloring mode
        _viewerscatterplotPlugin->getViewerScatterplotWidget().setColoringMode(currentIndex == 0 ? ViewerScatterplotWidget::ColoringMode::Constant : ViewerScatterplotWidget::ColoringMode::Data);

        // Set color by constant action visibility depending on the coloring type
        _constantColorAction.setVisible(_colorByAction.getCurrentIndex() == 0);

        // Set 1D/2D colormaps visible depending on the coloring type
        _colorMap2DAction.setVisible(_colorByAction.getCurrentIndex() == 1);
        _colorMapAction.setVisible(_colorByAction.getCurrentIndex() > 1);

        // Get smart pointer to current color dataset
        const auto currentColorDataset = getCurrentColorDataset();

        // Only proceed if we have a valid color dataset
        if (currentColorDataset.isValid()) {

            // Establish whether the current color dataset is of type points
            const auto currentColorDatasetTypeIsPointType = currentColorDataset->getDataType() == PointType;

            // Update dimension picker points dataset source
            _dimensionAction.setPointsDataset(currentColorDatasetTypeIsPointType ? Dataset<Points>(currentColorDataset) : Dataset<Points>());

            // Hide dimension picker action when not point type
            _dimensionAction.setVisible(currentColorDatasetTypeIsPointType);

            emit currentColorDatasetChanged(currentColorDataset);
        }
        else {

            // Disable the dimension picker (in constant mode)
            _dimensionAction.setPointsDataset(Dataset<Points>());
            _dimensionAction.setVisible(false);
        }

        updateScatterPlotWidgetColors();
        updateViewerScatterplotWidgetColorMap();
        updateColorMapActionScalarRange();
        updateColorMapActionReadOnly();
    });

    // Update child color datasets when a child is added to or removed from the points position dataset
    connect(&_viewerscatterplotPlugin->getPositionDataset(), &Dataset<Points>::dataChildAdded, this, &ColoringAction::updateColorByActionOptions);
    connect(&_viewerscatterplotPlugin->getPositionDataset(), &Dataset<Points>::dataChildRemoved, this, &ColoringAction::updateColorByActionOptions);

    // Update scatter plot widget colors when the scatter plot widget coloring/rendering mode changes
    connect(&_viewerscatterplotPlugin->getViewerScatterplotWidget(), &ViewerScatterplotWidget::renderModeChanged, this, &ColoringAction::updateScatterPlotWidgetColors);
    connect(&_viewerscatterplotPlugin->getViewerScatterplotWidget(), &ViewerScatterplotWidget::coloringModeChanged, this, &ColoringAction::updateScatterPlotWidgetColors);

    // Update scatter plot widget colors and color map range when the current dimension changes
    connect(&_dimensionAction, &DimensionPickerAction::currentDimensionIndexChanged, this, &ColoringAction::updateScatterPlotWidgetColors);
    connect(&_dimensionAction, &DimensionPickerAction::currentDimensionIndexChanged, this, &ColoringAction::updateColorMapActionScalarRange);

    // Update scatter plot widget color map when actions change
    connect(&_constantColorAction, &ColorAction::colorChanged, this, &ColoringAction::updateViewerScatterplotWidgetColorMap);
    connect(&_colorMapAction, &ColorMapAction::imageChanged, this, &ColoringAction::updateViewerScatterplotWidgetColorMap);
    connect(&_colorMap2DAction, &ColorMapAction::imageChanged, this, &ColoringAction::updateViewerScatterplotWidgetColorMap);
    connect(&_viewerscatterplotPlugin->getViewerScatterplotWidget(), &ViewerScatterplotWidget::coloringModeChanged, this, &ColoringAction::updateViewerScatterplotWidgetColorMap);
    connect(&_viewerscatterplotPlugin->getViewerScatterplotWidget(), &ViewerScatterplotWidget::renderModeChanged, this, &ColoringAction::updateViewerScatterplotWidgetColorMap);

    // Update scatter plot widget color map range when the color map action range changes
    connect(&_colorMapAction.getRangeAction(ColorMapAction::Axis::X), &DecimalRangeAction::rangeChanged, this, &ColoringAction::updateScatterPlotWidgetColorMapRange);

    // Enable/disable the color map action when the scatter plot widget rendering or coloring mode changes
    connect(&_viewerscatterplotPlugin->getViewerScatterplotWidget(), &ViewerScatterplotWidget::coloringModeChanged, this, &ColoringAction::updateColorMapActionReadOnly);
    connect(&_viewerscatterplotPlugin->getViewerScatterplotWidget(), &ViewerScatterplotWidget::renderModeChanged, this, &ColoringAction::updateColorMapActionReadOnly);

    updateViewerScatterplotWidgetColorMap();
    updateColorMapActionScalarRange();

    _viewerscatterplotPlugin->getViewerScatterplotWidget().setColoringMode(ViewerScatterplotWidget::ColoringMode::Constant);
}

QMenu* ColoringAction::getContextMenu(QWidget* parent /*= nullptr*/)
{
    auto menu = new QMenu("Color", parent);

    const auto addActionToMenu = [menu](QAction* action) -> void {
        auto actionMenu = new QMenu(action->text());

        actionMenu->addAction(action);

        menu->addMenu(actionMenu);
    };

    return menu;
}

void ColoringAction::addColorDataset(const Dataset<DatasetImpl>& colorDataset)
{
    // Do not add the same color dataset twice
    if (hasColorDataset(colorDataset))
        return;

    // Add the dataset to the model
    _colorByModel.addDataset(colorDataset);

    // Get smart pointer to added dataset
    auto& addedDataset = _colorByModel.getDatasets().last();

    for (const auto& dataset : _colorByModel.getDatasets()) {

        // Connect to the data changed signal so that we can update the scatter plot colors appropriately
        connect(&dataset, &Dataset<DatasetImpl>::dataChanged, this, [this, dataset]() {

            // Get smart pointer to current color dataset
            const auto currentColorDataset = getCurrentColorDataset();

            // Only proceed if we have a valid dataset for coloring
            if (!currentColorDataset.isValid())
                return;

            // Update colors if the dataset matches
            if (currentColorDataset == dataset)
                updateScatterPlotWidgetColors();
        });
    }
}

bool ColoringAction::hasColorDataset(const Dataset<DatasetImpl>& colorDataset) const
{
    return _colorByModel.rowIndex(colorDataset) >= 0;
}

Dataset<DatasetImpl> ColoringAction::getCurrentColorDataset() const
{
    // Get current color by option index
    const auto colorByIndex = _colorByAction.getCurrentIndex();

    // Only proceed if we have a valid color dataset row index
    if (colorByIndex < 2)
        return Dataset<DatasetImpl>();

    return _colorByModel.getDataset(colorByIndex);
}

void ColoringAction::setCurrentColorDataset(const Dataset<DatasetImpl>& colorDataset)
{
    addColorDataset(colorDataset);

    // Obtain row index of the color dataset
    const auto colorDatasetRowIndex = _colorByModel.rowIndex(colorDataset);

    // Set color by action current index if the color dataset was found
    if (colorDatasetRowIndex >= 0)
        _colorByAction.setCurrentIndex(colorDatasetRowIndex);

    emit currentColorDatasetChanged(colorDataset);
}

void ColoringAction::updateColorByActionOptions()
{
    // Get smart pointer to the position dataset
    auto positionDataset = _viewerscatterplotPlugin->getPositionDataset();

    // Only proceed if the position dataset is loaded
    if (!positionDataset.isValid())
        return;

    // Get child data hierarchy items of the position dataset
    const auto children = positionDataset->getDataHierarchyItem().getChildren();

    // Loop over all children and possibly add them to the color datasets
    for (auto child : children) {

        // Get smart pointer to child dataset
        const auto childDataset = child->getDataset();

        // Get the data type
        const auto dataType = childDataset->getDataType();

        // Add if points/clusters and not derived
        if (dataType == PointType || dataType == ClusterType)
            addColorDataset(childDataset);
    }
}

void ColoringAction::updateScatterPlotWidgetColors()
{
    // Only upload colors to scatter plot widget in data coloring mode
    if (_colorByAction.getCurrentIndex() <= 1)
        return;

    // Get smart pointer to current color dataset
    const auto currentColorDataset = getCurrentColorDataset();

    // Only proceed if we have a valid color dataset
    if (!currentColorDataset.isValid())
        return;

    if (currentColorDataset->getDataType() == ClusterType)
        _viewerscatterplotPlugin->loadColors(currentColorDataset.get<Clusters>());
    else {

        // Get current dimension index
        const auto currentDimensionIndex = _dimensionAction.getCurrentDimensionIndex();

        // Exit if dimension selection is not valid
        if (currentDimensionIndex >= 0)
            _viewerscatterplotPlugin->loadColors(currentColorDataset.get<Points>(), _dimensionAction.getCurrentDimensionIndex());
    }
}

void ColoringAction::updateColorMapActionScalarRange()
{
    const auto colorMapRange    = _viewerscatterplotPlugin->getViewerScatterplotWidget().getColorMapRange();
    const auto colorMapRangeMin = colorMapRange.x;
    const auto colorMapRangeMax = colorMapRange.y;

    auto& colorMapRangeAction = _colorMapAction.getRangeAction(ColorMapAction::Axis::X);


    // Initialize the color map range action with the color map range from the scatter plot 
    colorMapRangeAction.initialize({ colorMapRangeMin, colorMapRangeMax }, { colorMapRangeMin, colorMapRangeMax });
    _colorMapAction.getDataRangeAction(ColorMapAction::Axis::X).setRange({ colorMapRangeMin, colorMapRangeMax });
}

void ColoringAction::updateViewerScatterplotWidgetColorMap()
{
    // The type of color map depends on the type of rendering and coloring
    switch (_viewerscatterplotPlugin->getViewerScatterplotWidget().getRenderMode())
    {
        case ViewerScatterplotWidget::SCATTERPLOT:
        {
            if (_colorByAction.getCurrentIndex() == 0) {
                
                // Create 1x1 pixmap for the (constant) color map
                QPixmap colorPixmap(1, 1);

                // Fill it with the constant color
                colorPixmap.fill(_constantColorAction.getColor());

                // Update the scatter plot widget with the color map
                getViewerScatterplotWidget().setColorMap(colorPixmap.toImage());
                getViewerScatterplotWidget().setScalarEffect(PointEffect::Color);
                getViewerScatterplotWidget().setColoringMode(ViewerScatterplotWidget::ColoringMode::Constant);
            }
            else if (_colorByAction.getCurrentIndex() == 1) {
                
                // Update the scatter plot widget with the 2D color map
                getViewerScatterplotWidget().setColorMap(_colorMap2DAction.getColorMapImage());
                getViewerScatterplotWidget().setScalarEffect(PointEffect::Color2D);
                getViewerScatterplotWidget().setColoringMode(ViewerScatterplotWidget::ColoringMode::Scatter);
            }
            else {

                // Update the scatter plot widget with the color map
                getViewerScatterplotWidget().setColorMap(_colorMapAction.getColorMapImage().mirrored(false, true));
            }

            break;
        }

        case ViewerScatterplotWidget::DENSITY:
            break;

        case ViewerScatterplotWidget::LANDSCAPE:
        {
            // Update the scatter plot widget with the color map
            getViewerScatterplotWidget().setColorMap(_colorMapAction.getColorMapImage());

            break;
        }

        default:
            break;
    }
}

void ColoringAction::updateScatterPlotWidgetColorMapRange()
{
    // Get color map range action
    const auto& rangeAction = _colorMapAction.getRangeAction(ColorMapAction::Axis::X);

    // And assign scatter plot renderer color map range
    getViewerScatterplotWidget().setColorMapRange(rangeAction.getMinimum(), rangeAction.getMaximum());
}

bool ColoringAction::shouldEnableColorMap() const
{
    // Disable the color in density render mode
    if (_viewerscatterplotPlugin->getViewerScatterplotWidget().getRenderMode() == ViewerScatterplotWidget::DENSITY)
        return false;

    // Disable the color map in color by data mode
    if (_viewerscatterplotPlugin->getViewerScatterplotWidget().getColoringMode() == ViewerScatterplotWidget::ColoringMode::Constant)
        return false;

    // Get smart pointer to the current color dataset
    const auto currentColorDataset = getCurrentColorDataset();

    // Disable the color map when a clusters color dataset is loaded
    if (currentColorDataset.isValid() && currentColorDataset->getDataType() == ClusterType)
        return false;

    return true;
}

void ColoringAction::updateColorMapActionReadOnly()
{
    _colorMapAction.setEnabled(shouldEnableColorMap());
    _colorMap2DAction.setEnabled(shouldEnableColorMap());
}

void ColoringAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    _constantColorAction.fromParentVariantMap(variantMap);
    _dimensionAction.fromParentVariantMap(variantMap);
    _colorMapAction.fromParentVariantMap(variantMap);
    _colorMap2DAction.fromParentVariantMap(variantMap);
    _colorByAction.fromParentVariantMap(variantMap);
}

QVariantMap ColoringAction::toVariantMap() const
{
    QVariantMap variantMap = WidgetAction::toVariantMap();

    _colorByAction.insertIntoVariantMap(variantMap);
    _constantColorAction.insertIntoVariantMap(variantMap);
    _dimensionAction.insertIntoVariantMap(variantMap);
    _colorMapAction.insertIntoVariantMap(variantMap);
    _colorMap2DAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

ColoringAction::Widget::Widget(QWidget* parent, ColoringAction* coloringAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, coloringAction, widgetFlags)
{
    auto layout = new QHBoxLayout();

    // Enable/disable the widget depending on the render mode
    const auto renderModeChanged = [this, coloringAction]() {
        setEnabled(coloringAction->getViewerScatterplotWidget().getRenderMode() == ViewerScatterplotWidget::SCATTERPLOT);
    };

    // Enable/disable depending on the render mode
    connect(&coloringAction->getViewerScatterplotWidget(), &ViewerScatterplotWidget::renderModeChanged, this, renderModeChanged);

    // Initial update
    renderModeChanged();

    // Create widgets for actions
    auto labelWidget            = coloringAction->getColorByAction().createLabelWidget(this);
    auto colorByWidget          = coloringAction->getColorByAction().createWidget(this);
    auto colorByConstantWidget  = coloringAction->getConstantColorAction().createWidget(this);
    auto dimensionPickerLabelWidget = coloringAction->getDimensionAction().createLabelWidget(this);
    auto dimensionPickerWidget  = coloringAction->getDimensionAction().createWidget(this);

    // Adjust width of the constant color widget
    colorByConstantWidget->setFixedWidth(40);

    // Adjust size of the combo boxes to the contents
    colorByWidget->findChild<QComboBox*>("ComboBox")->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    dimensionPickerWidget->findChild<QComboBox*>("ComboBox")->setSizeAdjustPolicy(QComboBox::AdjustToContents);

    // Add widgets
    if (widgetFlags & PopupLayout) {
        auto layout = new QGridLayout();

        layout->addWidget(labelWidget, 0, 0);
        layout->addWidget(colorByWidget, 0, 1);
        layout->addWidget(colorByConstantWidget, 0, 2);
        layout->addWidget(dimensionPickerLabelWidget, 0, 3);
        layout->addWidget(dimensionPickerWidget, 0, 4);

        setPopupLayout(layout);
    }
    else {
        auto layout = new QHBoxLayout();

        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(labelWidget);
        layout->addWidget(colorByWidget);
        layout->addWidget(colorByConstantWidget);
        layout->addWidget(dimensionPickerLabelWidget);
        layout->addWidget(dimensionPickerWidget);

        setLayout(layout);
    }
}
