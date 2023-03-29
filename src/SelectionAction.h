#pragma once

#include "actions/PixelSelectionAction.h"
#include "util/PixelSelectionTool.h"

#include <QActionGroup>
#include <QDebug>

class ViewerScatterplotPlugin;

using namespace hdps::gui;

class SelectionAction : public PixelSelectionAction
{
protected: // Widget

    class Widget : public WidgetActionWidget {
    public:
        Widget(QWidget* parent, SelectionAction* selectionAction, const std::int32_t& widgetFlags);
    };

    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this, widgetFlags);
    };

public:
    SelectionAction(ViewerScatterplotPlugin& viewerscatterplotPlugin);

public:
    OptionAction& getDisplayModeAction() { return _displayModeAction; }
    ToggleAction& getOutlineOverrideColorAction() { return _outlineOverrideColorAction; }
    DecimalAction& getOutlineScaleAction() { return _outlineScaleAction; }
    DecimalAction& getOutlineOpacityAction() { return _outlineOpacityAction; }
    ToggleAction& getOutlineHaloEnabledAction() { return _outlineHaloEnabledAction; }

protected:
    ViewerScatterplotPlugin&  _viewerscatterplotPlugin;             /** Reference to scatter plot plugin */
    OptionAction        _displayModeAction;             /** Type of selection display (e.g. outline or override) */
    ToggleAction        _outlineOverrideColorAction;    /** Selection outline override color action */
    DecimalAction       _outlineScaleAction;            /** Selection outline scale action */
    DecimalAction       _outlineOpacityAction;          /** Selection outline opacity action */
    ToggleAction        _outlineHaloEnabledAction;      /** Selection outline halo enabled action */
};