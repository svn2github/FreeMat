// Test out the feasibility of doing a generic package building tool
// for FreeMat.  In general, downloading of files, and file manipulation
// and macro definitions are straightforward with Qt.  The tricky
// bit appears to be dealing with archives (.zip and .tar.gz).

#include <QtCore>
#include <QtScript>
#include <QtNetwork>
#include <QtGui>

QTextBrowser *console;
static bool shouldErase = false;
static bool wantsToQuit;

void OutputConsoleText(QString txt) {
  txt.replace("\r\n","\n");
  for (int i=0;i<txt.size();i++) {
    if (txt.at(i) == '\r') {
      console->moveCursor(QTextCursor::StartOfLine);
      shouldErase = true;
    } else if (txt.at(i) == '\n') {
      console->append("");
      shouldErase = false;
    } else {
      if (shouldErase) {
	QTextCursor cursor(console->textCursor());
	cursor.movePosition(QTextCursor::EndOfLine,
			    QTextCursor::KeepAnchor);
	
	cursor.removeSelectedText();
	shouldErase = false;
      }
      console->textCursor().insertText(txt.at(i));
    }
  }
}

static QScriptValue qtscript_sys(QScriptContext *ctx, QScriptEngine *eng) {
  QString cmd = ctx->argument(0).toString();
  QProcess toRun;
#ifdef Q_OS_WIN32
  char shellCmd[_MAX_PATH];
  if( !GetEnvironmentVariable("ComSpec", shellCmd, _MAX_PATH) )
    throw Exception("Unable to find command shell!");
  cmd = QString(shellCmd) + " /a /c " + QString(cmd);
#else
  cmd = QString("sh -c \"") + cmd + QString("\"");
#endif
  OutputConsoleText(cmd + "\n");
  toRun.start(cmd);
  if (!toRun.waitForStarted())
    return QScriptValue(eng,-1);
  toRun.closeWriteChannel();
  while (!toRun.waitForFinished(100)) {
    OutputConsoleText(toRun.readAllStandardOutput());
    OutputConsoleText(toRun.readAllStandardError());
    qApp->processEvents();
  }
  OutputConsoleText(toRun.readAllStandardOutput());
  OutputConsoleText(toRun.readAllStandardError());
  return QScriptValue(eng,toRun.exitCode());
}

static QScriptValue qtscript_disp(QScriptContext *ctx, QScriptEngine *eng) {
  QString str = ctx->argument(0).toString();
  OutputConsoleText(str);
  qApp->processEvents();
  return eng->undefinedValue();
}

static QScriptValue qtscript_fetch(QScriptContext *ctx, QScriptEngine *eng) {
  QScriptValue url = ctx->argument(0);
  QScriptValue filename = ctx->argument(1);
  QScriptValue timeout = ctx->argument(2);

  //   curl = curl_easy_init();
  //   if (curl) {
  //     curl_easy_setopt(curl, CURLOPT_URL, url.toString().toAscii().constData());
  //     curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
  //     res = curl_easy_perform(curl);
  //     curl_easy_cleanup(curl);
  //   }
  //   QFile file(filename.toString());
  //   if (!file.open(QIODevice::WriteOnly))
  //     throw QString("unable to open output file ") + filename.toString();
  //   URLRetriever p_url(QUrl(url.toString()),&file,timeout.toNumber());
  //   p_url.run();
  //   return QScriptValue(eng,!p_url.error());
  return QScriptValue(eng,false);
}

static QScriptValue qtscript_quit(QScriptContext *, QScriptEngine *eng) {
  wantsToQuit = true;
  return eng->undefinedValue();
}

static void interactive(QScriptEngine &eng)
{
  QScriptValue global = eng.globalObject();
  QScriptValue quitFunction = eng.newFunction(qtscript_quit);
  QScriptValue fetchFunction = eng.newFunction(qtscript_fetch);
  if (!global.property(QLatin1String("exit")).isValid())
    global.setProperty(QLatin1String("exit"), quitFunction);
  if (!global.property(QLatin1String("quit")).isValid())
    global.setProperty(QLatin1String("quit"), quitFunction);
  if (!global.property(QLatin1String("fetch")).isValid())
    global.setProperty(QLatin1String("fetch"), fetchFunction);
  wantsToQuit = false;

  QTextStream qin(stdin, QFile::ReadOnly);

  const char *qscript_prompt = "qs> ";
  const char *dot_prompt = ".... ";
  const char *prompt = qscript_prompt;

  QString code;

  forever {
    QString line;

    printf("%s", prompt);
    fflush(stdout);

    line = qin.readLine();
    if (line.isNull())
      break;

    code += line;
    code += QLatin1Char('\n');

    if (line.trimmed().isEmpty()) {
      continue;

    } else if (! eng.canEvaluate(code)) {
      prompt = dot_prompt;

    } else {
      QScriptValue result = eng.evaluate(code, QLatin1String("typein"));

      code.clear();
      prompt = qscript_prompt;

      if (! result.isUndefined())
	fprintf(stderr, "%s\n", qPrintable(result.toString()));

      if (wantsToQuit)
	break;
    }
  }
}

void RegisterFunction(QString name, QScriptEngine &eng, 
		      QScriptEngine::FunctionSignature fun) {
  QScriptValue global(eng.globalObject());
  QScriptValue ffun = eng.newFunction(fun);
  if (!global.property(name).isValid()) global.setProperty(name, ffun);
}

int main(int argc, char *argv[])
{
  QApplication qapp(argc,argv);
  if (argc < 2) {
    return 1;
  }
  QString filename(argv[1]);
  QScriptEngine eng;
  RegisterFunction("disp",eng,qtscript_disp);
  RegisterFunction("sys",eng,qtscript_sys);  
  RegisterFunction("quit",eng,qtscript_quit);

  console = new QTextBrowser;
  console->setFont(QFont("Courier",10));
  console->resize(640,480);
  console->show();

  QFile file(filename);
  QString contents;
  if (file.open(QFile::ReadOnly)) {
    QTextStream stream(&file);
    contents = stream.readAll();
    file.close();
  }
  if (contents.isEmpty()) 
    return qapp.exec();
  QScriptValue r = eng.evaluate(contents, filename);
  if (eng.hasUncaughtException()) {
    QStringList backtrace(eng.uncaughtExceptionBacktrace());
    OutputConsoleText(r.toString() + "\n" + 
		      backtrace.join("\n"));
  }
  //  interactive(eng);
  return qapp.exec();
  //   QScriptEngine eng;
  //   qDebug() << "starting interpreter...";
  //   interactive(eng);
 }
