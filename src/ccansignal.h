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

#ifndef CCANSIGNAL_H
#define CCANSIGNAL_H

#include "ccancomm.h"
#include "ccanfilter.h"
#include "csignaldisplay.h"
#include "multi_plot.h"

#include <QString>
#include <QModelIndex>



class CCANSignal;

/*!
 * \brief List of pointer to CANSignal
 */
typedef QList<CCANSignal*> CANSignalList;

/*!
 * \brief The CCANSignal class
 * Acts as a class for calculating the value of a CAN signal and as a data
 * provider for the Doc/View model of a TreeView.
 */
class CCANSignal : public CCANFilter
{
public:
    CCANSignal();
    ~CCANSignal();

    /*!
     * \brief Sets the name of a signal
     * \param signal_name Signalname
     */
    void setName(QString signal_name) { name = signal_name; }
    /*!
     * \brief Gets the name of a signal
     * \return Signalname
     */
    QString getName() { return name; }


    bool process_frame(const decoded_can_frame &frame, double update_rate_ms);

    /*!
     * \brief Gets the current value as QString
     * \return Current value as QString
     */
    QString getCurrentValue() { return current_value; }
    /*!
     * \brief Gets the current value as double
     * \return Current value as double
     */
    double getDoubleValue() { return double_value; }
    /*!
     * \brief Gets the unit as QString
     * \return Unit as QString
     */
    QString getUnit();
    /*!
     * \brief Sets the display entity for conversion raw->number
     * \param new_disp Pointer to display class
     */
    void setSignalDisplay(CSignalDisplay *new_disp) { disp = new_disp; }
    /*!
     * \brief Gets the display entity
     * \return Pointer to display class
     */
    CSignalDisplay *getSignalDisplay() { return disp; }

    void clear();

    // Functions needed the view model to build a tree of signals

    /*!
     * \brief Sets the parent signal
     * \param p Pointer to parent
     */
    void setParent(CCANSignal *p) { parent = p; }
    /*!
     * \brief Gets the parent signal
     * \return Pointer to parent
     */
    CCANSignal *getParent() { return parent; }
    /*!
     * \brief Gets the child items list
     * \return Pointer to the child items
     */
    CANSignalList* getChildren() { return &childItems; }
    /*!
     * \brief Gets the number of children
     * \return Number of children
     */
    int ChildCount() { return childItems.count(); }

    int row();
    int columnCount();
    QString data(int column);
    void addChild(CCANSignal *child);
    CCANSignal *getChild(int row);

    CPlot* addPlot(CGraph *g);
    void setColor(QPen *pen);
    QPen* getColor();

private:
    //! Name of the signal
    QString name;
    //! Last received value as string
    QString current_value;
    //! Last received value as double
    double double_value;

    //! Pointer to CSignalDisplay, calculate the value from raw data
    CSignalDisplay *disp;

    //! Parent signal
    CCANSignal *parent;
    //! Child signal
    CANSignalList childItems;
    //! Plot for displaying in graph
    CPlot *plot;
    //! Last update of the plot
    double last_update;
    //! Color
    QPen *color;
};

#endif // CCANSIGNAL_H
