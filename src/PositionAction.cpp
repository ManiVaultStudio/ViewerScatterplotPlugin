#include "PositionAction.h"
#include "Application.h"

#include "ViewerScatterplotPlugin.h"

#include <QMenu>
#include <QComboBox>

using namespace hdps::gui;

PositionAction::PositionAction(ViewerScatterplotPlugin* viewerscatterplotPlugin) :
    PluginAction(viewerscatterplotPlugin, viewerscatterplotPlugin, "Position"),
    _xDimensionPickerAction(this, "X"),
    _yDimensionPickerAction(this, "Y")
{
    setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("ruler-combined"));
    setSerializationName("Position");

    _xDimensionPickerAction.setSerializationName("X");
    
    _yDimensionPickerAction.setSerializationName("Y");
    
    // Add actions to scatter plot plugin (for shortcuts)
    _viewerscatterplotPlugin->getWidget().addAction(&_xDimensionPickerAction);
    _viewerscatterplotPlugin->getWidget().addAction(&_yDimensionPickerAction);

    // Set tooltips
    _xDimensionPickerAction.setToolTip("X dimension");
    _yDimensionPickerAction.setToolTip("Y dimension");

   

    // Update scatter plot when the x-dimension changes
    connect(&_xDimensionPickerAction, &DimensionPickerAction::currentDimensionIndexChanged, [this, viewerscatterplotPlugin](const std::uint32_t& currentDimensionIndex) {
        viewerscatterplotPlugin->setXDimension(currentDimensionIndex);
    });

    // Update scatter plot when the y-dimension changes
    connect(&_yDimensionPickerAction, &DimensionPickerAction::currentDimensionIndexChanged, [this, viewerscatterplotPlugin](const std::uint32_t& currentDimensionIndex) {
        viewerscatterplotPlugin->setYDimension(currentDimensionIndex);
    });

    // Set dimension defaults when the position dataset changes
    connect(&viewerscatterplotPlugin->getPositionDataset(), &Dataset<Points>::changed, this, [this]() {
        // Assign position dataset to x- and y dimension action
        _xDimensionPickerAction.setPointsDataset(_viewerscatterplotPlugin->getPositionDataset());
        _yDimensionPickerAction.setPointsDataset(_viewerscatterplotPlugin->getPositionDataset());

        auto xdimensionNames = _xDimensionPickerAction.getDimensionNames();
        if(xdimensionNames.contains("_Overview_X"))
        {
            _xDimensionPickerAction.setDefaultDimensionName("_Overview_X");
            _xDimensionPickerAction.setCurrentDimensionName("_Overview_X");
        }
        else
        {
            // Assign current and default index to x-dimension action
            _xDimensionPickerAction.setCurrentDimensionIndex(0);
            _xDimensionPickerAction.setDefaultDimensionIndex(0);
        }

        auto ydimensionNames = _yDimensionPickerAction.getDimensionNames();
        if(ydimensionNames.contains("_Overview_Y"))
        {
            _yDimensionPickerAction.setDefaultDimensionName("_Overview_Y");
            _yDimensionPickerAction.setCurrentDimensionName("_Overview_Y");
        }
        else
        {
            // Establish y-dimension
            const auto yIndex = _xDimensionPickerAction.getNumberOfDimensions() >= 2 ? 1 : 0;

            // Assign current and default index to y-dimension action
            _yDimensionPickerAction.setCurrentDimensionIndex(yIndex);
            _yDimensionPickerAction.setDefaultDimensionIndex(yIndex);
        }

       
    });
}

QMenu* PositionAction::getContextMenu(QWidget* parent /*= nullptr*/)
{
    auto menu = new QMenu("Position", parent);

    auto xDimensionMenu = new QMenu("X dimension");
    auto yDimensionMenu = new QMenu("Y dimension");

    xDimensionMenu->addAction(&_xDimensionPickerAction);
    yDimensionMenu->addAction(&_yDimensionPickerAction);

    menu->addMenu(xDimensionMenu);
    menu->addMenu(yDimensionMenu);

    return menu;
}

std::int32_t PositionAction::getDimensionX() const
{
    return _xDimensionPickerAction.getCurrentDimensionIndex();
}

std::int32_t PositionAction::getDimensionY() const
{
    return _yDimensionPickerAction.getCurrentDimensionIndex();
}

void PositionAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    _xDimensionPickerAction.fromParentVariantMap(variantMap);
    _yDimensionPickerAction.fromParentVariantMap(variantMap);
}

QVariantMap PositionAction::toVariantMap() const
{
    QVariantMap variantMap = WidgetAction::toVariantMap();

    _xDimensionPickerAction.insertIntoVariantMap(variantMap);
    _yDimensionPickerAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

PositionAction::Widget::Widget(QWidget* parent, PositionAction* positionAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, positionAction, widgetFlags)
{
    auto xDimensionLabel    = positionAction->_xDimensionPickerAction.createLabelWidget(this);
    auto yDimensionLabel    = positionAction->_yDimensionPickerAction.createLabelWidget(this);
    auto xDimensionWidget   = positionAction->_xDimensionPickerAction.createWidget(this);
    auto yDimensionWidget   = positionAction->_yDimensionPickerAction.createWidget(this);

    xDimensionWidget->findChild<QComboBox*>("ComboBox")->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    yDimensionWidget->findChild<QComboBox*>("ComboBox")->setSizeAdjustPolicy(QComboBox::AdjustToContents);

    if (widgetFlags & PopupLayout) {
        auto layout = new QGridLayout();

        layout->addWidget(xDimensionLabel, 0, 0);
        layout->addWidget(xDimensionWidget, 0, 1);
        layout->addWidget(yDimensionLabel, 1, 0);
        layout->addWidget(yDimensionWidget, 1, 1);

        setPopupLayout(layout);
    }
    else {
        auto layout = new QHBoxLayout();

        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(xDimensionLabel);
        layout->addWidget(xDimensionWidget);
        layout->addWidget(yDimensionLabel);
        layout->addWidget(yDimensionWidget);

        setLayout(layout);
    }
}
