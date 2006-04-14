#include "blastuneapp.hpp"
#include <QtGui>
#include <QtCore>

static int libcount=0;
SingleSelectTable *m_table;

void TunerWidget::addlibs() {
  QStringList files = QFileDialog::getOpenFileNames(this,
						    "Select one or more BLAS libraries to test","/home","Libraries (*.dll *.so)");
  QStringList list = files;
  QStringList::Iterator it = list.begin();
  while (it != list.end()) {
    m_table->insertRow(libcount);
    m_table->setItem(libcount++,0,new QTableWidgetItem(*it));
    ++it;
  }
  m_table->resizeColumnsToContents();
  m_table->verticalHeader()->hide();
}

void TunerWidget::runtime() {
  // Loop over the libraries
  QProgressDialog progress("Timing libraries...","Abort Timings",0,libcount,this);
  for (int i=0;i<libcount;i++) {
    progress.setValue(i);
    qApp->processEvents();
    if (progress.wasCanceled())
      break;
    QProcess timeproc;
    QString libname (m_table->item(i,0)->text());
    int retval;
    retval = -1;
    int eTime;
    float eFlops;
    QTime tme;
    tme.start();
    retval = timeproc.execute("BlasTune",QStringList() << libname);
    eTime = tme.elapsed();
    eFlops = 500*500*500*10/(eTime/1000.)/1e6;
    if (retval == 0) {
      m_table->setItem(i,1,new QTableWidgetItem("Yes"));
      m_table->setItem(i,2,new QTableWidgetItem(QString::number(eFlops)));
    }    else
      m_table->setItem(i,1,new QTableWidgetItem("No"));
  }
  progress.setValue(libcount);
}

void TunerWidget::selectlib() {
  // Get the currently selected library
  QList<QTableWidgetItem *> sel(m_table->selectedItems());
  bool failed;
  if (sel.empty()) 
    failed = true;
  else
    failed = sel.at(1)->text() == "No";
  if (failed)
    QMessageBox::critical(0,"FreeMat BLAS Selector",
			  QString("You must select one of the BLAS libraries listed that worked (second column says 'Yes') \n preferrably with the highest estimated MFLOPS \n (Mega-FLoating-Operations-Per-Seconds)"));
  if (QMessageBox::question(this,"Verify BLAS Selection",
			    QString("Do you want to install:\n") + "   " + sel.at(0)->text() + "\n"
			    + "as the BLAS library for FreeMat?",
			    QMessageBox::Yes, QMessageBox::No, QMessageBox::NoButton) != 
      QMessageBox::Yes)
    return;
  QFile::copy(sel.at(0)->text(),
	      qApp->applicationDirPath()+"/blas.dll");
}

SingleSelectTable::SingleSelectTable(int rows, int cols) : QTableWidget(rows,cols) {
}


void SingleSelectTable::forceSingleSelect() {
  QList<QTableWidgetItem *> sel(selectedItems());
  if (sel.empty()) return;
  QModelIndex mind(indexFromItem(sel.first()));
  QTableWidgetSelectionRange rnge(mind.row(),0,mind.row(),2);
  disconnect(this,SIGNAL(itemSelectionChanged()),this,SLOT(forceSingleSelect()));
  clearSelection();
  setRangeSelected(rnge,true);
  connect(this,SIGNAL(itemSelectionChanged()),this,SLOT(forceSingleSelect()));
}

//layout:
// vlist(libname, timing)
// buttons(addlib, runtiming, selectlib, close)
TunerWidget::TunerWidget() : QWidget() {
  QVBoxLayout *v_layout = new QVBoxLayout;
  m_table = new SingleSelectTable(0,3);
  connect(m_table,SIGNAL(itemSelectionChanged()),m_table,SLOT(forceSingleSelect()));
  m_table->setHorizontalHeaderLabels(QStringList() << "Library" << "Works?" << "Est. MFLOPs");
  v_layout->addWidget(m_table);
  QHBoxLayout *h_layout = new QHBoxLayout;
  QPushButton *button;
  button = new QPushButton("Add libraries");
  connect(button,SIGNAL(clicked()),this,SLOT(addlibs()));
  h_layout->addWidget(button);
  button = new QPushButton("Run timings");
  connect(button,SIGNAL(clicked()),this,SLOT(runtime()));
  h_layout->addWidget(button);
  button = new QPushButton("Choose library");
  connect(button,SIGNAL(clicked()),this,SLOT(selectlib()));
  h_layout->addWidget(button);
  button = new QPushButton("Quit");
  connect(button,SIGNAL(clicked()),qApp,SLOT(quit()));
  h_layout->addWidget(button);
  QWidget *buttonpane = new QWidget;
  buttonpane->setLayout(h_layout);
  v_layout->addWidget(buttonpane);
  setLayout(v_layout);
}

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  TunerWidget *m_main = new TunerWidget;
  m_main->show();
  return app.exec();
}
