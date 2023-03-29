#include "ColorSourceModel.h"

#include <DataHierarchyItem.h>
#include <Application.h>
#include <Set.h>

using namespace hdps;

ColorSourceModel::ColorSourceModel(QObject* parent /*= nullptr*/) :
    QAbstractListModel(parent),
    _datasets(),
    _showFullPathName(true)
{
}

int ColorSourceModel::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
    // Constant color option, 2D colormap, plus the number of available datasets
    return _datasets.count() + 2;
}

int ColorSourceModel::rowIndex(const Dataset<DatasetImpl>& dataset) const
{
    // Only proceed if we have a valid dataset
    if (!dataset.isValid())
        return -1;

    if (!_datasets.contains(dataset))
        return -1;

    // Return the index of the dataset
    return _datasets.indexOf(dataset) + 2;
}

int ColorSourceModel::columnCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
    return 1;
}

QVariant ColorSourceModel::data(const QModelIndex& index, int role) const
{
    // Get row/column and smart pointer to the dataset
    const auto row      = index.row();
    const auto column   = index.column();
    const auto dataset  = getDataset(row);

    switch (role)
    {
        // Return palette icon for constant color and dataset icon otherwise
        case Qt::DecorationRole:
            return row > 1 ? dataset->getIcon() : Application::getIconFont("FontAwesome").getIcon("palette");

        // Return 'Constant' for constant color and dataset (full path) GUI name otherwise
        case Qt::DisplayRole:
        {
            if (row > 1)
            {
                if (row == 2)
                    return dataset->getGuiName();
                else
                    return _showFullPathName ? dataset->getDataHierarchyItem().getFullPathName() : dataset->getGuiName();
            }
            if (row == 1 )
                return "Scatter layout";
            else
                return "Constant";
        }

        default:
            break;
    }

    return QVariant();
}

void ColorSourceModel::addDataset(const Dataset<DatasetImpl>& dataset)
{
    // Insert row into model
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    {
        // Add the dataset
        _datasets << dataset;
    }
    endInsertRows();

    // Get smart pointer to last added dataset
    auto& addedDataset = _datasets.last();

    // Remove a dataset from the model when it is about to be deleted
    connect(&addedDataset, &Dataset<DatasetImpl>::dataAboutToBeRemoved, this, [this, &addedDataset]() {
        removeDataset(addedDataset);
    });

    // Notify others that the model has updated when the dataset GUI name changes
    connect(&addedDataset, &Dataset<DatasetImpl>::dataGuiNameChanged, this, [this, &addedDataset]() {

        // Get row index of the dataset
        const auto colorDatasetRowIndex = rowIndex(addedDataset);

        // Only proceed if we found a valid row index
        if (colorDatasetRowIndex < 0)
            return;

        // Establish model index
        const auto modelIndex = index(colorDatasetRowIndex, 0);

        // Only proceed if we have a valid model index
        if (!modelIndex.isValid())
            return;

        // Notify others that the data changed
        emit dataChanged(modelIndex, modelIndex);
    });
}

void ColorSourceModel::removeDataset(const Dataset<DatasetImpl>& dataset)
{
    // Get row index of the dataset
    const auto datasetRowIndex = rowIndex(dataset);

    // Remove row from model
    beginRemoveRows(QModelIndex(), datasetRowIndex, datasetRowIndex);
    {
        // Remove dataset from internal vector
        _datasets.removeOne(dataset);
    }
    endRemoveRows();
}

void ColorSourceModel::removeAllDatasets()
{
    // Remove row from model
    beginRemoveRows(QModelIndex(), 0, rowCount() - 1);
    {
        // Remove all datasets
        _datasets.clear();
    }
    endRemoveRows();

    // And update model data with altered datasets
    updateData();
}

const Datasets& ColorSourceModel::getDatasets() const
{
    return _datasets;
}

Dataset<DatasetImpl> ColorSourceModel::getDataset(const std::int32_t& rowIndex) const
{
    // Return empty smart pointer when out of range
    if (rowIndex <= 1 || rowIndex > (_datasets.count() + 1))
        return Dataset<DatasetImpl>();

    // Subtract the constant point size and 2D colormap rows
    return _datasets[rowIndex - 2];
}

bool ColorSourceModel::getShowFullPathName() const
{
    return _showFullPathName;
}

void ColorSourceModel::setShowFullPathName(const bool& showFullPathName)
{
    _showFullPathName = showFullPathName;

    updateData();
}

void ColorSourceModel::updateData()
{
    // Update the datasets string list model
    for (auto dataset : _datasets) {

        // Continue if the dataset is not valid
        if (!dataset.isValid())
            continue;

        // Get dataset model index
        const auto datasetModelIndex = index(_datasets.indexOf(dataset), 0);

        // Notify others that the data changed
        emit dataChanged(datasetModelIndex, datasetModelIndex);
    }
}
