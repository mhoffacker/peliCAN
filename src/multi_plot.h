#ifndef MULTI_PLOT_H
#define MULTI_PLOT_H

#include <qwt_plot.h>
#include <qwt_scale_widget.h>
#include <qwt_date_scale_draw.h>
#include <qwt_plot_curve.h>


#include <QList>
#include <QLinkedList>
#include <QGridLayout>

class CPlot;
class CGraph;
class CAxis;


/*!
 * \brief The CGraph class
 * Creates the dispplay elements on a given widget
 */
class CGraph
{
public:
    CGraph(QWidget *w);
    ~CGraph();


    CPlot* addPlot();
    void removePlot(CPlot *p);
    void removeAllPlots();

    CAxis* addAxisLeft();
    CAxis* addAxisRight();
    void removeAxis(CAxis *axis);

    QWidget* getWidget();


    void update();

    void setXScale(double min, double max, unsigned int tick_count);

    void setSliding(bool sliding, double window=0);



private:
    /*!
     * \brief Pointer to parent widget
     */
    QWidget *widget;

    QGridLayout *grid;

    QwtPlot *plot;

    QwtScaleWidget *xscale;

    /*!
     * \brief H-box layout for the left sided axis
     */
    QHBoxLayout *h;

    /*!
     * \brief H-box layout on the right sided axis
     */
    QHBoxLayout *h2;


    QwtDateScaleDraw *x_d_sd;

    /*!
     * \brief List of assigned plots
     */
    QList<CPlot*> plots;

    /*!
     * \brief current min-value in x-direction
     */
    double _min;
    /*!
     * \brief current max-value in x-direction
     */
    double _max;
    /*!
     * \brief first time to scale
     * This indicates, if it is the first time to scale. In this case, set _min and _max to new value
     */
    bool first_scale;
    /*!
     * \brief Flag for sliding mode
     */
    bool is_sliding;
    /*!
     * \brief current window in sliding mode
     */
    double sliding_window;

};

/*!
 * \brief The CPlot class
 */
class CPlot
{
public:
    CPlot(CGraph *g);
    QwtPlotCurve *getCurve();

    void addValuePair(double x, double y);
    void setAxis(CAxis *a);

    void setScale(double delta_y, double fac_y);

    double getMin() { return _min; }
    double getMax() { return _max; }

    void remove_older(double t);

    void remove_from_axis();
private:
    CGraph *graph;
    QwtPlotCurve *curve;
    CAxis *axis;

    /*!
     * \brief d_y y-axis intercept
     */
    double d_y;
    /*!
     * \brief f_y current scaling factor to scale to 0...1
     */
    double f_y;
    /*!
     * \brief _min current minimum value
     */
    double _min;
    /*!
     * \brief _max current maximum value
     */
    double _max;

    /*!
     * \brief v_x vector of x-values
     */
    QVector<double> v_x;
    /*!
     * \brief v_y vector of y-values
     */
    QVector<double> v_y;
    /*!
     * \brief v_y_scaled vector of scaled y-values. (range 0...1)
     */
    QVector<double> v_y_scaled;

    /*!
     * \brief min_max_last_it Position of the latest inserted element
     */
    QLinkedList<double>::Iterator min_max_last_it;
    /*!
     * \brief min_max_it_list List of the iterators of the min/max list in the same order as v_y
     */
    QList< QLinkedList<double>::Iterator > min_max_it_list;
    /*!
     * \brief min_max_list Sorted list of the min/max values
     */
    QLinkedList<double> min_max_list;
};

/*!
 * \brief The CAxis class
 */
class CAxis
{
public:
    CAxis(CGraph *g, bool right=false);


    void appendPlot(CPlot *p);
    void removePlot(CPlot *p);
    void removeAllPlots();

    QwtScaleWidget *getScale();

    void rescalePlots(double y);
    void calcNewRescale();

private:

    void setMinMax(double min, double max, unsigned int tick_count);
    QwtScaleWidget *yscale;

    /*!
     * \brief plots List of all attached plots
     */
    QList<CPlot*> plots;

    /*!
     * \brief _min current min value on scale
     */
    double _min;
    /*!
     * \brief _max current max value on scale
     */
    double _max;

    bool first_sample;  // First sample for rescaling axis
};

#endif // MULTI_PLOT_H
