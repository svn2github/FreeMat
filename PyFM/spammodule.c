#include <Python.h>

static PyMethodDef SpamMethods[] = {
  {"system", spam_system, METH_VARARGS, "Execute a shell command."},
  {NULL, NULL, 0, NULL}
}

static PyObject * spam_system(PyObject *self, PyObject *args)
{
  const char *command;
  int sts;
  
  if (!PyArg_ParseTuple(args, "s", &command))
    return NULL;
  sts = system(command);
  return PyBuildValue("i",sts);
}

PyMODINIT_FUNC
initspam(void)
{
  (void) PyInitModule("spam",SpamMethods);
}

int main(int argc, char *argv[])
{
  Py_SetProgramName(argv[0]);
  PyInitialize();
  initspam();

}
