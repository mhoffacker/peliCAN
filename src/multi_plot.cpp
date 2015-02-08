#include "multi_plot.h"

/*!
 * \brief CAxis::CAxis Axis constructor
 * \param g Pointer to the CGraph
 * \param right flag, if the axis should be placed on the right side
 */
CAxis::CAxis(CGraph *g, bool right)
{
    if ( right )
        yscale = new QwtScaleWidget(QwtScaleDraw::RightScale, g->getWidget());
    else
        yscale = new QwtScaleWidget(QwtScaleDraw::LeftScale, g->getWidget());

    first_sample = true;

    setMinMax(-1, 1, 10);
}

/*!
 * \brief CAxis::appendPlot append a plot
 * Add a plot and rescale the axis
 * \param p Pointer to a CPlot plot
 */
void CAxis::appendPlot(CPlot *p)
{
    plots.append(p);

    calcNewRescale();
}

/*!
 * \brief CAxis::removePlot Removes a plot
 * Remove a plot and rescale the axis
 * \param p
 */
void CAxis::removePlot(CPlot *p)
{
    int index = plots.indexOf(p);

    if ( index == -1 )
        return;

    plots.removeAt(index);

    calcNewRescale();
}

/*!
 * \brief CAxis::removeAllPlots Removes all plots
 */
void CAxis::removeAllPlots()
{
    while ( !plots.isEmpty() )
        removePlot(plots.first());
}

/*!
 * \brief CAxis::getScale gets the scale
 * \return Return a pointer to the internal QwtScaleWidget
 */
QwtScaleWidget* CAxis::getScale()
{
    return yscale;
}

/*!
 * \brief CAxis::rescalePlots Rescales all plots attached to this axis
 * Rescale all plots attached to this axis. The parameter y is checked
 * if this is a new maximum value bigger than the current max scale.
 *
 * If a plot has a new maximum value it has to call CAxis::calcNewRescale
 * for a complete new calculation of the scale value.
 *
 * This function is faster but works only for new max. values and is called
 * automatically when a new data pair is inserted into a plot
 *
 * \param y new y-value
 */
void CAxis::rescalePlots(double y)
{
    bool rescale;
    double f;

    rescale = false;
    if ( first_sample || y > _max || y < _min )
    {
        if ( y > _max || first_sample )
        {
            _max = y;
            rescale = true;
        }

        if ( y < _min || first_sample )
        {
            _min = y;
            rescale = true;
        }

        if ( rescale )
        {
            if ( _max != _min)
                f = 1.0 / (_max - _min);
            else
                f = 1.0;

            foreach ( CPlot *p, plots)
            {
                p->setScale(-_min, f);
            }
        }

        setMinMax(_min, _max, 10);
    }
    first_sample = false;
}

/*!
 * \brief CAxis::setMinMax Sets the min/max of the scale
 * \param min new min value
 * \param max new max value
 * \param tick_count number of ticks in between
 */
void CAxis::setMinMax(double min, double max, unsigned int tick_count)
{
    QList<double> t;
    QwtScaleDiv x_sd;

    double delta = (max-min)/((double)tick_count-1.0);
    double ins_val = min;

    for ( unsigned int i=0; i<tick_count; i++ )
    {
        t.append(ins_val);
        ins_val += delta;
    }
    x_sd.setTicks(QwtScaleDiv::MajorTick, t);
    x_sd.setInterval(min, max);
    yscale->setScaleDiv(x_sd);

}

/*!
 * \brief CAxis::calcNewRescale Calculates new rescale value
 * Finds the min/max value of all attached plots.
 * This function is called if a plot finds, that
 * its min or max value has been removed due
 * to the sliding window.
 */
void CAxis::calcNewRescale()
{
    bool first = true;

    foreach (CPlot *p, plots) {
        if ( first )
        {
            _min = p->getMin();
            _max = p->getMax();
            first = false;
        }

        if ( p->getMax() > _max )
            _max = p->getMax();

        if ( p->getMin() < _min )
            _min = p->getMin();
    }

    setMinMax(_min, _max, 10);
}

/*!
 * \brief CPlot::CPlot constructor
 * \param g Pointer to CGraph the plot is part of
 */
CPlot::CPlot(CGraph *g)
{
    graph = g;
    curve = new QwtPlotCurve();
    axis = NULL;

    d_y = 0;    // Set shift in y-direction to 0
    f_y = 1;    // Set scale in y-direction to 1

    _min = 0;
}

/*!
 * \brief CPlot::setAxis Sets the axis for this plot
 * \param a Pointer to CAxis the plot shall be attached to
 */
void CPlot::setAxis(CAxis *a)
{
    if ( a == NULL )
        return;

    if ( axis != NULL )
        remove_from_axis();

    axis = a;
    axis->appendPlot(this);
}

/*!
 * \brief CPlot::getCurve Gets curve
 * \return Returns the pointer to the internal QwtPlotCurve
 */
QwtPlotCurve* CPlot::getCurve()
{
    return curve;
}

/*!
 * \brief CPlot::addValuePair Adds a value pair to a plot
 * Insert a new value pair. Check if this is a new min/max value
 *
 * Calls CAxis::rescalePlots() to rescale y-axis and
 * CGraph::setXScale()
 * \param x x-value of data pair
 * \param y y-value of data pair
 */
void CPlot::addValuePair(double x, double y)
{
    QLinkedList<double>::Iterator to_insert;

    // Save the min value for shifting the plot up to set min/max value to initial sample
    if ( v_y.size() == 0 )
    {
        _min = y;
        _max = y;

        // First element, add it and save the position for the next insertion
        min_max_last_it = min_max_list.insert(min_max_list.end(), y);

        // Save iterator in the same order as v_y
        min_max_it_list.append(min_max_last_it);

    } else {
        if ( y < _min )
            _min = y;

        if ( y > _max )
            _max = y;

        // Start for the insertion at the last element
        to_insert = min_max_last_it;

        // Find the first element that is smalle than the new one
        // It will give the wrong position, since we insert before
        // this element. The next while loop handles this
        while ( y < *to_insert && to_insert != min_max_list.begin() )
            to_insert--;

        // Finds the first element that is bigger than the new one
        // Insert before this element
        while ( y >= *to_insert && to_insert != min_max_list.end() )
            to_insert++;

        // Add it and save the position for the next insertion
        min_max_last_it = min_max_list.insert(to_insert, y);

        // Save iterator in the same order as v_y
        min_max_it_list.append(min_max_last_it);
    }

    v_x.append(x);
    v_y.append(y);

    v_y_scaled.append((y+d_y) * f_y);   // Scale to 0....1

    curve->setSamples(v_x, v_y_scaled);

    if ( axis != NULL )
    {
        axis->rescalePlots(y);

        graph->setXScale(v_x.first(), v_x.last(), 10);
    }
}

/*!
 * \brief CPlot::setScale Scales to data
 * Scales all y-values in the range of 0...1
 * \param delta_y y-axis intercept to shift upwards
 * \param fac_y scale factor to have the biggest y-value scaled to 1
 */
void CPlot::setScale(double delta_y, double fac_y)
{
    d_y = delta_y;
    f_y = fac_y;

    QVector<double>::Iterator it = v_y_scaled.begin();

    foreach ( double y, v_y )
    {
        // Replace every scaled value with the new one
        *it = (y+d_y) * f_y;
        it++;
    }
    curve->setSamples(v_x, v_y_scaled);
}

/*!
 * \brief CPlot::remove_older Removes data pairs with x < t
 * Called in sliding mode by CGraph::setXScale()
 * Removes all elements from Vectors of x- y- and scaled_y-vector
 *
 * If the smalles or bigged y-element is removed it will find the
 * new min/max elements and call CAxis::calcNewRescale() and
 * CAxis::rescalePlots() to rescale all plots attached to this
 * axis
 * \param t
 */
void CPlot::remove_older(double t)
{
    bool calc_new_min_max = false;

    if ( v_x.isEmpty() )
        return;

    while ( v_x.first() < t && !v_x.isEmpty())
    {
        if ( !calc_new_min_max )
        {
            if ( v_y.first() <= _min )
                calc_new_min_max = true;

            if ( v_y.first() >= _max )
                calc_new_min_max = true;
        }

        v_y.erase(v_y.begin());
        v_y_scaled.erase(v_y_scaled.begin());
        v_x.erase(v_x.begin());

        // Removes the element from the min/max list
        QLinkedList<double>::Iterator to_remove;
        to_remove = *min_max_it_list.begin();
        min_max_list.erase(to_remove);
        min_max_it_list.erase(min_max_it_list.begin());

    }

    if ( v_x.isEmpty() )
        return;

    if ( calc_new_min_max )
    {
        /*
        _min = v_y.first();
        _max = v_y.first();

        foreach(double k, v_y)
        {
            if ( k < _min )
                _min = k;

            if ( k > _max )
                _max = k;
        }
        */
        _min = min_max_list.first();
        _max = min_max_list.last();

        if ( axis != NULL )
        {
            axis->calcNewRescale();
            axis->rescalePlots(_max);
        }

    }
}

/*!
 * \brief CPlot::remove_from_axis Remove from the axis
 */
void CPlot::remove_from_axis()
{
    if ( axis != NULL )
        axis->removePlot(this);

    axis = NULL;
}

CGraph::~CGraph()
{
}
/*!
 * \brief CGraph::CGraph
 * Creates the view elements for the multi-axis graph
 * \param w Pointer to parent widget
 */
CGraph::CGraph(QWidget *w)
{
    widget = w;

    grid = new QGridLayout(w);
    grid->setSpacing(6);
    grid->setContentsMargins(11, 11, 11, 11);


    plot = new QwtPlot(w);
    plot->enableAxis(QwtPlot::yLeft, false);
    plot->enableAxis(QwtPlot::yRight, false);
    plot->enableAxis(QwtPlot::xBottom, false);
    plot->enableAxis(QwtPlot::xTop, false);


    xscale = new QwtScaleWidget(QwtScaleDraw::BottomScale, w);

    x_d_sd = new QwtDateScaleDraw();

    x_d_sd->setDateFormat(QwtDate::Millisecond, "hh:mm:ss:zzz");
    x_d_sd->setLabelAlignment(Qt::AlignTrailing);
    x_d_sd->setLabelRotation(90);


    xscale->setScaleDraw(x_d_sd);


    h = new QHBoxLayout();
    h2 = new QHBoxLayout();

    grid->addLayout(h,      0, 0, 1, 1);
    grid->addWidget(plot,   0, 1, 1, 1);
    grid->addWidget(xscale, 1, 1, 1, 1);
    grid->addLayout(h2,     0, 2, 1, 1);

    first_scale = true;
}

/*!
 * \brief CGraph::setSliding Sets the sliding mode
 * \param sliding enable/disable sliding
 * \param window difference between min...max. Ignored if sliding=false
 */
void CGraph::setSliding(bool sliding, double window)
{
    is_sliding = sliding;
    sliding_window = window;
}


/*!
 * \brief CGraph::setXScale Sets the range of the x-scale
 * Sets the new range of the x-scale.
 *
 * If the view is in sliding mode, all there is a iteration thru all
 * plots to call CPlot::remove_older() to remove all values lower than
 * the new minimum value.
 *
 * \param min Minimum to show
 * \param max Maximum to show
 * \param tick_count Ticks to show
 */
void CGraph::setXScale(double min, double max, unsigned int tick_count)
{
    QList<double> t;
    QwtScaleDiv x_sd;

    bool rescale = false;

    if ( first_scale || min < _min )
    {
        _min = min;
        rescale = true;
    }

    if ( first_scale || max > _max )
    {
        _max = max;
        rescale = true;
    }

    if ( !rescale )
        return;

    if ( is_sliding )
    {
        min = max - sliding_window;
        foreach (CPlot *p, plots) {
            p->remove_older(min);
        }
    }

    double delta = (max-min)/((double)tick_count-1.0);
    double ins_val = min;

    for ( unsigned int i=0; i<tick_count; i++ )
    {
        t.append(ins_val);
        ins_val += delta;
    }
    x_sd.setTicks(QwtScaleDiv::MajorTick, t);
    x_sd.setInterval(min, max);
    xscale->setScaleDiv(x_sd);

    plot->setAxisAutoScale(QwtPlot::xBottom, false);
    plot->setAxisScaleDiv(QwtPlot::xBottom, x_sd);

    x_sd.setInterval(0, 1);
    plot->setAxisAutoScale(QwtPlot::yLeft, false);
    plot->setAxisScaleDiv(QwtPlot::yLeft, x_sd);

    first_scale = false;
}

/*!
 * \brief CGraph::getWidget returns the parent widget
 * \return Pointer to parent widget
 */
QWidget* CGraph::getWidget()
{
    return widget;
}

/*!
 * \brief CGraph::update Updates the graph
 */
void CGraph::update()
{
    plot->replot();
}

/*!
 * \brief CGraph::addPlot Adds a new plot
 * \return Pointer to new CPlot
 */
CPlot* CGraph::addPlot()
{
    CPlot *p = new CPlot(this);

    plots.append(p);

    p->getCurve()->attach(this->plot);

    // Set initial scaling factor
    double f;
    if ( _max != _min)
        f = 1.0 / (_max - _min);
    else
        f = 1.0;

    p->setScale(p->getMin(), f);

    return p;
}

/*!
 * \brief CGraph::removePlot Removes a plot
 * Removes a plot from internal list and the axis
 * \param p Pointer to plot
 */
void CGraph::removePlot(CPlot *p)
{
    int index = plots.indexOf(p);

    if ( index == -1 )
        return;

    plots.removeAt(index);

    p->remove_from_axis();

    p->getCurve()->detach();
}

void CGraph::removeAllPlots()
{
    while ( !plots.empty() )
        removePlot(plots.first());
}

/*!
 * \brief CGraph::addAxisLeft Add an axis on the left
 * \return Pointer to new CAxis
 */
CAxis* CGraph::addAxisLeft()
{
    CAxis *a = new CAxis(this, false);
    h->addWidget(a->getScale());
    return a;
}

/*!
 * \brief CGraph::addAxisRight Add an axis on the right
 * \return Pointer to new CAxis
 */
CAxis* CGraph::addAxisRight()
{
    CAxis *a = new CAxis(this, true);
    h2->addWidget(a->getScale());
    return a;
}

/*!
 * \brief CGraph::removeAxis Removes an axis
 * Removes an axis and all attached plots
 * \param axis
 */
void CGraph::removeAxis(CAxis *axis)
{
    axis->removeAllPlots();

    h->removeWidget(axis->getScale());
    h2->removeWidget(axis->getScale());

    axis->getScale()->hide();
}
