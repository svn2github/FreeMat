/*
 * Copyright (c) 2002-2006 Samit Basu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#include "QTTerm.hpp"
#include "KeyManager.hpp"
#include <qapplication.h>
#include <qclipboard.h>
#include <math.h>
#include <QKeyEvent>
#include <QDebug>
#include <iostream>


QTTerm::QTTerm(QWidget *parent) : QTextEdit(parent) {
  setObjectName("qtterm");
  setMinimumSize(100,100);
  setLineWrapMode(QTextEdit::NoWrap);
  setCursorWidth(10);
  setOverwriteMode(true);
  autoFlush = new QTimer(this);
  connect(autoFlush,SIGNAL(timeout()),this,SLOT(Flush()));
  autoFlush->start(50);
  destCursor = textCursor();

#ifdef __APPLE__
  QFont afont("Monaco",10);
#elif WIN32
  QFont afont("Lucida Console",10);
#else
  QFont afont("Monospace",10);
#endif
  setFont(afont);
}

#ifndef __APPLE__
#define CTRLKEY(x)  else if ((keycode == x) && (e->modifiers() & Qt::ControlModifier))
#else
#define CTRLKEY(x)  else if ((keycode == x) && (e->modifiers() & Qt::MetaModifier))
#endif

void QTTerm::setFont(QFont font) {
  fnt = font;
  qDebug() << "Font set to " << font.toString();
  QTextCursor cur(textCursor());
  cur.movePosition(QTextCursor::Start);
  cur.movePosition(QTextCursor::End,QTextCursor::KeepAnchor);
  QTextCharFormat cfrmt(cur.charFormat());
  cfrmt.setFont(font);
  cur.setCharFormat(cfrmt);
  destCursor.setCharFormat(cfrmt);
  setCurrentFont(fnt);
}

QFont QTTerm::getFont() {
  return fnt;
}

void QTTerm::keyPressEvent(QKeyEvent *e) {
  int keycode = e->key(); 
  if (!keycode) return;
  if (keycode == Qt::Key_Left)
    emit OnChar(KM_LEFT);
  CTRLKEY('Z')
    emit OnChar(KM_CTRLK);
  CTRLKEY('A')
    emit OnChar(KM_CTRLA);
  CTRLKEY('D')
    emit OnChar(KM_CTRLD); 
 CTRLKEY('E')
    emit OnChar(KM_CTRLE);
  CTRLKEY('K')
    emit OnChar(KM_CTRLK);
  CTRLKEY('Y')
    emit OnChar(KM_CTRLY);
  else if (keycode == Qt::Key_Right)
    emit OnChar(KM_RIGHT);
  else if (keycode == Qt::Key_Up)
    emit OnChar(KM_UP);
  else if (keycode == Qt::Key_Down)
    emit OnChar(KM_DOWN);
  else if (keycode == Qt::Key_Delete)
    emit OnChar(KM_DELETE);
  else if (keycode == Qt::Key_Insert)
    emit OnChar(KM_INSERT);
  else if (keycode == Qt::Key_Home)
    emit OnChar(KM_HOME);
  else if (keycode == Qt::Key_End)
    emit OnChar(KM_END);
  else if (keycode == Qt::Key_Return) {
    emit OnChar(KM_NEWLINE);
    adjustScrollback();
  }
  else if (keycode == Qt::Key_Backspace)
    emit OnChar(KM_BACKSPACE);
  else {
    QByteArray p(e->text().toAscii());
    char key;
    if (!e->text().isEmpty())
      key = p[0];
    else
      key = 0;
    if (key) {
      emit OnChar(key);
      e->accept();
    } else
      e->ignore();
  }
}

void QTTerm::adjustScrollback() {
  QTextCursor cur(textCursor());
  if (cur.position() > 100000) {
    qDebug() << "Adjusting scrollback";
    // Moved beyond the scroll back limit
    int toDel = cur.position() - 90000;
    QTextCursor del(textCursor());
    del.movePosition(QTextCursor::Start);
    while (del.position() < toDel) {
      del.movePosition(QTextCursor::Down,QTextCursor::KeepAnchor);
    }
    del.removeSelectedText();
  }
}

void QTTerm::MoveDown() {
  Flush();
  qDebug() << "Down";
  destCursor.movePosition(QTextCursor::Down);
  setTextCursor(destCursor);
}

void QTTerm::MoveUp() {
  Flush();
  qDebug() << "Up";
  destCursor.movePosition(QTextCursor::Up);
  setTextCursor(destCursor);
}

void QTTerm::MoveLeft() {
  Flush();
  qDebug() << "Left";
  destCursor.movePosition(QTextCursor::Left);
  setTextCursor(destCursor);
}

void QTTerm::MoveRight() {
  Flush();
  qDebug() << "Right";
  destCursor.movePosition(QTextCursor::Right);
  setTextCursor(destCursor);
}

void QTTerm::MoveBOL() {
  Flush();
  qDebug() << "Bol";
  destCursor.movePosition(QTextCursor::StartOfLine);
  setTextCursor(destCursor);
}

void QTTerm::ClearDisplay() {
  Flush();
  qDebug() << "CLD";
  destCursor.movePosition(QTextCursor::Start);
  destCursor.movePosition(QTextCursor::End,QTextCursor::KeepAnchor);
  destCursor.removeSelectedText();
  setTextCursor(destCursor);
}

QString QTTerm::getAllText() {
  QTextCursor cur(textCursor());
  cur.movePosition(QTextCursor::Start);
  cur.movePosition(QTextCursor::End,QTextCursor::KeepAnchor);
  return cur.selectedText();
}

QString QTTerm::getSelectionText() {
  return textCursor().selectedText();
}

void QTTerm::ClearEOL() {
  Flush();
  qDebug() << "CEOL";
  destCursor.movePosition(QTextCursor::EndOfLine,QTextCursor::KeepAnchor);
  destCursor.removeSelectedText();
  setTextCursor(destCursor);
}

void QTTerm::ClearEOD() {
  Flush();
  qDebug() << "CEOD";
  destCursor.movePosition(QTextCursor::End,QTextCursor::KeepAnchor);
  destCursor.removeSelectedText();
  setTextCursor(destCursor);
}

void QTTerm::OutputRawString(string txt) {
  QString emitText(QString::fromStdString(txt));
  putbuf += emitText;
}

void QTTerm::Output(QString fragment) {
  if (destCursor.atEnd())
    destCursor.insertText(fragment);
  else {
    for (int i=0;i<fragment.size();i++)
      destCursor.deleteChar();
    destCursor.insertText(fragment);
  }    
  setTextCursor(destCursor);
}

void QTTerm::Flush() {
  if (putbuf.isEmpty()) return;
  putbuf.replace("\r\n","\n");
  if (!putbuf.contains('\r')) 
    Output(putbuf);
  else {
    QStringList pbuft(putbuf.split('\r'));
    for (int i=0;i<pbuft.size();i++) {
      qDebug() << "Fragment: " << pbuft[i];
      Output(pbuft[i]);
      destCursor.movePosition(QTextCursor::StartOfLine);
      setTextCursor(destCursor);
    }
  }
  ensureCursorVisible();
  putbuf.clear();
}

void QTTerm::clearSelection() {
  QTextCursor cur(textCursor());
  cur.clearSelection();
  setTextCursor(cur);
}
