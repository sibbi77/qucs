/***************************************************************************
 * Copyright (C) 2006 by Gopala Krishna A <krishna.ggk@gmail.com>          *
 *                                                                         *
 * This is free software; you can redistribute it and/or modify            *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2, or (at your option)     *
 * any later version.                                                      *
 *                                                                         *
 * This software is distributed in the hope that it will be useful,        *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this package; see the file COPYING.  If not, write to        *
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,   *
 * Boston, MA 02110-1301, USA.                                             *
 ***************************************************************************/

#ifndef SAVEDIALOG_H
#define SAVEDIALOG_H

#include <qvariant.h>
#include <qdialog.h>
#include <qmap.h>
#include <qlistview.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QLabel;
class QListView;
class QListViewItem;
class QPushButton;
class QucsDoc;
class QucsApp;

class SaveDialog : public QDialog
{
   Q_OBJECT
   public:
      enum {
         AbortClosing = 0,
         DontSave,
         SaveSelected
      };
            
      SaveDialog(QWidget* p = 0, const char* n = 0, bool modal = true, WFlags fl = 0 );
      ~SaveDialog();
      void addUnsavedDoc(QucsDoc *doc);
      void setApp(QucsApp *a);
      bool isEmpty() const;
			     
   protected slots:
      void dontSaveClicked();
      void saveSelectedClicked();
      void languageChange();
      void reject();
      
   private:
      void initDialog();
      
      QMap<QucsDoc*,QCheckListItem*> unsavedDocs;
            
      QLabel* label;
      QListView* filesView;
      QPushButton* abortClosingButton;
      QPushButton* dontSaveButton;
      QPushButton* saveSelectedButton;
      QVBoxLayout* SaveDialogLayout;
      QHBoxLayout* buttonsLayout;
      QSpacerItem* spacer;
      QucsApp *app;
};

#endif // SAVEDIALOG_H
