#include "SubsetAction.h"
#include "ViewerScatterplotPlugin.h"
#include "PointData.h"

#include <Application.h>

#include <QMenu>

using namespace hdps;
using namespace hdps::gui;

SubsetAction::SubsetAction(ViewerScatterplotPlugin* viewerscatterplotPlugin) :
    PluginAction(viewerscatterplotPlugin, viewerscatterplotPlugin, "Subset"),
    _subsetNameAction(this, "Subset name"),
    _createSubsetAction(this, "Create subset"),
    _sourceDataAction(this, "Source data")
{
    setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("crop"));

    _subsetNameAction.setToolTip("Name of the subset");
    _createSubsetAction.setToolTip("Create subset from selected data points");

    connect(&_createSubsetAction, &QAction::triggered, this, [this]() {
        _viewerscatterplotPlugin->createSubset(_sourceDataAction.getCurrentIndex() == 1, _subsetNameAction.getString());
    });

    const auto onCurrentDatasetChanged = [this]() -> void {
        if (!_viewerscatterplotPlugin->getPositionDataset().isValid())
            return;

        const auto datasetGuiName = _viewerscatterplotPlugin->getPositionDataset()->getGuiName();

        QStringList sourceDataOptions;

        if (!datasetGuiName.isEmpty()) {
            const auto sourceDatasetGuiName = _viewerscatterplotPlugin->getPositionDataset()->getSourceDataset<Points>()->getGuiName();

            sourceDataOptions << QString("From: %1").arg(datasetGuiName);

            if (sourceDatasetGuiName != datasetGuiName)
                sourceDataOptions << QString("From: %1 (source data)").arg(sourceDatasetGuiName);
        }

        _sourceDataAction.setOptions(sourceDataOptions);
        _sourceDataAction.setEnabled(sourceDataOptions.count() >= 2);
    };

    connect(&viewerscatterplotPlugin->getPositionDataset(), &Dataset<Points>::changed, this, onCurrentDatasetChanged);

    onCurrentDatasetChanged();
}

QMenu* SubsetAction::getContextMenu()
{
    auto menu = new QMenu("Subset");

    menu->addAction(&_createSubsetAction);
    menu->addAction(&_sourceDataAction);

    return menu;
}

SubsetAction::Widget::Widget(QWidget* parent, SubsetAction* subsetAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, subsetAction)
{
    auto layout = new QHBoxLayout();

    layout->addWidget(subsetAction->_createSubsetAction.createWidget(this));
    layout->addWidget(subsetAction->_sourceDataAction.createWidget(this));

    if (widgetFlags & PopupLayout)
    {
        setPopupLayout(layout);
            
    } else {
        layout->setContentsMargins(0, 0, 0, 0);
        setLayout(layout);
    }
}