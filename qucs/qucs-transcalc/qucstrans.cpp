/***************************************************************************
                          qucstrans.cpp  -  description
                             -------------------
    begin                : Sun Feb 27 2005
    copyright            : (C) 2005, 2006 by Stefan Jahn
    email                : stefan@lkcc.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <qlayout.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qpushbutton.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qmessagebox.h>
#include <qtoolbutton.h>
#include <qimage.h>
#include <qfiledialog.h>
#include <qmenubar.h>
#include <qaction.h>
#include <qpopupmenu.h>
#include <qhgroupbox.h>
#include <qvgroupbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qimage.h>
#include <qiconset.h>
#include <qscrollview.h>
#include <qtooltip.h>
#include <qradiobutton.h>
#include <qstatusbar.h>
#include <qdir.h>
#include <qbuttongroup.h>
#include <qwidgetstack.h>
#include <qregexp.h>
#include <qvalidator.h>
#include <qclipboard.h>
#include <qapplication.h>

#include "qucstrans.h"
#include "helpdialog.h"
#include "optionsdialog.h"
#include "transline.h"
#include "units.h"
#include "microstrip.h"
#include "coplanar.h"
#include "coax.h"
#include "rectwaveguide.h"
#include "c_microstrip.h"

QDir QucsWorkDir;

// Defines maximum number of entries in each property category.
static const int TransMaxBox[MAX_TRANS_BOXES] = { 9, 1, 4, 3 };

// Helper #defines for the below transmission line types.
#define TRANS_RADIOS  { -1, -1, -1, -1 }
#define TRANS_QOBJS   NULL, NULL, NULL, NULL, NULL
#define TRANS_END     { NULL, 0, NULL, { NULL }, 0, TRANS_QOBJS }
#define TRANS_RESULT  { NULL, NULL, NULL }
#define TRANS_RESULTS { TRANS_RESULT }

// Defines the available transmission line types.
struct TransType TransLineTypes[] = {
  { ModeMicrostrip, "Microstrip", "microstrip.png", NULL,
    { { {
      { "Er",    2.94,  NULL, TRANS_NONES,   0, TRANS_QOBJS },
      { "Mur",   1,     NULL, TRANS_NONES,   0, TRANS_QOBJS },
      { "H",     10,    NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      { "H_t",   1e20,  NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      { "T",     0.1,   NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      { "Cond",  4.1e7, NULL, TRANS_NONES,   0, TRANS_QOBJS },
      { "Tand",  0,     NULL, TRANS_NONES,   0, TRANS_QOBJS },
      { "Rough", 0,     NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "Freq",  1, NULL, TRANS_FREQS, 0, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "W",     10,  NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      { "L",     100, NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "Z0",    50, NULL, TRANS_OHMS,   0, TRANS_QOBJS },
      { "Ang_l", 90, NULL, TRANS_ANGLES, 0, TRANS_QOBJS },
      TRANS_END
    } } },
    4, TRANS_RESULTS, TRANS_RADIOS
  },
  { ModeCoplanar, "Coplanar", "cpw.png", NULL,
    { { {
      { "Er",    2.94,  NULL, TRANS_NONES,   0, TRANS_QOBJS },
      { "H",     10,    NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      { "T",     0.1,   NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      { "Cond",  4.1e7, NULL, TRANS_NONES,   0, TRANS_QOBJS },
      { "Tand",  0,     NULL, TRANS_NONES,   0, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "Freq",  1, NULL, TRANS_FREQS, 0, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "W",     10,  NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      { "S",      5,  NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      { "L",     100, NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "Z0",    50, NULL, TRANS_OHMS,   0, TRANS_QOBJS },
      { "Ang_l", 90, NULL, TRANS_ANGLES, 0, TRANS_QOBJS },
      TRANS_END
    } } },
    4, TRANS_RESULTS, { 1, 0, -1, -1 }
  },
  { ModeGroundedCoplanar, "GroundedCoplanar", "cpw_back.png", NULL,
    { { {
      { "Er",    2.94,  NULL, TRANS_NONES,   0, TRANS_QOBJS },
      { "H",     10,    NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      { "T",     0.1,   NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      { "Cond",  4.1e7, NULL, TRANS_NONES,   0, TRANS_QOBJS },
      { "Tand",  0,     NULL, TRANS_NONES,   0, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "Freq",  1, NULL, TRANS_FREQS, 0, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "W",     10,  NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      { "S",      5,  NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      { "L",     100, NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "Z0",    50, NULL, TRANS_OHMS,   0, TRANS_QOBJS },
      { "Ang_l", 90, NULL, TRANS_ANGLES, 0, TRANS_QOBJS },
      TRANS_END
    } } },
    4, TRANS_RESULTS, { 1, 0, -1, -1 }
  },
  { ModeRectangular, "Rectangular", "rectwaveguide.png", NULL,
    { { {
      { "Er",    1,     NULL, TRANS_NONES, 0, TRANS_QOBJS },
      { "Mur",   1,     NULL, TRANS_NONES, 0, TRANS_QOBJS },
      { "Cond",  4.1e7, NULL, TRANS_NONES, 0, TRANS_QOBJS },
      { "Tand",  0,     NULL, TRANS_NONES, 0, TRANS_QOBJS },
      { "TanM",  0,     NULL, TRANS_NONES, 0, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "Freq",  10, NULL, TRANS_FREQS, 0, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "a",     1000, NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      { "b",     500,  NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      { "L",     4000, NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "Z0",    50, NULL, TRANS_OHMS,   0, TRANS_QOBJS },
      { "Ang_l", 90, NULL, TRANS_ANGLES, 0, TRANS_QOBJS },
      TRANS_END
    } } },
    3, TRANS_RESULTS, { 0, 1, -1, -1 }
  },
  { ModeCoaxial, "Coaxial", "coax.png", NULL,
    { { {
      { "Er",    2.1,   NULL, TRANS_NONES, 0, TRANS_QOBJS },
      { "Mur",   1,     NULL, TRANS_NONES, 0, TRANS_QOBJS },
      { "Tand",  0.002, NULL, TRANS_NONES, 0, TRANS_QOBJS },
      { "Sigma", 4.1e7, NULL, TRANS_NONES, 0, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "Freq",  10, NULL, TRANS_FREQS, 0, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "din",   40,   NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      { "dout",  134,  NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      { "L",     1000, NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "Z0",    50, NULL, TRANS_OHMS,   0, TRANS_QOBJS },
      { "Ang_l", 90, NULL, TRANS_ANGLES, 0, TRANS_QOBJS },
      TRANS_END
    } } },
    2, TRANS_RESULTS, { 0, 1, -1, -1 }
  },
  { ModeCoupledMicrostrip, "CoupledMicrostrip", "c_microstrip.png", NULL,
    { { {
      { "Er",    4.3,   NULL, TRANS_NONES,   0, TRANS_QOBJS },
      { "Mur",   1,     NULL, TRANS_NONES,   0, TRANS_QOBJS },
      { "H",     8.27,  NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      { "H_t",   1e20,  NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      { "T",     0.68,  NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      { "Cond",  4.1e7, NULL, TRANS_NONES,   0, TRANS_QOBJS },
      { "Tand",  0,     NULL, TRANS_NONES,   0, TRANS_QOBJS },
      { "Rough", 0,     NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "Freq",  10, NULL, TRANS_FREQS, 0, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "W",     8.66, NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      { "S",     5.31, NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      { "L",     1000, NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "Z0e",   50, NULL, TRANS_OHMS,   0, TRANS_QOBJS },
      { "Z0o",   50, NULL, TRANS_OHMS,   0, TRANS_QOBJS },
      { "Ang_l", 90, NULL, TRANS_ANGLES, 0, TRANS_QOBJS },
      TRANS_END
    } } },
    7, TRANS_RESULTS, TRANS_RADIOS
  },
  { ModeNone, NULL, NULL, NULL, { { { TRANS_END } } }, 0,
    TRANS_RESULTS, TRANS_RADIOS }
};

struct TransUnit TransUnits[] = {
  { "Frequency",  TRANS_FREQS },
  { "Length",     TRANS_LENGTHS },
  { "Resistance", TRANS_OHMS },
  { "Angle",      TRANS_ANGLES },
};

/* Constructor setups the GUI. */
QucsTranscalc::QucsTranscalc() {
  // set application icon
  setIcon (QPixmap(QucsSettings.BitmapDir + "big.qucs.xpm"));
  setCaption("Qucs Transcalc " PACKAGE_VERSION);

  QMenuBar * menuBar = new QMenuBar (this);

  // create file menu
  QPopupMenu * fileMenu = new QPopupMenu ();
  QAction * fileLoad =
    new QAction (tr("Load"), tr("&Load"), CTRL+Key_L, this);
  fileLoad->addTo (fileMenu);
  connect(fileLoad, SIGNAL(activated()), SLOT(slotFileLoad()));
  QAction * fileSave =
    new QAction (tr("Save"), tr("&Save"), CTRL+Key_S, this);
  fileSave->addTo (fileMenu);
  connect(fileSave, SIGNAL(activated()), SLOT(slotFileSave()));
  fileMenu->insertSeparator ();
  QAction * fileOption =
    new QAction (tr("Options"), tr("&Options"), CTRL+Key_O, this);
  fileOption->addTo (fileMenu);
  connect(fileOption, SIGNAL(activated()), SLOT(slotOptions()));
  fileMenu->insertSeparator ();
  QAction * fileQuit =
    new QAction (tr("Quit"), tr("&Quit"), CTRL+Key_Q, this);
  fileQuit->addTo (fileMenu);
  connect(fileQuit, SIGNAL(activated()), SLOT(slotQuit()));

  // create execute menu
  QPopupMenu * execMenu = new QPopupMenu ();
  QAction * execCopy =
    new QAction (tr("Copy to Clipboard"),
		 tr("&Copy to Clipboard"), Key_F2, this);
  execCopy->addTo (execMenu);
  connect(execCopy, SIGNAL(activated()), SLOT(slotCopyToClipBoard()));
  QAction * execAnalyze =
    new QAction (tr("Analyze"), tr("&Analyze"), Key_F3, this);
  execAnalyze->addTo (execMenu);
  connect(execAnalyze, SIGNAL(activated()), SLOT(slotAnalyze()));
  QAction * execSynthesize =
    new QAction (tr("Synthesize"), tr("&Synthesize"), Key_F4, this);
  execSynthesize->addTo (execMenu);
  connect(execSynthesize, SIGNAL(activated()), SLOT(slotSynthesize()));

  // create help menu
  QPopupMenu * helpMenu = new QPopupMenu ();
  QAction * helpHelp =
    new QAction (tr("Help"), tr("&Help"), Key_F1, this);
  helpHelp->addTo (helpMenu);
  connect(helpHelp, SIGNAL(activated()), SLOT(slotHelpIntro()));
  QAction * helpAbout =
    new QAction (tr("About"), tr("About"), 0, helpMenu);
  helpAbout->addTo (helpMenu);
  connect(helpAbout, SIGNAL(activated()), SLOT(slotAbout()));

  // setup menu bar
  menuBar->insertItem (tr("&File"), fileMenu);
  menuBar->insertItem (tr("&Execute"), execMenu);
  menuBar->insertSeparator ();
  menuBar->insertItem (tr("&Help"), helpMenu);

  // main box
  QVBoxLayout * v = new QVBoxLayout (this);
  v->setSpacing (0);
  v->setMargin (0);

  // reserve space for menubar
  QWidget * Space = new QWidget (this);
  Space->setFixedSize(5, menuBar->height() + 2);
  v->addWidget (Space);

  // main layout
  QHBox * h = new QHBox (this);
  h->setSpacing (5);
  v->addWidget (h);

  // left margin
  QWidget * sl = new QWidget (h);
  sl->setFixedWidth (2);

  // transmission line type choice
  QVGroupBox * lineGroup = new QVGroupBox (tr("Transmission Line Type"), h);
  tranType = new QComboBox (lineGroup);
  tranType->insertItem (tr("Microstrip Line"));
  tranType->insertItem (tr("Coplanar Waveguide"));
  tranType->insertItem (tr("Grounded Coplanar"));
  tranType->insertItem (tr("Rectangular Waveguide"));
  tranType->insertItem (tr("Coaxial Line"));
  tranType->insertItem (tr("Coupled Microstrip"));
  connect(tranType, SIGNAL(activated(int)), SLOT(slotSelectType(int)));
  // setup transmission line picture
  pix = new QLabel (lineGroup);
  pix->setPixmap (QPixmap (QImage (QucsSettings.BitmapDir +
				   "microstrip.png")));

  QVBox * vm = new QVBox (h);
  vm->setSpacing (3);
  QVBox * vr = new QVBox (h);
  vr->setSpacing (3);

  // right margin
  QWidget * sr = new QWidget (h);
  sr->setFixedWidth (2);

  // init additional translations
  setupTranslations ();

  // set current mode
  mode = ModeMicrostrip;

  // substrate parameter box
  QHGroupBox * substrate = new QHGroupBox (tr("Substrate Parameters"), vm);
  createPropItems (substrate, TRANS_SUBSTRATE);

  // component parameter box
  QHGroupBox * component = new QHGroupBox (tr("Component Parameters"), vm);
  createPropItems (component, TRANS_COMPONENT);

  // physical parameter box
  QHGroupBox * physical = new QHGroupBox (tr("Physical Parameters"), vr);
  createPropItems (physical, TRANS_PHYSICAL);

  // analyze/synthesize buttons
  QHBox * h2 = new QHBox (vr);
  QPushButton * analyze = new QPushButton (tr("Analyze"), h2);
  QToolTip::add (analyze, tr("Derive Electrical Parameters"));
  connect(analyze, SIGNAL(clicked()), SLOT(slotAnalyze()));
  QPushButton * synthesize = new QPushButton (tr("Synthesize"), h2);
  QToolTip::add (synthesize, tr("Compute Physical Parameters"));
  connect(synthesize, SIGNAL(clicked()), SLOT(slotSynthesize()));

  // electrical parameter box
  QHGroupBox * electrical = new QHGroupBox (tr("Electrical Parameters"), vr);
  createPropItems (electrical, TRANS_ELECTRICAL);

  calculated = new QHGroupBox (tr("Calculated Results"), vr);

  // status line
  statBar = new QStatusBar (this);
  QLabel * statText = new QLabel ("Ready.", statBar);
  statBar->message (tr("Ready."));
  statBar->setFixedHeight (statText->height ());
  v->addWidget (statBar);
  delete statText;

  // setup calculated result bix
  createResultItems (calculated);
  updateSelection ();

  // intantiate transmission lines
  TransLineTypes[0].line = new microstrip ();
  TransLineTypes[0].line->setApplication (this);
  TransLineTypes[1].line = new coplanar ();
  TransLineTypes[1].line->setApplication (this);
  TransLineTypes[2].line = new groundedCoplanar ();
  TransLineTypes[2].line->setApplication (this);
  TransLineTypes[3].line = new rectwaveguide ();
  TransLineTypes[3].line->setApplication (this);
  TransLineTypes[4].line = new coax ();
  TransLineTypes[4].line->setApplication (this);
  TransLineTypes[5].line = new c_microstrip ();
  TransLineTypes[5].line->setApplication (this);
}

/* Destructor destroys the application. */
QucsTranscalc::~QucsTranscalc()
{
  for (int i = 0; i < MAX_TRANS_TYPES; i++) {
    if (TransLineTypes[i].line)
      delete TransLineTypes[i].line;
  }
}

/* The function puts translatable text into the transmission line
   structures. */
void QucsTranscalc::setupTranslations () {
  // calculated results
  int i = 0;
  TransLineTypes[i].result[0].name = new QString(tr("ErEff"));
  TransLineTypes[i].result[1].name = new QString(tr("Conductor Losses"));
  TransLineTypes[i].result[2].name = new QString(tr("Dielectric Losses"));
  TransLineTypes[i].result[3].name = new QString(tr("Skin Depth"));

  i++;
  TransLineTypes[i].result[0].name = new QString(tr("ErEff"));
  TransLineTypes[i].result[1].name = new QString(tr("Conductor Losses"));
  TransLineTypes[i].result[2].name = new QString(tr("Dielectric Losses"));
  TransLineTypes[i].result[3].name = new QString(tr("Skin Depth"));

  i++;
  TransLineTypes[i].result[0].name = new QString(tr("ErEff"));
  TransLineTypes[i].result[1].name = new QString(tr("Conductor Losses"));
  TransLineTypes[i].result[2].name = new QString(tr("Dielectric Losses"));
  TransLineTypes[i].result[3].name = new QString(tr("Skin Depth"));

  i++;
  TransLineTypes[i].result[0].name = new QString(tr("ErEff"));
  TransLineTypes[i].result[1].name = new QString(tr("Conductor Losses"));
  TransLineTypes[i].result[2].name = new QString(tr("Dielectric Losses"));
  TransLineTypes[i].result[3].name = new QString(tr("TE-Modes"));
  TransLineTypes[i].result[4].name = new QString(tr("TM-Modes"));

  i++;
  TransLineTypes[i].result[0].name = new QString(tr("Conductor Losses"));
  TransLineTypes[i].result[1].name = new QString(tr("Dielectric Losses"));
  TransLineTypes[i].result[2].name = new QString(tr("TE-Modes"));
  TransLineTypes[i].result[3].name = new QString(tr("TM-Modes"));

  i++;
  TransLineTypes[i].result[0].name = new QString(tr("ErEff Even"));
  TransLineTypes[i].result[1].name = new QString(tr("ErEff Odd"));
  TransLineTypes[i].result[2].name = new QString(tr("Conductor Losses Even"));
  TransLineTypes[i].result[3].name = new QString(tr("Conductor Losses Odd"));
  TransLineTypes[i].result[4].name = new QString(tr("Dielectric Losses Even"));
  TransLineTypes[i].result[5].name = new QString(tr("Dielectric Losses Odd"));
  TransLineTypes[i].result[6].name = new QString(tr("Skin Depth"));

  // extra tool tips
  struct TransType * t = TransLineTypes;
  t->array[0].item[0].tip = new QString(tr("Relative Permittivity"));
  t->array[0].item[1].tip = new QString(tr("Relative Permeability"));
  t->array[0].item[2].tip = new QString(tr("Height of Substrate"));
  t->array[0].item[3].tip = new QString(tr("Height of Box Top"));
  t->array[0].item[4].tip = new QString(tr("Strip Thickness"));
  t->array[0].item[5].tip = new QString(tr("Strip Conductivity"));
  t->array[0].item[6].tip = new QString(tr("Dielectric Loss Tangent"));
  t->array[0].item[7].tip = new QString(tr("Conductor Roughness"));
  t->array[1].item[0].tip = new QString(tr("Frequency"));
  t->array[2].item[0].tip = new QString(tr("Line Width"));
  t->array[2].item[1].tip = new QString(tr("Line Length"));
  t->array[3].item[0].tip = new QString(tr("Characteristic Impedance"));
  t->array[3].item[1].tip = new QString(tr("Electrical Length"));

  t++;
  t->array[0].item[0].tip = new QString(tr("Relative Permittivity"));
  t->array[0].item[1].tip = new QString(tr("Height of Substrate"));
  t->array[0].item[2].tip = new QString(tr("Strip Thickness"));
  t->array[0].item[3].tip = new QString(tr("Strip Conductivity"));
  t->array[0].item[4].tip = new QString(tr("Dielectric Loss Tangent"));
  t->array[1].item[0].tip = new QString(tr("Frequency"));
  t->array[2].item[0].tip = new QString(tr("Line Width"));
  t->array[2].item[1].tip = new QString(tr("Gap Width"));
  t->array[2].item[2].tip = new QString(tr("Line Length"));
  t->array[3].item[0].tip = new QString(tr("Characteristic Impedance"));
  t->array[3].item[1].tip = new QString(tr("Electrical Length"));

  t++;
  t->array[0].item[0].tip = new QString(tr("Relative Permittivity"));
  t->array[0].item[1].tip = new QString(tr("Height of Substrate"));
  t->array[0].item[2].tip = new QString(tr("Strip Thickness"));
  t->array[0].item[3].tip = new QString(tr("Strip Conductivity"));
  t->array[0].item[4].tip = new QString(tr("Dielectric Loss Tangent"));
  t->array[1].item[0].tip = new QString(tr("Frequency"));
  t->array[2].item[0].tip = new QString(tr("Line Width"));
  t->array[2].item[1].tip = new QString(tr("Gap Width"));
  t->array[2].item[2].tip = new QString(tr("Line Length"));
  t->array[3].item[0].tip = new QString(tr("Characteristic Impedance"));
  t->array[3].item[1].tip = new QString(tr("Electrical Length"));

  t++;
  t->array[0].item[0].tip = new QString(tr("Relative Permittivity"));
  t->array[0].item[1].tip = new QString(tr("Relative Permeability"));
  t->array[0].item[2].tip = new QString(tr("Conductivity of Metal"));
  t->array[0].item[3].tip = new QString(tr("Dielectric Loss Tangent"));
  t->array[0].item[4].tip = new QString(tr("Magnetic Loss Tangent"));
  t->array[1].item[0].tip = new QString(tr("Frequency"));
  t->array[2].item[0].tip = new QString(tr("Width of Waveguide"));
  t->array[2].item[1].tip = new QString(tr("Height of Waveguide"));
  t->array[2].item[2].tip = new QString(tr("Waveguide Length"));
  t->array[3].item[0].tip = new QString(tr("Characteristic Impedance"));
  t->array[3].item[1].tip = new QString(tr("Electrical Length"));

  t++;
  t->array[0].item[0].tip = new QString(tr("Relative Permittivity"));
  t->array[0].item[1].tip = new QString(tr("Relative Permeability"));
  t->array[0].item[2].tip = new QString(tr("Dielectric Loss Tangent"));
  t->array[0].item[3].tip = new QString(tr("Conductivity of Metal"));
  t->array[1].item[0].tip = new QString(tr("Frequency"));
  t->array[2].item[0].tip = new QString(tr("Inner Diameter"));
  t->array[2].item[1].tip = new QString(tr("Outer Diameter"));
  t->array[2].item[2].tip = new QString(tr("Length"));
  t->array[3].item[0].tip = new QString(tr("Characteristic Impedance"));
  t->array[3].item[1].tip = new QString(tr("Electrical Length"));

  t++;
  t->array[0].item[0].tip = new QString(tr("Relative Permittivity"));
  t->array[0].item[1].tip = new QString(tr("Relative Permeability"));
  t->array[0].item[2].tip = new QString(tr("Height of Substrate"));
  t->array[0].item[3].tip = new QString(tr("Height of Box Top"));
  t->array[0].item[4].tip = new QString(tr("Strip Thickness"));
  t->array[0].item[5].tip = new QString(tr("Strip Conductivity"));
  t->array[0].item[6].tip = new QString(tr("Dielectric Loss Tangent"));
  t->array[0].item[7].tip = new QString(tr("Conductor Roughness"));
  t->array[1].item[0].tip = new QString(tr("Frequency"));
  t->array[2].item[0].tip = new QString(tr("Line Width"));
  t->array[2].item[1].tip = new QString(tr("Gap Width"));
  t->array[2].item[2].tip = new QString(tr("Length"));
  t->array[3].item[0].tip = new QString(tr("Even-Mode Impedance"));
  t->array[3].item[1].tip = new QString(tr("Odd-Mode Impedance"));
  t->array[3].item[2].tip = new QString(tr("Electrical Length"));
}

/* Creates a property item 'val' in a parameter category specified by
   its 'box'. */
void QucsTranscalc::createPropItem (QVBox ** parentRows, TransValue * val,
				    int box, QButtonGroup * group) {
  QRadioButton * r = NULL;
  QLabel * l;
  QLineEdit * e;
  QComboBox * c;
  QDoubleValidator * v = new QDoubleValidator (this);

  // name label
  l = new QLabel (val->name, parentRows[0]);
  l->setAlignment (Qt::AlignRight);
  if (val->tip) QToolTip::add (l, *(val->tip));
  val->label = l;

  // editable value text
  e = new QLineEdit (parentRows[1]);
  e->setText (QString::number (val->value));
  e->setAlignment (Qt::AlignRight);
  e->setValidator (v);
  connect(e, SIGNAL(textChanged(const QString&)), SLOT(slotValueChanged()));
  if (!val->name) e->setDisabled (true);
  val->lineedit = e;

  // unit choice
  c = new QComboBox (parentRows[2]);
  if (!val->units[0]) {
    c->insertItem ("NA");
    c->setDisabled(true);
  }
  else {
    int nounit = 0;
    for (int i = 0; val->units[i]; i++) {
      c->insertItem (val->units[i]);
      if (!strcmp (val->units[i], "NA")) nounit++;
    }
    c->setDisabled (nounit != 0);
    c->setCurrentItem (0);
  }
  connect(c, SIGNAL(activated(int)), SLOT(slotValueChanged()));
  val->combobox = c;

  // special synthesize-computation choice
  if (box == TRANS_PHYSICAL) {
    QWidgetStack * s = new QWidgetStack(parentRows[3]);
    s->setFixedSize (val->lineedit->height()-10, val->lineedit->height()-10);
    r = new QRadioButton (s);
    QWidget * spacer = new QWidget (s);
    s->addWidget (r, 1);
    s->addWidget (spacer, 2);
    r->setDisabled (true);
    val->radio = r;
    val->stack = s;
    group->insert(r);
  }
}

/* The function changes the content of a GUI's property entry
   depending on the given transmission line property. */
void QucsTranscalc::updatePropItem (TransValue * val) {
  // update label text
  val->label->setText (val->name);
  QToolTip::remove (val->label);
  if (val->tip) QToolTip::add (val->label, *(val->tip));

  // update editable value text
  val->lineedit->setText (QString::number (val->value));
  val->lineedit->setDisabled (!val->name);

  // update unit choice
  val->combobox->clear();
  if (!val->units[0]) {
    val->combobox->insertItem ("NA");
    val->combobox->setDisabled (true);
  }
  else {
    int nounit = 0;
    for (int i = 0; val->units[i]; i++) {
      val->combobox->insertItem (val->units[i]);
      if (!strcmp (val->units[i], "NA")) nounit++;
    }
    val->combobox->setCurrentItem(val->unit);
    val->combobox->setDisabled (nounit);
  }
}

/* Switches to the current type of transmission line layout. */
void QucsTranscalc::setMode (int _mode) {
  // change necessary?
  if (mode == _mode) return;
  storeValues ();
  mode = _mode;
  setUpdatesEnabled(false);
  updateMode ();

  // update selection and results
  updateSelection ();
  updateResultItems ();

  slotAnalyze();
  setUpdatesEnabled(true);
  repaint();
}

/* Updates the property items of the current mode. */
void QucsTranscalc::updateMode (void) {
  // go through each type of parameter category
  for (int box = 0; box < MAX_TRANS_BOXES; box++) {
    int last = 0, idx = getTypeIndex();
    struct TransValue * val = TransLineTypes[idx].array[box].item;
    // update each property item
    for (int i = 0; i < TransMaxBox[box]; i++) {
      // fix uninitialized memory
      if (val->name == NULL) last++;
      if (last) {
	val->name = NULL;
	val->value = 0;
	val->tip = NULL;
	val->units[0] = NULL;
      }
      updatePropItem (val);
      val++;
    }
  }
}

/* Updates the current choice of physical property selection. */
void QucsTranscalc::updateSelection () {
  int idx = getTypeIndex ();
  struct TransValue * val = TransLineTypes[idx].array[TRANS_PHYSICAL].item;
  for (int i = 0; i < TransMaxBox[TRANS_PHYSICAL]; i++) {
    if (TransLineTypes[idx].radio[i] != -1) {
      val->stack->raiseWidget (1);
      if (TransLineTypes[idx].radio[i] == 1) {
	val->radio->setDown (true);
	val->radio->setChecked (true);
	QToolTip::add (val->radio, tr("Selected for Calculation"));
      }
      else {
	val->radio->setDown (false);
	val->radio->setChecked (false);
	QToolTip::add (val->radio, tr("Check item for Calculation"));
      }
      val->radio->setDisabled (false);
    }
    else {
      QToolTip::remove (val->radio);
      val->stack->raiseWidget (2);
      val->radio->setDown (false);
      val->radio->setChecked (false);
      val->radio->setDisabled (true);
    }
    val++;
  }
}

/* The function creates the property items for the given category of
   transmission line parameters. */
void QucsTranscalc::createPropItems (QHGroupBox * parent, int box) {
  struct TransValue * val, * dup;
  int last = 0, idx = getTypeIndex ();
  val = TransLineTypes[idx].array[box].item;

  QVBox *rows[4];
  QButtonGroup * group = new QButtonGroup();
  connect(group, SIGNAL(pressed(int)), SLOT(slotRadioChecked(int)));
  rows[0] = new QVBox (parent);
  rows[1] = new QVBox (parent);
  rows[2] = new QVBox (parent);
  rows[3] = new QVBox (parent);

  parent->layout()->setSpacing (2);
  rows[0]->setSpacing (2);
  rows[1]->setSpacing (2);
  rows[2]->setSpacing (2);
  rows[3]->setSpacing (2);

  // go through each parameter category
  for (int i = 0; i < TransMaxBox[box]; i++) {
    // fix uninitialized memory
    if (val->name == NULL) last++;
    if (last) {
      val->name = NULL;
      val->value = 0;
      val->tip = NULL;
      val->units[0] = NULL;
    }
    createPropItem (rows, val, box, group);
    // publish the newly created widgets to the other transmission lines
    for (int _mode = 0; _mode < MAX_TRANS_TYPES; _mode++) {
      if (idx != _mode) {
	dup = & TransLineTypes[_mode].array[box].item[i];
	dup->label = val->label;
	dup->lineedit = val->lineedit;
	dup->combobox = val->combobox;
	dup->radio = val->radio;
	dup->stack = val->stack;
      }
    }
    val++;
  }
}

/* Creates a single result item. */
void QucsTranscalc::createResultItem (QVBox ** parentRows, TransResult * res) {
  QLabel * n =
      new QLabel (res->name ? *(res->name) + ":" : QString(), parentRows[0]);
  n->setAlignment (Qt::AlignRight);
  res->label = n;
  QLabel * v = new QLabel (parentRows[1]);
  v->setAlignment (Qt::AlignLeft);
  res->value = v;
  if (!res->name) {
    v->hide ();
    n->hide ();
  }
}

/* Updates the given result item. */
void QucsTranscalc::updateResultItem (TransResult * res) {
  if (res->name) {
    res->label->setText (*(res->name) + ":");
    res->label->show ();
    res->value->show ();
  } else {
    res->label->hide ();
    res->value->hide ();
  }
}

/* Creates all the result items. */
void QucsTranscalc::createResultItems (QHGroupBox * parent) {
  struct TransResult * res, * dup;
  int idx = getTypeIndex ();
  res = & TransLineTypes[idx].result[0];

  QVBox *rows[2];
  rows[0] = new QVBox (parent);
  rows[1] = new QVBox (parent);

  parent->layout()->setSpacing (2);
  rows[0]->setSpacing (2);
  rows[1]->setSpacing (2);

  for (int i = 0; i < MAX_TRANS_RESULTS; i++) {
    createResultItem (rows, res);
    for (int _mode = 0; _mode < MAX_TRANS_TYPES; _mode++) {
      if (idx != _mode) {
	dup = & TransLineTypes[_mode].result[i];
	dup->label = res->label;
	dup->value = res->value;
      }
    }
    res++;
  }
}

/* Puts the given result into its widget. */
void QucsTranscalc::setResult (int line, const char * text) {
  struct TransResult * res;
  res = & TransLineTypes[getTypeIndex ()].result[line];
  res->value->setText (text);
}

/* Updates the current result items. */
void QucsTranscalc::updateResultItems() {
  int idx = getTypeIndex ();
  struct TransResult * res = TransLineTypes[idx].result;
  for (int i = 0; i < MAX_TRANS_RESULTS; i++) {
    updateResultItem (res);
    res++;
  }
}

/* Returns the appropriate array index for the given mode. */
int QucsTranscalc::getTypeIndex () {
  struct TransType * type = TransLineTypes;
  for (int i = 0; type->type != ModeNone; i++) {
    if (type->type == mode) return i;
    type++;
  }
  return 0;
}

/* Returns the property value specified by its name. */
struct TransValue * QucsTranscalc::findProperty (QString prop) {
  struct TransValue * val = NULL;
  for (int box = 0; box < MAX_TRANS_BOXES; box++) {
    val = TransLineTypes[getTypeIndex ()].array[box].item;
    for (int i = 0; val->name; i++) {
      if (prop == val->name) return val;
      val++;
    }
  }
  return NULL;
}

/* Sets the given property to the given value. */
void QucsTranscalc::setProperty (QString prop, double value) {
  struct TransValue * val = findProperty (prop);
  if (val) {
    val->lineedit->setText (QString::number (value));
    val->value = value;
  }
}

/* Returns the given property value. */
double QucsTranscalc::getProperty (QString prop) {
  struct TransValue * val = findProperty (prop);
  if (val) {
    QString str = val->lineedit->text ();
    val->value = str.toDouble ();
    return val->value;
  }
  return 0;
}

/* Sets the given property unit. */
void QucsTranscalc::setUnit (QString prop, const char * unit) {
  struct TransValue * val = findProperty (prop);
  if (val) {
    if (!unit) {
      val->combobox->setCurrentItem (0);
      val->unit = 0;
    }
    else for (int i = 0; val->units[i]; i++) {
      if (!strcmp (unit, val->units[i])) {
	val->combobox->setCurrentItem (i);
	val->unit = i;
	break;
      }
    }
  }
}

/* Returns the given property unit. */
char * QucsTranscalc::getUnit (QString prop) {
  struct TransValue * val = findProperty (prop);
  if (val) {
    QString str = val->combobox->currentText ();
    for (int i = 0; val->units[i]; i++) {
      if (str == val->units[i]) {
	val->unit = i;
	return (char *) val->units[i];
      }
    }
  }
  return NULL;
}

/* Returns the given property selection. */
bool QucsTranscalc::isSelected (QString prop) {
  struct TransValue * val = findProperty (prop);
  if (val) {
    if (val->radio)
      if (val->radio->isChecked ()) {
	return true;
      }
  }
  return false;
}

void QucsTranscalc::slotAbout()
{
  QMessageBox::about(this, tr("About..."),
    "QucsTranscalc Version " PACKAGE_VERSION "\n"+
    tr("Transmission Line Calculator for Qucs\n")+
    tr("Copyright (C) 2001 by Gopal Narayanan\n")+
    tr("Copyright (C) 2002 by Claudio Girardi\n")+
    tr("Copyright (C) 2005 by Stefan Jahn\n")+
    tr("Copyright (C) 2008 by Michael Margraf\n")+
    "\nThis is free software; see the source for copying conditions."
    "\nThere is NO warranty; not even for MERCHANTABILITY or "
    "\nFITNESS FOR A PARTICULAR PURPOSE.");
}

void QucsTranscalc::slotQuit()
{
  int tmp;
  tmp = x();		// call size and position function in order to ...
  tmp = y();		// ... set them correctly before closing the ...
  tmp = width();	// dialog !!!  Otherwise the frame of the window ...
  tmp = height();	// will not be recognized (a X11 problem).

  storeValues ();
  accept();
}

void QucsTranscalc::closeEvent(QCloseEvent *Event)
{
  int tmp;
  tmp = x();		// call size and position function in order to ...
  tmp = y();		// ... set them correctly before closing the ...
  tmp = width();	// dialog !!!  Otherwise the frame of the window ...
  tmp = height();	// will not be recognized (a X11 problem).

  storeValues ();
  Event->accept();
}

void QucsTranscalc::slotSelectType (int Type)
{
  pix->setPixmap (QPixmap (QImage (
       QucsSettings.BitmapDir + QString(TransLineTypes[Type].bitmap))));
  setMode (Type);
  statBar->message(tr("Ready."));
}

void QucsTranscalc::slotAnalyze()
{
  if (TransLineTypes[getTypeIndex()].line)
    TransLineTypes[getTypeIndex()].line->analyze();
  statBar->message(tr("Values are consistent."));
}

void QucsTranscalc::slotSynthesize()
{
  if (TransLineTypes[getTypeIndex()].line)
    TransLineTypes[getTypeIndex()].line->synthesize();
  statBar->message(tr("Values are consistent."));
}

void QucsTranscalc::slotValueChanged()
{
  statBar->message(tr("Values are inconsistent."));
}

// Load transmission line values from the given file.
bool QucsTranscalc::loadFile(QString fname, int * _mode) {
  QFile file(QDir::convertSeparators (fname));
  if(!file.open(IO_ReadOnly)) return false; // file doesn't exist

  QTextStream stream(&file);
  QString Line, Name, Unit;
  double Value;

  int oldmode = mode;
  while(!stream.atEnd()) {
    Line = stream.readLine();
    for (int i = 0; i < MAX_TRANS_TYPES; i++) {
      if (Line == "<" + QString(TransLineTypes[i].description) + ">") {
	if (_mode) {
	  *_mode = TransLineTypes[i].type;
	  setMode (*_mode);
	  updatePixmap (mode);
	} else {
	  mode = TransLineTypes[i].type;
	}
	while(!stream.atEnd()) {
	  Line = stream.readLine();
	  if (Line == "</" + QString(TransLineTypes[i].description) + ">")
	    break;
	  Line = Line.simplifyWhiteSpace();
	  Name = Line.section(' ',0,0);
	  Value = Line.section(' ',1,1).toDouble();
	  Unit = Line.section(' ',2,2);
	  setProperty (Name, Value);
	  setUnit (Name, Unit);
	}
	break;
      }
    }
  }
  if (!_mode) mode = oldmode;
  updateMode ();
  file.close();
  return true;
}

// Saves current transmission line values into the given file.
bool QucsTranscalc::saveFile(QString fname) {
  QFile file (QDir::convertSeparators (fname));
  if(!file.open (IO_WriteOnly)) return false; // file not writable
  QTextStream stream (&file);

  // some lines of documentation
  stream << "# QucsTranscalc " << PACKAGE_VERSION << "  " << fname << "\n";
  stream << "#   Generated on " << QDate::currentDate().toString()
	 << " at " << QTime::currentTime().toString() << ".\n";
  stream << "#   It is not suggested to edit the file, use QucsTranscalc "
	 << "instead.\n\n";

  storeValues ();
  saveMode (stream);
  file.close ();
  return true;
}

/* Writes the transmission line values for the current modes into the
   given stream. */
void QucsTranscalc::saveMode(QTextStream& stream) {
  struct TransType * t = &TransLineTypes[getTypeIndex ()];
  struct TransValue * val = NULL;
  stream << "<" << t->description << ">\n";
  for (int box = 0; box < MAX_TRANS_BOXES; box++) {
    val = t->array[box].item;
    while (val->name) {
      stream << "  " << val->name << " " << val->value << " "
	     << val->units[val->unit] << "\n";
      val++;
    }
  }
  stream << "</" << t->description << ">\n";
}

// Writes the transmission line values for all modes into the given stream.
void QucsTranscalc::saveModes(QTextStream& stream) {
  int oldmode = mode;
  for (int i = 0; i < MAX_TRANS_TYPES; i++) {
    mode = TransLineTypes[i].type;
    saveMode (stream);
  }
  mode = oldmode;
}

// Saves the GUI values into internal data structures.
void QucsTranscalc::storeValues (void) {
  struct TransType * t = &TransLineTypes[getTypeIndex ()];
  struct TransValue * val;
  for (int box = 0; box < MAX_TRANS_BOXES; box++) {
    val = t->array[box].item;
    int i = 0;
    while (val->name) {
      getProperty (val->name);
      getUnit (val->name);
      if (box == TRANS_PHYSICAL) {
	if (val->radio->isEnabled()) {
	  if (val->radio->isChecked())
	    t->radio[i] = 1;
	  else
	    t->radio[i] = 0;
	}
	else t->radio[i] = -1;
      }
      i++;
      val++;
    }
  }
}

void QucsTranscalc::slotFileLoad()
{
  statBar->message(tr("Loading file..."));
  int _mode = 0;
  QString s = QFileDialog::getOpenFileName(QucsWorkDir.path(),
   tr("Transcalc File")+" (*.trc)", this, "", tr("Enter a Filename"));
  if (!s.isEmpty())  {
    QucsWorkDir.setPath(QDir::cleanDirPath(s));
    if (!loadFile (s, &_mode)) {
      QMessageBox::critical (this, tr("Error"),
			     tr("Cannot load file:")+" '"+s+"'!");
    }
  }
  else statBar->message(tr("Loading aborted."), 2000);

  statBar->message(tr("Ready."));
}

void QucsTranscalc::slotFileSave()
{
  statBar->message(tr("Saving file..."));

  QString s = QFileDialog::getSaveFileName(QucsWorkDir.path(),
   tr("Transcalc File")+" (*.trc)", this, "", tr("Enter a Filename"));
  if (!s.isEmpty())  {
    QucsWorkDir.setPath(QDir::cleanDirPath(s));
    if (!saveFile (s)) {
      QMessageBox::critical (this, tr("Error"),
			     tr("Cannot save file:")+" '"+s+"'!");
    }
  }
  else statBar->message(tr("Saving aborted."), 2000);

  statBar->message(tr("Ready."));
}

// Returns the current textual mode.
QString QucsTranscalc::getMode (void) {
  return TransLineTypes[getTypeIndex ()].description;
}

// Sets the mode according to the given textual mode.
void QucsTranscalc::setMode (QString _mode) {
  for (int i = 0; i < MAX_TRANS_TYPES; i++) {
    if (TransLineTypes[i].description == _mode) {
      setMode (TransLineTypes[i].type);
      updatePixmap (mode);
      break;
    }
  }
}

// Updates the combobox and pixmap for the current mode.
void QucsTranscalc::updatePixmap (int _mode) {
  pix->setPixmap (QPixmap (QImage (
       QucsSettings.BitmapDir + QString(TransLineTypes[_mode].bitmap))));
  tranType->setCurrentItem(_mode);
}

void QucsTranscalc::slotHelpIntro()
{
  HelpDialog *d = new HelpDialog(this);
  d->show();
}

void QucsTranscalc::slotOptions()
{
  OptionsDialog *d = new OptionsDialog(this);
  d->show();
}

// Translates a given textual unit into a array index.
int QucsTranscalc::translateUnit(const char * unit, int type) {
  for (int i = 0; TransUnits[type].units[i]; i++) {
    if (unit && !strcmp (unit, TransUnits[type].units[i]))
      return i;
  }
  return 0;
}

void QucsTranscalc::slotRadioChecked(int id)
{
  int idx = getTypeIndex ();
  for (int i = 0; i < TransMaxBox[TRANS_PHYSICAL]; i++) {
    if (TransLineTypes[idx].radio[i] != -1) {
      TransLineTypes[idx].radio[i] = 0;
      if (i == id) {
	TransLineTypes[idx].radio[i] = 1;
      }
    }
  }
  updateSelection ();
}

void QucsTranscalc::slotCopyToClipBoard()
{
  int created = 0;
  QString s = "<Qucs Schematic " PACKAGE_VERSION ">\n";

  // create microstrip schematic
  if (mode == ModeMicrostrip) {
    transline * l = TransLineTypes[ModeMicrostrip].line;
    s += "<Components>\n";
    s += "  <Pac P1 1 90 150 -74 -26 1 1 \"1\" 1 \"50 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n";
    s +="  <Pac P2 1 270 150 18 -26 0 1 \"2\" 1 \"50 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n";
    s += "  <GND * 1 90 180 0 0 0 0>\n";
    s += "  <GND * 1 270 180 0 0 0 0>\n";
    s += QString("  <SUBST SubstTC1 1 390 140 -30 24 0 0 \"%1\" 1 \"%2 mm\" 1 \"%3 um\" 1 \"%4\" 1 \"%5\" 1 \"%6\" 1>\n").
      arg(l->getProperty("Er")).
      arg(l->getProperty("H", UNIT_LENGTH, LENGTH_MM)).
      arg(l->getProperty("T", UNIT_LENGTH, LENGTH_UM)).
      arg(l->getProperty("Tand")).
      arg(1 / l->getProperty("Cond")).
      arg(l->getProperty("Rough", UNIT_LENGTH, LENGTH_M));
    s += "  <.SP SPTC1 1 90 240 0 51 0 0 ";
    double freq = l->getProperty("Freq", UNIT_FREQ, FREQ_GHZ);
    if (freq > 0)
      s += QString("\"log\" 1 \"%1 GHz\" 1 \"%2 GHz\" 1 ").
	arg(freq / 10).arg(freq * 10);
    else
      s += "\"lin\" 1 \"0 GHz\" 1 \"10 GHz\" 1 ";
    s += "\"51\" 1 \"no\" 0 \"1\" 0 \"2\" 0>\n";
    s += QString("  <MLIN MSTC1 1 180 100 -26 15 0 0 \"SubstTC1\" 1 \"%1 mm\" 1 \"%2 mm\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").
      arg(l->getProperty("W", UNIT_LENGTH, LENGTH_MM)).
      arg(l->getProperty("L", UNIT_LENGTH, LENGTH_MM));
    s += "  <Eqn EqnTC1 1 240 260 -23 12 0 0 \"A=twoport(S,'S','A')\" 1 \"ZL=real(sqrt(A[1,2]/A[2,1]))\" 1 \"yes\" 0>\n"; 
    s += "</Components>\n";
    s += "<Wires>\n";
    s += "  <90 100 150 100 \"\" 0 0 0 \"\">\n";
    s += "  <90 100 90 120 \"\" 0 0 0 \"\">\n";
    s += "  <210 100 270 100 \"\" 0 0 0 \"\">\n";
    s += "  <270 100 270 120 \"\" 0 0 0 \"\">\n";
    s += "</Wires>\n";
    created++;
  }

  // create coupled microstrip schematic
  else if (mode == ModeCoupledMicrostrip) {
    transline * l = TransLineTypes[ModeCoupledMicrostrip].line;
    s += "<Components>\n";
    s += "  <Pac P1 1 100 130 -74 -26 1 1 \"1\" 1 \"50 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n";
    s += "  <Pac P2 1 320 130 18 -26 0 1 \"2\" 1 \"50 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n";
    s += "  <Pac P3 1 280 220 18 -26 0 1 \"3\" 1 \"50 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n";
    s += "  <Pac P4 1 140 200 -74 -26 1 1 \"4\" 1 \"50 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n";
    s += "  <GND * 1 100 160 0 0 0 0>\n";
    s += "  <GND * 1 140 230 0 0 0 0>\n";
    s += "  <GND * 1 320 160 0 0 0 0>\n";
    s += "  <GND * 1 280 250 0 0 0 0>\n";
    s += QString("  <SUBST SubstTC1 1 410 220 -30 24 0 0 \"%1\" 1 \"%2 mm\" 1 \"%3 um\" 1 \"%4\" 1 \"%5\" 1 \"%6\" 1>\n").
      arg(l->getProperty("Er")).
      arg(l->getProperty("H", UNIT_LENGTH, LENGTH_MM)).
      arg(l->getProperty("T", UNIT_LENGTH, LENGTH_UM)).
      arg(l->getProperty("Tand")).
      arg(1 / l->getProperty("Cond")).
      arg(l->getProperty("Rough", UNIT_LENGTH, LENGTH_M));
    s += "  <.SP SPTC1 1 100 290 0 51 0 0 ";
    double freq = l->getProperty("Freq", UNIT_FREQ, FREQ_GHZ);
    if (freq > 0)
      s += QString("\"log\" 1 \"%1 GHz\" 1 \"%2 GHz\" 1 ").
	arg(freq / 10).arg(freq * 10);
    else
      s += "\"lin\" 1 \"0 GHz\" 1 \"10 GHz\" 1 ";
    s += "\"51\" 1 \"no\" 0 \"1\" 0 \"2\" 0>\n";
    s += QString("  <MCOUPLED MSTC1 1 190 110 -26 37 0 0 \"SubstTC1\" 1 \"%1 mm\" 1 \"%2 mm\" 1 \"%3 mm\" 1 \"Kirschning\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").
      arg(l->getProperty("W", UNIT_LENGTH, LENGTH_MM)).
      arg(l->getProperty("L", UNIT_LENGTH, LENGTH_MM)).
      arg(l->getProperty("S", UNIT_LENGTH, LENGTH_MM));
    s += "</Components>\n";
    s += "<Wires>\n";
    s += "  <100 80 160 80 \"\" 0 0 0 \"\">\n";
    s += "  <100 80 100 100 \"\" 0 0 0 \"\">\n";
    s += "  <140 140 140 170 \"\" 0 0 0 \"\">\n";
    s += "  <140 140 160 140 \"\" 0 0 0 \"\">\n";
    s += "  <320 80 320 100 \"\" 0 0 0 \"\">\n";
    s += "  <220 80 320 80 \"\" 0 0 0 \"\">\n";
    s += "  <280 140 280 190 \"\" 0 0 0 \"\">\n";
    s += "  <220 140 280 140 \"\" 0 0 0 \"\">\n";
    s += "</Wires>\n";
    created++;
  }

  // create coaxial line schematic
  else if (mode == ModeCoaxial) {
    transline * l = TransLineTypes[ModeCoaxial].line;
    s += "<Components>\n";
    s += "  <Pac P1 1 90 150 -74 -26 1 1 \"1\" 1 \"50 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n";
    s +="  <Pac P2 1 270 150 18 -26 0 1 \"2\" 1 \"50 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n";
    s += "  <GND * 1 90 180 0 0 0 0>\n";
    s += "  <GND * 1 270 180 0 0 0 0>\n";
    s += "  <.SP SPTC1 1 90 240 0 51 0 0 ";
    double freq = l->getProperty("Freq", UNIT_FREQ, FREQ_GHZ);
    if (freq > 0)
      s += QString("\"log\" 1 \"%1 GHz\" 1 \"%2 GHz\" 1 ").
	arg(freq / 10).arg(freq * 10);
    else
      s += "\"lin\" 1 \"0 GHz\" 1 \"10 GHz\" 1 ";
    s += "\"51\" 1 \"no\" 0 \"1\" 0 \"2\" 0>\n";
    s += QString("  <COAX CXTC1 1 180 100 -26 15 0 0 \"%1\" 1 \"%2\" 0 \"%3\" 0 \"%4 mm\" 1 \"%5 mm\" 1  \"%6 mm\" 1  \"%7\" 0 \"26.85\" 0>\n").
      arg(l->getProperty("Er")).
      arg(1 / l->getProperty("Sigma")).
      arg(l->getProperty("Mur")).
      arg(l->getProperty("dout", UNIT_LENGTH, LENGTH_MM)).
      arg(l->getProperty("din", UNIT_LENGTH, LENGTH_MM)).
      arg(l->getProperty("L", UNIT_LENGTH, LENGTH_MM)).
      arg(l->getProperty("Tand"));
    s += "</Components>\n";
    s += "<Wires>\n";
    s += "  <90 100 150 100 \"\" 0 0 0 \"\">\n";
    s += "  <90 100 90 120 \"\" 0 0 0 \"\">\n";
    s += "  <210 100 270 100 \"\" 0 0 0 \"\">\n";
    s += "  <270 100 270 120 \"\" 0 0 0 \"\">\n";
    s += "</Wires>\n";
    created++;
  }

  // create coplanar schematic
  else if (mode == ModeCoplanar) {
    transline * l = TransLineTypes[ModeCoplanar].line;
    s += "<Components>\n";
    s += "  <Pac P1 1 90 150 -74 -26 1 1 \"1\" 1 \"50 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n";
    s +="  <Pac P2 1 270 150 18 -26 0 1 \"2\" 1 \"50 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n";
    s += "  <GND * 1 90 180 0 0 0 0>\n";
    s += "  <GND * 1 270 180 0 0 0 0>\n";
    s += QString("  <SUBST SubstTC1 1 390 140 -30 24 0 0 \"%1\" 1 \"%2 mm\" 1 \"%3 um\" 1 \"%4\" 1 \"%5\" 1 \"0\" 1>\n").
      arg(l->getProperty("Er")).
      arg(l->getProperty("H", UNIT_LENGTH, LENGTH_MM)).
      arg(l->getProperty("T", UNIT_LENGTH, LENGTH_UM)).
      arg(l->getProperty("Tand")).
      arg(1 / l->getProperty("Cond"));
    s += "  <.SP SPTC1 1 90 240 0 51 0 0 ";
    double freq = l->getProperty("Freq", UNIT_FREQ, FREQ_GHZ);
    if (freq > 0)
      s += QString("\"log\" 1 \"%1 GHz\" 1 \"%2 GHz\" 1 ").
	arg(freq / 10).arg(freq * 10);
    else
      s += "\"lin\" 1 \"0 GHz\" 1 \"10 GHz\" 1 ";
    s += "\"51\" 1 \"no\" 0 \"1\" 0 \"2\" 0>\n";
    s += QString("  <CLIN CLTC1 1 180 100 -26 25 0 0 \"SubstTC1\" 1 \"%1 mm\" 1 \"%2 mm\" 1 \"%3 mm\" 1 \"Air\" 1 \"yes\" 0>\n").
      arg(l->getProperty("W", UNIT_LENGTH, LENGTH_MM)).
      arg(l->getProperty("S", UNIT_LENGTH, LENGTH_MM)).
      arg(l->getProperty("L", UNIT_LENGTH, LENGTH_MM));
    s += "  <Eqn EqnTC1 1 240 260 -23 12 0 0 \"A=twoport(S,'S','A')\" 1 \"ZL=real(sqrt(A[1,2]/A[2,1]))\" 1 \"yes\" 0>\n"; 
    s += "</Components>\n";
    s += "<Wires>\n";
    s += "  <90 100 150 100 \"\" 0 0 0 \"\">\n";
    s += "  <90 100 90 120 \"\" 0 0 0 \"\">\n";
    s += "  <210 100 270 100 \"\" 0 0 0 \"\">\n";
    s += "  <270 100 270 120 \"\" 0 0 0 \"\">\n";
    s += "</Wires>\n";
    created++;
  }

  // create coplanar schematic
  else if (mode == ModeGroundedCoplanar) {
    transline * l = TransLineTypes[ModeGroundedCoplanar].line;
    s += "<Components>\n";
    s += "  <Pac P1 1 90 150 -74 -26 1 1 \"1\" 1 \"50 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n";
    s +="  <Pac P2 1 270 150 18 -26 0 1 \"2\" 1 \"50 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n";
    s += "  <GND * 1 90 180 0 0 0 0>\n";
    s += "  <GND * 1 270 180 0 0 0 0>\n";
    s += QString("  <SUBST SubstTC1 1 390 140 -30 24 0 0 \"%1\" 1 \"%2 mm\" 1 \"%3 um\" 1 \"%4\" 1 \"%5\" 1 \"0\" 1>\n").
      arg(l->getProperty("Er")).
      arg(l->getProperty("H", UNIT_LENGTH, LENGTH_MM)).
      arg(l->getProperty("T", UNIT_LENGTH, LENGTH_UM)).
      arg(l->getProperty("Tand")).
      arg(1 / l->getProperty("Cond"));
    s += "  <.SP SPTC1 1 90 240 0 51 0 0 ";
    double freq = l->getProperty("Freq", UNIT_FREQ, FREQ_GHZ);
    if (freq > 0)
      s += QString("\"log\" 1 \"%1 GHz\" 1 \"%2 GHz\" 1 ").
	arg(freq / 10).arg(freq * 10);
    else
      s += "\"lin\" 1 \"0 GHz\" 1 \"10 GHz\" 1 ";
    s += "\"51\" 1 \"no\" 0 \"1\" 0 \"2\" 0>\n";
    s += QString("  <CLIN CLTC1 1 180 100 -26 25 0 0 \"SubstTC1\" 1 \"%1 mm\" 1 \"%2 mm\" 1 \"%3 mm\" 1 \"Metal\" 1 \"yes\" 0>\n").
      arg(l->getProperty("W", UNIT_LENGTH, LENGTH_MM)).
      arg(l->getProperty("S", UNIT_LENGTH, LENGTH_MM)).
      arg(l->getProperty("L", UNIT_LENGTH, LENGTH_MM));
    s += "  <Eqn EqnTC1 1 240 260 -23 12 0 0 \"A=twoport(S,'S','A')\" 1 \"ZL=real(sqrt(A[1,2]/A[2,1]))\" 1 \"yes\" 0>\n"; 
    s += "</Components>\n";
    s += "<Wires>\n";
    s += "  <90 100 150 100 \"\" 0 0 0 \"\">\n";
    s += "  <90 100 90 120 \"\" 0 0 0 \"\">\n";
    s += "  <210 100 270 100 \"\" 0 0 0 \"\">\n";
    s += "  <270 100 270 120 \"\" 0 0 0 \"\">\n";
    s += "</Wires>\n";
    created++;
  }

  // rectangular waveguide schematic
  else if (mode == ModeRectangular) {
    transline * l = TransLineTypes[ModeRectangular].line;
    s += "<Components>\n";
    s += "  <Pac P1 1 90 150 -74 -26 1 1 \"1\" 1 \"50 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n";
    s +="  <Pac P2 1 270 150 18 -26 0 1 \"2\" 1 \"50 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n";
    s += "  <GND * 1 90 180 0 0 0 0>\n";
    s += "  <GND * 1 270 180 0 0 0 0>\n";
    s += "  <.SP SPTC1 1 90 240 0 51 0 0 ";
    double freq = l->getProperty("Freq", UNIT_FREQ, FREQ_GHZ);
    if (freq > 0)
      s += QString("\"log\" 1 \"%1 GHz\" 1 \"%2 GHz\" 1 ").
	arg(freq / 10).arg(freq * 10);
    else
      s += "\"lin\" 1 \"0 GHz\" 1 \"10 GHz\" 1 ";
    s += "\"51\" 1 \"no\" 0 \"1\" 0 \"2\" 0>\n";
    s += QString("  <RECTLINE RLTC1 1 180 100 -26 25 0 0 \"%1 mm\" 1 \"%2 mm\" 1 \"%3 mm\" 1 \"%4\" 0 \"%5\" 0 \"%6\" 0 \"%7\" 0 \"26.85\" 0>\n").
      arg(l->getProperty("a", UNIT_LENGTH, LENGTH_MM)).
      arg(l->getProperty("b", UNIT_LENGTH, LENGTH_MM)).
      arg(l->getProperty("L", UNIT_LENGTH, LENGTH_MM)).
      arg(l->getProperty("Er")).
      arg(l->getProperty("Mur")).
      arg(l->getProperty("Tand")).
      arg(1 / l->getProperty("Cond"));
    s += "  <Eqn EqnTC1 1 240 260 -23 12 0 0 \"A=twoport(S,'S','A')\" 1 \"ZL=real(sqrt(A[1,2]/A[2,1]))\" 1 \"yes\" 0>\n"; 
    s += "</Components>\n";
    s += "<Wires>\n";
    s += "  <90 100 150 100 \"\" 0 0 0 \"\">\n";
    s += "  <90 100 90 120 \"\" 0 0 0 \"\">\n";
    s += "  <210 100 270 100 \"\" 0 0 0 \"\">\n";
    s += "  <270 100 270 120 \"\" 0 0 0 \"\">\n";
    s += "</Wires>\n";
    created++;
  }

  // put resulting transmission line schematic into clipboard
  QClipboard *cb = QApplication::clipboard();
  cb->setText(s);

  // put a message into status line
  if (created)
    statBar->message(tr("Schematic copied into clipboard."), 2000);
  else
    statBar->message(tr("Transmission line type not available."), 2000);
}
