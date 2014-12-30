/*
 *peliCAN
 *Copyright (C) 2014 Michael Hoffacker, mhoffacker3@googlemail.com
 *
 *This program is free software: you can redistribute it and/or modify
 *it under the terms of the GNU General Public License as published by
 *the Free Software Foundation, either version 3 of the License, or
 *(at your option) any later version.
 *
 *This program is distributed in the hope that it will be useful,
 *but WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *GNU General Public License for more details.
 *
 *You should have received a copy of the GNU General Public License
 *along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ccansignal.h"

/*!
 * \brief Constructor
 */
CCANSignal::CCANSignal()
    : CCANFilter()
{
    name = "Unnamed signal";
    parent = NULL;
    disp = NULL;
    plot = NULL;
    color = NULL;
}

/*!
 * \brief Destructor
 */
CCANSignal::~CCANSignal()
{
    if ( disp != NULL )
        delete disp;
}

QString CCANSignal::getUnit()
{
    if ( disp != NULL )
        return disp->getUnit();

    return QString("");
}

void CCANSignal::setColor(QPen *pen)
{
    color = pen;

    if ( color )
        plot->getCurve()->setPen(*color);
}

QPen* CCANSignal::getColor()
{
    return color;
}

/*!
 * \brief Clear tree
 * Removes all sub-items from the tree
 */
void CCANSignal::clear()
{
    while ( !childItems.empty() )
    {
        CCANSignal *item = childItems.first();
        childItems.removeFirst();

        item->clear();
        delete item;
    }
}

/*!
 * \brief Processes a CAN frame
 * Checks of the CAN frame matches the filter criteria and calculate the new
 * value.
 * \param frame Incomming CAN frame
 * \return true, if filter criteria was matched
 */
bool CCANSignal::process_frame(const decoded_can_frame &frame, double update_rate_ms)
{

    if ( !match(frame) )
        return false;

    if ( disp != NULL )
    {
        current_value = disp->getValue(frame);
        double_value = current_value.toDouble();

        if ( plot && update_rate_ms >= 0 )
        {
            double time;

            // Time in secounds
            time = frame.tv_ioctl.tv_sec + ((double)frame.tv_ioctl.tv_usec)/1000000.0;

            if ( last_update + (update_rate_ms/1000) < time )
            {
                // Plot wants the time in milli secounds
                plot->addValuePair(time*1000, double_value);
                last_update = time;
            }
        }

    }

    return true;
}

/*!
 * \brief Add a child to tree
 * \param child Pointer to child
 */
void CCANSignal::addChild(CCANSignal *child)
{
    childItems.append(child);
}

/*!
 * \brief Gets child number "row"
 * \param row Index of child signal
 * \return Pointer to child signal
 */
CCANSignal* CCANSignal::getChild(int row)
{
    return childItems.value(row);
}

/*!
 * \brief Gets index of the signal
 * \return Index of the signal
 */
int CCANSignal::row()
{
    if ( parent )
        return parent->getChildren()->indexOf(const_cast<CCANSignal*>(this));

    return 0;
}

/*!
 * \brief Returns the number of columns
 * This is needed for QT's Doc/View model. The number of colums depend on the
 * data to be displayed. See CCANSignal::data() for the values to be shown
 * \return Number of columns in the tree view
 */
int CCANSignal::columnCount()
{
    return 3;
}

/*!
 * \brief Gets value of the current column
 * \param column Index of column
 * \return String to be displayed in the tree view.
 */
QString CCANSignal::data(int column)
{
    if ( parent == NULL )
    {
        // Root element
        if ( column == 0 )
            return QString("Signal name");
        if ( column == 1 )
            return QString("Value");
        if ( column == 2 )
            return QString("Unit");
    } else if ( ChildCount() > 0 )
    {
        // Signal group
        if ( column == 0 )
            return name;
        if ( column == 1 )
            return QString("");
        if ( column == 2 )
            return QString("");
    } else {
        // Signal
        if ( column == 0 )
            return name;
        if ( column == 1 )
            return getCurrentValue();
        if ( column == 2 )
            return getUnit();
    }

    return QString();
}


CPlot* CCANSignal::addPlot(CGraph *g)
{
    plot = g->addPlot();
    return plot;
}
