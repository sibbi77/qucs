/***************************************************************************
                                vexp.cpp
                               ----------
    begin                : Fri Mar 9 2007
    copyright            : (C) 2007 by Gunther Kraut
    email                : gn.kraut@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "vexp.h"


vExp::vExp()
{
  Description = QObject::tr("exponential voltage source");

  // normal voltage source symbol
  Arcs.append(new Arc(-12,-12, 24, 24,     0, 16*360,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-30,  0,-12,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 30,  0, 12,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 18,  5, 18, 11,QPen(QPen::red,1)));
  Lines.append(new Line( 21,  8, 15,  8,QPen(QPen::red,1)));
  Lines.append(new Line(-18,  5,-18, 11,QPen(QPen::black,1)));

  // write 'Exp' inside voltage source symbol
  Lines.append(new Line( -3,  -7, -3, -4,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -3,  -7, 5, -7,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 1,  -7, 1, -4,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 5,  -7, 5, -4,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -3,  -1, 1, 3,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 1,  -1, -3, 3,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 1,  6, -5, 6,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 1,  6, 1, 9,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -3,  6, -3, 9,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 1,  9, -3, 9,QPen(QPen::darkBlue,2)));

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  14;

  tx = x1+4;
  ty = y2+4;
  Model = "Vexp";
  Name  = "V";

  Props.append(new Property("U1", "0 V", true,
		QObject::tr("voltage before rising edge")));
  Props.append(new Property("U2", "1 V", true,
		QObject::tr("maximum voltage of the pulse")));
  Props.append(new Property("T1", "0", true,
		QObject::tr("start time of the exponentially rising edge")));
  Props.append(new Property("T2", "1 ms", true,
		QObject::tr("start of exponential decay")));
  Props.append(new Property("Tr", "1 ns", false,
		QObject::tr("rise time of the rising edge")));
  Props.append(new Property("Tf", "1 ns", false,
		QObject::tr("fall time of the falling edge")));

  rotate();  // fix historical flaw
}

vExp::~vExp()
{
}

Component* vExp::newOne()
{
  return new vExp();
}

Element* vExp::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Exponential Voltage Pulse");
  BitmapFile = (char *) "vexp";

  if(getNewOne)  return new vExp();
  return 0;
}
