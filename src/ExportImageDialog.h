#pragma once

#include "ExportImageAction.h"

#include <QDialog>

class ViewerScatterplotPlugin;

/**
 * Export image dialog class
 *
 * Dialog for exporting to image/video
 *
 * @author Thomas Kroes
 */
class ExportImageDialog : public QDialog
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     * @param viewerscatterplotPlugin Reference to viewerscatterplot plugin
     */
    ExportImageDialog(QWidget* parent, ViewerScatterplotPlugin& viewerscatterplotPlugin);

    /** Get preferred size */
    QSize sizeHint() const override {
        return QSize(600, 500);
    }

    /** Get minimum size hint*/
    QSize minimumSizeHint() const override {
        return sizeHint();
    }

protected:
    ViewerScatterplotPlugin&      _viewerscatterplotPlugin;     /** Reference to viewerscatterplot plugin */
    ExportImageAction       _exportImageAction;     /** Export to image action */
};
