
/*
  Checks the po file for translation statistics
  To compile it you need the gettext package installed
  with its development files.
  Compilation commandline:

  gcc -lgettextpo -o postats postats.c

*/

/* #include <stdio.h> */
/* #include <stdlib.h> */
/* #include <string.h> */
/* #include <sys/types.h> */
/* #include <unistd.h> */
#include <Python.h>
#include <gettext-po.h>

int translated = 0;
int fuzzy = 0;
int untranslated = 0;
int total = 0;

#define TRUE 1
#define FALSE 0

int po_message_is_translated(po_message_t m)
{
  if (po_message_msgid_plural(m) == NULL)
    return po_message_msgstr(m)[0] != '\0';	
  else
    {
      int i;

      for (i = 0; ; i++)
	{
	  const char *str_i = po_message_msgstr_plural(m, i);
	  if (str_i == NULL)
	    break;
	  if (str_i[0] == '\0')
	    return FALSE;
	}

      return TRUE;
    }
}

static void process_message(po_message_t m)
{
  //	const char * msgstr = po_message_msgstr(m);
  const char * msgid = po_message_msgid(m);

  if(msgid[0] == '\0')return;
  /*	if(strlen(msgid) < 1)return;
	if(strlen(msgstr) < 1)return;
	if(msgstr[0] == '\0')return;
  */
  total++;
  if (po_message_is_fuzzy(m))
    fuzzy++;
  else if (po_message_is_translated(m))
    translated++;
  else {
    untranslated++;
  }

  //printf("%s\n%s\n",msgid,msgstr);
}

static void process_messages(po_message_iterator_t it)
{
  po_message_t m = po_next_message(it);
  while(m)
    {
      process_message(m);
      m = po_next_message(it);
    }
}

void foo1(int severity,
	  po_message_t message,
	  const char *filename, size_t lineno, size_t column,
	  int multiline_p, const char *message_text)
{
  return;
}
void foo2(int severity,
	  po_message_t message1,
	  const char *filename1, size_t lineno1, size_t column1,
	  int multiline_p1, const char *message_text1,
	  po_message_t message2,
	  const char *filename2, size_t lineno2, size_t column2,
	  int multiline_p2, const char *message_text2)
{
  return;
}
static PyObject *
stats(PyObject *self, PyObject *args)
{
  po_file_t po;
  po_message_iterator_t it;
  const char * const * domains;
  const char * filename;
  PyObject * result;
	
  if (!PyArg_ParseTuple(args, "s", &filename)) return NULL;

  struct po_xerror_handler gettext_error_handler = {
    foo1,
    foo2
  };
  po = po_file_read(filename, &gettext_error_handler);

  if(!po)
    {
      return NULL;
    }

  domains = po_file_domains(po);

  if(!domains)
    {
      return NULL;
    }

  while(*domains)
    {
      it = po_message_iterator(po,*domains);
      process_messages(it);
      po_message_iterator_free(it);
      domains++;
    }

  po_file_free(po);

  result = Py_BuildValue("[iiii]", total, translated, fuzzy, untranslated);
  
  /* reset counters, small hack until we use a struct */
  total = translated = fuzzy, untranslated = 0;

  return result;
}

static PyMethodDef POMethods[] = {
	{"stats", stats, METH_VARARGS, "Caclulate po statistics."},
	{NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC
initpo(void){
	(void) Py_InitModule("po", POMethods);
}

