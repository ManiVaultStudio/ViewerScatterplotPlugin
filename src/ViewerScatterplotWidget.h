#pragma once

#include "renderers/PointRenderer.h"
#include "renderers/DensityRenderer.h"
#include "util/PixelSelectionTool.h"

#include "graphics/Vector2f.h"
#include "graphics/Vector3f.h"
#include "graphics/Matrix3f.h"
#include "graphics/Bounds.h"
#include "graphics/Selection.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>

#include <QMouseEvent>
#include <QMenu>

using namespace hdps;
using namespace hdps::gui;
using namespace hdps::util;

class ViewerScatterplotWidget : public QOpenGLWidget, QOpenGLFunctions_3_3_Core
{
    Q_OBJECT

public:
    enum RenderMode {
        SCATTERPLOT,
        DENSITY,
        LANDSCAPE
    };

    /** The way that point colors are determined */
    enum class ColoringMode {
        Constant,      /** Point color is a constant color */
        Data,          /** Determined by external dataset */
        Scatter,       /** Determined by scatter layout using a 2D colormap */
    };

public:
    ViewerScatterplotWidget();
    ~ViewerScatterplotWidget();

    /** Returns true when the widget was initialized and is ready to be used. */
    bool isInitialized();

    /** Get/set render mode */
    RenderMode getRenderMode() const;
    void setRenderMode(const RenderMode& renderMode);

    /** Get/set background color */
    QColor getBackgroundColor();
    void setBackgroundColor(QColor color);

    /** Get/set coloring mode */
    ColoringMode getColoringMode() const;
    void setColoringMode(const ColoringMode& coloringMode);

    /** Get reference to the pixel selection tool */
    PixelSelectionTool& getPixelSelectionTool();

    /**
     * Feed 2-dimensional data to the viewerscatterplot.
     */
    void setData(const std::vector<Vector2f>* data);
    void setHighlights(const std::vector<char>& highlights, const std::int32_t& numSelectedPoints);
    void setScalars(const std::vector<float>& scalars);

    /**
     * Set colors for each individual data point
     * @param colors Vector of colors (size must match that of the loaded points dataset)
     */
    void setColors(const std::vector<Vector3f>& colors);

    /**
     * Set point size scalars
     * @param pointSizeScalars Point size scalars
     */
    void setPointSizeScalars(const std::vector<float>& pointSizeScalars);

    /**
     * Set point opacity scalars
     * @param pointOpacityScalars Point opacity scalars (assume the values are normalized)
     */
    void setPointOpacityScalars(const std::vector<float>& pointOpacityScalars);

    void setScalarEffect(PointEffect effect);
    void setPointScaling(hdps::gui::PointScaling scalingMode);

    /**
     * Set sigma value for kernel density esitmation.
     * @param sigma kernel width as a fraction of the output square width. Typical values are [0.01 .. 0.5]
     */
    void setSigma(const float sigma);

    Bounds getBounds() const {
        return _dataBounds;
    }

    Vector3f getColorMapRange() const;
    void setColorMapRange(const float& min, const float& max);

    /**
     * Create screenshot
     * @param width Width of the screen shot (in pixels)
     * @param height Height of the screen shot (in pixels)
     * @param backgroundColor Background color of the screen shot
     */
    void createScreenshot(std::int32_t width, std::int32_t height, const QString& fileName, const QColor& backgroundColor);

public: // Selection

    /**
     * Get the selection display mode color
     * @return Selection display mode
     */
    PointSelectionDisplayMode getSelectionDisplayMode() const;

    /**
     * Set the selection display mode
     * @param selectionDisplayMode Selection display mode
     */
    void setSelectionDisplayMode(PointSelectionDisplayMode selectionDisplayMode);

    /**
     * Get the selection outline color
     * @return Color of the selection outline
     */
    QColor getSelectionOutlineColor() const;

    /**
     * Set the selection outline color
     * @param selectionOutlineColor Selection outline color
     */
    void setSelectionOutlineColor(const QColor& selectionOutlineColor);

    /**
     * Get whether the selection outline color should be the point color or a custom color
     * @return Boolean determining whether the selection outline color should be the point color or a custom color
     */
    bool getSelectionOutlineOverrideColor() const;

    /**
     * Set whether the selection outline color should be the point color or a custom color
     * @param selectionOutlineOverrideColor Boolean determining whether the selection outline color should be the point color or a custom color
     */
    void setSelectionOutlineOverrideColor(bool selectionOutlineOverrideColor);

    /**
     * Get the selection outline scale
     * @return Scale of the selection outline
     */
    float getSelectionOutlineScale() const;

    /**
     * Set the selection outline scale
     * @param selectionOutlineScale Scale of the selection outline
     */
    void setSelectionOutlineScale(float selectionOutlineScale);

    /**
     * Get the selection outline opacity
     * @return Opacity of the selection outline
     */
    float getSelectionOutlineOpacity() const;

    /**
     * Set the selection outline opacity
     * @param selectionOutlineOpacity Opacity of the selection outline
     */
    void setSelectionOutlineOpacity(float selectionOutlineOpacity);

    /**
     * Get whether the selection outline halo is enabled or not
     * @return Boolean determining whether the selection outline halo is enabled or not
     */
    bool getSelectionOutlineHaloEnabled() const;

    /**
     * Set whether the selection outline halo is enabled or not
     * @param selectionOutlineHaloEnabled Boolean determining whether the selection outline halo is enabled or not
     */
    void setSelectionOutlineHaloEnabled(bool selectionOutlineHaloEnabled);

protected:
    void initializeGL()         Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;
    void paintGL()              Q_DECL_OVERRIDE;
    void cleanup();
    
public: // Const access to renderers

    const PointRenderer& getPointRenderer() const { 
        return _pointRenderer;
    }

    const DensityRenderer& getDensityRenderer() const {
        return _densityRenderer;
    }

public:

    /** Assign a color map image to the point and density renderers */
    void setColorMap(const QImage& colorMapImage);

signals:
    void initialized();

    /**
     * Signals that the render mode changed
     * @param renderMode Signals that the render mode has changed
     */
    void renderModeChanged(const RenderMode& renderMode);

    /**
     * Signals that the coloring mode changed
     * @param coloringMode Signals that the coloring mode has changed
     */
    void coloringModeChanged(const ColoringMode& coloringMode);

    /** Signals that the density computation has started */
    void densityComputationStarted();

    /** Signals that the density computation has ended */
    void densityComputationEnded();

public slots:
    void computeDensity();

private:
    const Matrix3f          toClipCoordinates = Matrix3f(2, 0, 0, 2, -1, -1);
    Matrix3f                toNormalisedCoordinates;
    Matrix3f                toIsotropicCoordinates;
    bool                    _isInitialized = false;
    RenderMode              _renderMode = SCATTERPLOT;
    QColor                  _backgroundColor;
    ColoringMode            _coloringMode = ColoringMode::Constant;
    PointRenderer           _pointRenderer;                     
    DensityRenderer         _densityRenderer;                   
    QSize                   _windowSize;                        /** Size of the viewerscatterplot widget */
    Bounds                  _dataBounds;                        /** Bounds of the loaded data */
    QImage                  _colorMapImage;
    PixelSelectionTool      _pixelSelectionTool;
};
