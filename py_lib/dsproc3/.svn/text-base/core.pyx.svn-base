#cython: embedsignature=True
# file dsproc3.pyx

import time

# Cython modules
from libc.stdlib cimport malloc,free
from cpython.ref cimport PyObject
from cpython.string cimport PyString_AsString

cdef extern from *:
    int PY_VERSION_HEX
    int PY_MAJOR_VERSION
    int PY_MINOR_VERSION
    char* __FILE__
    int __LINE__

# TODO: This is forcing python 3.1 or earlier i.e. does not work with 3.2
cdef extern from "Python.h":
    PyObject* PyCObject_FromVoidPtr(void *cobj, void (*destr)(void *))
# The stdio.pxd provided by Cython doesn't have this
cdef extern from *:
    ctypedef char const_char "const char"

# our modules
cimport cds3.core
from dsproc3.cdsproc3 cimport *
cimport dsproc3.cdsproc3_enums as enums

# extra C types not made available by Cython
cdef extern from "sys/types.h" nogil:
    ctypedef int time_t

# extra C types from ARM headers
cdef extern from "messenger.h" nogil:
    int msngr_debug_level
    int msngr_provenance_level

# python modules
import inspect
import sys

# numpy
import numpy as np
cimport numpy as np
np.import_array()

# convenient shorthand used in debug functions
LIB_NAME = enums.DSPROC_LIB_NAME

cdef class PyVarTarget:
    """PyVarTarget"""

    def __cinit__(self):
        """TODO"""
        self.cobj = NULL

    def __init__(self):
        """TODO"""
        pass

    cdef set_vartarget(self, VarTarget *obj):
        """TODO"""
        self.cobj = obj

    def __dealloc__(self):
        """TODO"""
        pass

cdef inline __line():
    """Returns caller's (filename,lineno,function)."""
    cdef char *filename
    cdef char *function
    cdef int lineno
    frame = inspect.currentframe()
    #if frame.f_back:
    #    frame = frame.f_back
    #if frame.f_back:
    #    frame = frame.f_back
    #if frame.f_back:
    #    frame = frame.f_back
    filename = frame.f_code.co_filename
    lineno = frame.f_lineno
    function = frame.f_code.co_name
    del frame
    return filename, lineno, function

#def LOG(char *format, *args, char *_file="", int _line=0):
def log(char *format, *args):
    s=format
    file,line,func=__line()
    if args:
        try:
            s = format % args
        except TypeError:
            s = str((format,args))
    dsproc_log(func, file, line, s)


#def WARNING(char *format, *args, char *_file="", int _line=0):
def warning(char *format, *args):
    s=format
    file,line,func=__line()
    if args:
        try:
            s = format % args
        except TypeError:
            s = str((format,args))
    dsproc_warning(func, file, line, s)

#def ERROR(char *status, char *format, *args, char *_file="", int _line=0):
def error(char *status, char *format, *args):
    s=format
    file,line,func=__line()
    if args:
        try:
            s = format % args
        except TypeError:
            s = str((format,args))
    dsproc_error(func, file, line, status, s)

def mentor_mail(char *format, *args):
    s=format
    file,line,func=__line()
    if args:
        try:
            s = format % args
        except TypeError:
            s = str((format,args))
    dsproc_mentor_mail(func, file, line, s)

#def debug_lv1(char *format, *args, char *_file="", int _line=0):
def debug_lv1(char *format, *args):
    s=format
    file,line,func=__line()
    #if _file:
    #    file = _file
    #if _line:
    #    line = _line
    if args:
        try:
            s = format % args
        except TypeError:
            s = str((format,args))
    if msngr_debug_level or msngr_provenance_level:
        dsproc_debug(func, file, line, 1, s)

def debug_lv2(char *format, *args):
    s=format
    file,line,func=__line()
    if args:
        try:
            s = format % args
        except TypeError:
            s = str((format,args))
    if msngr_debug_level or msngr_provenance_level:
        dsproc_debug(func, file, line, 2, s)

def debug_lv3(char *format, *args):
    s=format
    file,line,func=__line()
    if args:
        try:
            s = format % args
        except TypeError:
            s = str((format,args))
    if msngr_debug_level or msngr_provenance_level:
        dsproc_debug(func, file, line, 3, s)

def debug_lv4(char *format, *args):
    s=format
    file,line,func=__line()
    if args:
        try:
            s = format % args
        except TypeError:
            s = str((format,args))
    if msngr_debug_level or msngr_provenance_level:
        dsproc_debug(func, file, line, 4, s)

def debug_lv5(char *format, *args):
    s=format
    file,line,func=__line()
    if args:
        try:
            s = format % args
        except TypeError:
            s = str((format,args))
    if msngr_debug_level or msngr_provenance_level:
        dsproc_debug(func, file, line, 5, s)

##############################################################################
# ported from dsproc_hooks.c
##############################################################################
_user_data = None
_init_process_hook = None
_finish_process_hook = None
_process_data_hook = None
_pre_retrieval_hook = None
_post_retrieval_hook = None
_pre_transform_hook = None
_post_transform_hook = None
_process_file_hook = None

def set_init_process_hook(hook):
    """Set hook function to call when a process is first initialized.
    
    This function must be called from the main function before dsproc_main()
    is called.
    
    The specified init_process_hook() function will be called once just before
    the main data processing loop begins and before the initial database
    connection is closed.
    
    The init_process_hook() function does not take any arguments, but it must
    return:
      - a user defined data structure or value (a Python object) that will be
        passed in as user_data to all other hook functions.
      - 1 if no user data is returned.
      - None if a fatal error occurred and the process should exit.
    
    @param hook - function to call when the process is initialized

    """
    global _init_process_hook
    _init_process_hook = hook

def set_finish_process_hook(hook):
    """Set hook function to call before a process finishes.
    
    This function must be called from the main function before dsproc_main()
    is called, or from the init_process_hook() function.
    
    The specified finish_process_hook() function will be called once just after
    the main data processing loop finishes.  This function should be used to
    clean up any temporary files used, and to free any memory used by the
    structure returned by the init_process_hook() function.
    
    The finish_process_hook function must take the following argument:
    
      - object  user_data:  value returned by the init_process_hook() function
    
    @param hook - function to call before the process finishes

    """
    global _finish_process_hook
    _finish_process_hook = hook

def set_process_data_hook(hook):
    """Set the main data processing function.
    
    This function must be called from the main function before dsproc_main()
    is called, or from the init_process_hook() function.
    
    The specified process_data_hook function will be called once per
    processing interval just after the output datasets are created, but
    before they are stored to disk.
    
    The process_data_hook function must take the following arguments:
    
      - object user_data:  value returned by the init_process_hook() function
      - time_t begin_date: the begin time of the current processing interval
      - time_t end_date:   the end time of the current processing interval
      - Group  input_data: the parent CDS.Group containing the input data.
    
    And must return:
    
      -  1 if processing should continue normally
      -  0 if processing should skip the current processing interval
           and continue on to the next one.
      - -1 if a fatal error occurred and the process should exit.
    
    @param hook - the main data processing function
    
    """
    global _process_data_hook
    _process_data_hook = hook

def set_pre_retrieval_hook(hook):
    """Set hook function to call before data is retrieved.
    
    This function must be called from the main function before dsproc_main()
    is called, or from the init_process_hook() function.
    
    The specified pre_retrieval_hook function will be called once per
    processing interval just prior to data retrieval, and must take the
    following arguments:
    
    The pre_retrieval_hook function must take the following arguments:
    
      - object  user_data:  value returned by the init_process_hook() function
      - time_t  begin_date: the begin time of the current processing interval
      - time_t  end_date:   the end time of the current processing interval
    
    And must return:
    
      -  1 if processing should continue normally
      -  0 if processing should skip the current processing interval
           and continue on to the next one.
      - -1 if a fatal error occurred and the process should exit.
    
    @param hook - function to call before data is retrieved

    """
    global _pre_retrieval_hook
    _pre_retrieval_hook = hook

def set_post_retrieval_hook(hook):
    """Set hook function to call after data is retrieved.
    
    This function must be called from the main function before dsproc_main()
    is called, or from the init_process_hook() function.
    
    The specified post_retrieval_hook function will be called once per
    processing interval just after data retrieval, but before the retrieved
    observations are merged and QC is applied.
    
    The post_retrieval_hook function must take the following arguments:
    
      - object user_data:  value returned by the init_process_hook() function
      - time_t begin_date: the begin time of the current processing interval
      - time_t end_date:   the end time of the current processing interval
      - Group  ret_data:   the parent CDS.Group containing the retrieved data
    
    And must return:
    
      -  1 if processing should continue normally
      -  0 if processing should skip the current processing interval
           and continue on to the next one.
      - -1 if a fatal error occurred and the process should exit.
    
    @param hook - function to call after data is retrieved

    """
    global _post_retrieval_hook
    _post_retrieval_hook = hook

def set_pre_transform_hook(hook):
    """Set hook function to call before the data is transformed.
    
    This function must be called from the main function before dsproc_main()
    is called, or from the init_process_hook() function.
    
    The specified pre_transform_hook function will be called once per
    processing interval just prior to data transformation, and after
    the retrieved observations are merged and QC is applied.
    
    The pre_transform_hook function must take the following arguments:
    
      - object user_data:  value returned by the init_process_hook() function
      - time_t begin_date: the begin time of the current processing interval
      - time_t end_date:   the end time of the current processing interval
      - Group  ret_data:   the parent CDS.Group containing the retrieved data
    
    And must return:
    
      -  1 if processing should continue normally
      -  0 if processing should skip the current processing interval
           and continue on to the next one.
      - -1 if a fatal error occurred and the process should exit.
    
    @param hook - function to call before the data is transformed
    
    """
    global _pre_transform_hook
    _pre_transform_hook = hook

def set_post_transform_hook(hook):
    """Set hook function to call after the data is transformed.
    
    This function must be called from the main function before dsproc_main()
    is called, or from the init_process_hook() function.
    
    The specified post_transform_hook function will be called once per
    processing interval just after data transformation, but before the
    output datasets are created.
    
    The post_transform_hook function must take the following arguments:
    
      - object user_data:  value returned by the init_process_hook() function
      - time_t begin_date: the begin time of the current processing interval
      - time_t end_date:   the end time of the current processing interval
      - Group  trans_data: the parent CDS.Group containing the transformed data
    
    And must return:
    
      -  1 if processing should continue normally
      -  0 if processing should skip the current processing interval
           and continue on to the next one.
      - -1 if a fatal error occurred and the process should exit.
    
    @param hook - function to call after the data is transformed
    
    """
    global _post_transform_hook
    _post_transform_hook = hook

def set_process_file_hook(hook):
    """Set the main file processing function.
    
    This function must be called from the main function before dsproc_main()
    is called, or from the init_process_hook() function.
    
    The specified process_file_hook function will be called once for
    for every file found in the input direcotyr, and it must take the
    following arguments:
    
      - object user_data:  value returned by the init_process_hook() function
      - char   input_dir:  the full path to the input directory
      - char   file_name:  the name of the file to process
    
    And must return:
    
      -  1 if processing should continue normally
      -  0 if processing should skip the file
           and continue on to the next one.
      - -1 if a fatal error occurred and the process should exit.
    
    @param hook - the main file processing function
    
    """
    global _process_file_hook
    _process_file_hook = hook

def _run_init_process_hook():
    """Private: Run the init_process_hook function.
    
    @return
      -  1 if successful
      -  0 if an error occurred.
    
    """
    global _user_data
    cdef const char *status_text
    if _init_process_hook:
        debug_lv1("\n----- ENTERING INIT PROCESS HOOK -------\n")
        _user_data = _init_process_hook()
        if not _user_data:
            status_text = dsproc_get_status()
            if len(status_text) == 0 or status_text.startswith('\0'):
                error("Unknown Data Processing Error (check logs)","Error message not set by init_process_hook function\n")
            return 0
        debug_lv1("----- EXITING INIT PROCESS HOOK --------\n\n")
    return 1

def _run_finish_process_hook():
    """Private: Run the finish_process_hook function."""
    if _finish_process_hook:
        debug_lv1("\n----- ENTERING FINISH PROCESS HOOK -----\n")
        _finish_process_hook(_user_data)
        debug_lv1("----- EXITING FINISH PROCESS HOOK ------\n\n")

def _run_process_data_hook(
        time_t begin_date,
        time_t end_date,
        object input_data):
    """Private: Run the process_data_hook function.
    
    @param begin_date - the begin time of the current processing interval
    @param end_date   - the end time of the current processing interval
    @param input_data - the parent CDS.Group containing the input data
    
    @return
      -  1 if processing should continue normally
      -  0 if processing should skip the current processing interval
           and continue on to the next one.
      - -1 if a fatal error occurred and the process should exit.
    
    """
    cdef const char *status_text
    cdef int   status
    status = 1
    if _process_data_hook:
        debug_lv1("\n----- ENTERING PROCESS DATA HOOK -------\n")
        status = _process_data_hook(
                _user_data, begin_date, end_date, input_data)
        if status < 0:
            status_text = dsproc_get_status()
            if len(status_text) == 0 or status_text.startswith('\0'):
                error("Unknown Data Processing Error (check logs)","Error message not set by process_data_hook function\n")
        debug_lv1("----- EXITING PROCESS DATA HOOK --------\n\n")

    return status

def _run_pre_retrieval_hook(
        time_t   begin_date,
        time_t   end_date):
    """Private: Run the pre_retrieval_hook function.
    
    @param begin_date - the begin time of the current processing interval
    @param end_date   - the end time of the current processing interval
    
    @return
      -  1 if processing should continue normally
      -  0 if processing should skip the current processing interval
           and continue on to the next one.
      - -1 if a fatal error occurred and the process should exit.
    
    """
    cdef const char *status_text
    cdef int   status
    status = 1
    if _pre_retrieval_hook:
        debug_lv1("\n----- ENTERING PRE-RETRIEVAL HOOK ------\n")
        status = _pre_retrieval_hook(_user_data, begin_date, end_date)
        if status < 0:
            status_text = dsproc_get_status()
            if len(status_text) == 0 or status_text.startswith('\0'):
                error("Unknown Data Processing Error (check logs)","Error message not set by pre_retrieval_hook function\n")
        debug_lv1("----- EXITING PRE-RETRIEVAL HOOK -------\n")

    return status

def _run_post_retrieval_hook(
        time_t begin_date,
        time_t end_date,
        object ret_data):
    """Private: Run the post_retrieval_hook function.
    
    @param begin_date - the begin time of the current processing interval
    @param end_date   - the end time of the current processing interval
    @param ret_data   - the parent CDS.Group containing the retrieved data 
    @return
      -  1 if processing should continue normally
      -  0 if processing should skip the current processing interval
           and continue on to the next one.
      - -1 if a fatal error occurred and the process should exit.
    
    """
    cdef const char *status_text
    cdef int   status
    status = 1
    if _post_retrieval_hook:
        debug_lv1("\n----- ENTERING POST-RETRIEVAL HOOK -----\n")
        status = _post_retrieval_hook(
            _user_data, begin_date, end_date, ret_data)
        if status < 0:
            status_text = dsproc_get_status()
            if len(status_text) == 0 or status_text.startswith('\0'):
                error("Unknown Data Processing Error (check logs)","Error message not set by post_retrieval_hook function\n")
        debug_lv1("----- EXITING POST-RETRIEVAL HOOK ------\n\n")

    return status

def _run_pre_transform_hook(
        time_t begin_date,
        time_t end_date,
        object ret_data):
    """Private: Run the pre_transform_hook function.
    
    @param begin_date - the begin time of the current processing interval
    @param end_date   - the end time of the current processing interval
    @param ret_data   - the parent CDS.Group containing the retrieved data 
    @return
      -  1 if processing should continue normally
      -  0 if processing should skip the current processing interval
           and continue on to the next one.
      - -1 if a fatal error occurred and the process should exit.
    
    """
    cdef const char *status_text
    cdef int   status
    status = 1
    if _pre_transform_hook:
        debug_lv1("\n----- ENTERING PRE-TRANSFORM HOOK ------\n")
        status = _pre_transform_hook(
            _user_data, begin_date, end_date, ret_data)
        if status < 0:
            status_text = dsproc_get_status()
            if len(status_text) == 0 or status_text.startswith('\0'):
                error("Unknown Data Processing Error (check logs)","Error message not set by pre_transform_hook function\n")
        debug_lv1("----- EXITING PRE-TRANSFORM HOOK -------\n\n")

    return status

def _run_post_transform_hook(
        time_t begin_date,
        time_t end_date,
        object trans_data):
    """Private: Run the post_transform_hook function.
    
    @param begin_date - the begin time of the current processing interval
    @param end_date   - the end time of the current processing interval
    @param trans_data - the parent CDS.Group containing the transformed data
    
    @return
      -  1 if processing should continue normally
      -  0 if processing should skip the current processing interval
           and continue on to the next one.
      - -1 if a fatal error occurred and the process should exit.
    
    """
    cdef const char *status_text
    cdef int   status
    status = 1
    if _post_transform_hook:
        debug_lv1("\n----- ENTERING POST-TRANSFORM HOOK -----\n")
        status = _post_transform_hook(
            _user_data, begin_date, end_date, trans_data)
        if status < 0:
            status_text = dsproc_get_status()
            if len(status_text) == 0 or status_text.startswith('\0'):
                error("Unknown Data Processing Error (check logs)","Error message not set by post_transform_hook function\n")
        debug_lv1("----- EXITING POST-TRANSFORM HOOK ------\n\n")

    return status

def _run_process_file_hook(
        char *input_dir,
        char *file_name):
    """Private: Run the process_file_hook function.
    
    @param input_dir   - full path to the input directory
    @param file_name   - name of the file to process
    
    @return
      -  1 if processing should continue normally
      -  0 if processing should skip the current file
           and continue on to the next one.
      - -1 if a fatal error occurred and the process should exit.
    
    """
    cdef const char *status_text
    cdef int   status
    status = 1
    if _process_file_hook:
        debug_lv1("\n----- ENTERING PROCESS FILE HOOK -----\n")
        status = _process_file_hook(
            _user_data, input_dir, file_name)
        if status < 0:
            status_text = dsproc_get_status()
            if len(status_text) == 0 or status_text.startswith('\0'):
                error("Unknown Data Processing Error (check logs)","Error message not set by process_file_hook function\n")
        debug_lv1("----- EXITING PROCESS FILE HOOK ------\n\n")

    return status
##############################################################################
# ported from dsproc3.h
##############################################################################

# helper function for freeing memory allocated by libdsproc3
cdef void _free(void *address):
    free(address)

cdef inline void* _alloc_single(CDSDataType type, object initial_value=None):
    """Allocates a single value of the given type and return its address."""
    cdef void *retval = NULL
    if type == CDS_NAT:
        raise ValueError, "CDS_NAT"
    elif type == CDS_CHAR:
        retval = malloc(sizeof(char))
        if initial_value is not None:
            (<char*>retval)[0] = initial_value
    elif type == CDS_BYTE:
        retval = malloc(sizeof(char))
        if initial_value is not None:
            (<char*>retval)[0] = initial_value
    elif type == CDS_SHORT:
        retval = malloc(sizeof(short))
        if initial_value is not None:
            (<short*>retval)[0] = initial_value
    elif type == CDS_INT:
        retval = malloc(sizeof(int))
        if initial_value is not None:
            (<int*>retval)[0] = initial_value
    elif type == CDS_FLOAT:
        retval = malloc(sizeof(float))
        if initial_value is not None:
            (<float*>retval)[0] = initial_value
    elif type == CDS_DOUBLE:
        retval = malloc(sizeof(double))
        if initial_value is not None:
            (<double*>retval)[0] = initial_value
    else:
        raise ValueError, "Unknown CDSDataType %s" % type
    return retval

cdef inline object _convert_single(CDSDataType type, void *value):
    """Converts a single value at the given address to the given type
    and frees it."""
    cdef object retval
    if type == CDS_NAT:
        raise ValueError, "CDS_NAT"
    elif type == CDS_CHAR:
        retval = (<char*>value)[0];
        free(<char*>value)
    elif type == CDS_BYTE:
        retval = (<char*>value)[0];
        free(<char*>value)
    elif type == CDS_SHORT:
        retval = (<short*>value)[0];
        free(<short*>value)
    elif type == CDS_INT:
        retval = (<int*>value)[0];
        free(<int*>value)
    elif type == CDS_FLOAT:
        retval = (<float*>value)[0];
        free(<float*>value)
    elif type == CDS_DOUBLE:
        retval = (<double*>value)[0];
        free(<double*>value)
    else:
        raise ValueError, "Unknown CDSDataType %s" % type
    return retval

cdef inline int cds_type_to_dtype(CDSDataType type) except -1:
# Use np.NPY_<type> when calling into numpy C interface
# i.e. PyArray
    if type == CDS_NAT:
        raise ValueError, "CDS_NAT"
    elif type == CDS_CHAR:
        return np.NPY_STRING
    elif type == CDS_BYTE:
        return np.NPY_BYTE
    elif type == CDS_SHORT:
        return np.NPY_SHORT
    elif type == CDS_INT:
        return np.NPY_INT
    elif type == CDS_FLOAT:
        return np.NPY_FLOAT
    elif type == CDS_DOUBLE:
        return np.NPY_DOUBLE
    else:
        raise ValueError, "Unknown CDSDataType %s" % type

cdef inline np.dtype cds_type_to_dtype_obj(CDSDataType type):
# Use np.dtype<type> when calling into numpy Python interface
    """Converts a CDSDataType to a dtype instance."""
    if type == CDS_NAT:
        raise ValueError, "CDS_NAT"
    elif type == CDS_CHAR:
        return np.dtype(np.uint8)
    elif type == CDS_BYTE:
        return np.dtype(np.int8)
    elif type == CDS_SHORT:
        return np.dtype(np.int16)
    elif type == CDS_INT:
        return np.dtype(np.int32)
    elif type == CDS_FLOAT:
        return np.dtype(np.float32)
    elif type == CDS_DOUBLE:
        return np.dtype(np.float64)
    else:
        raise ValueError, "Unknown CDSDataType %s" % type


cdef inline int dtype_to_cds_type(np.dtype type) except -1:
    if type == np.dtype(np.uint8):
        return CDS_CHAR
    elif type == np.dtype('S1'):
        return CDS_CHAR
    elif type == np.dtype(np.str):
        return CDS_CHAR
    elif type == np.dtype(np.int8):
        return CDS_BYTE
    elif type == np.dtype(np.int16):
        return CDS_SHORT
    elif type == np.dtype(np.int32):
        return CDS_INT
    elif type == np.dtype(np.float32):
        return CDS_FLOAT
    elif type == np.dtype(np.float64):
        return CDS_DOUBLE
    else:
        raise ValueError, "Unknown dtype for CDSDataType %s" % type

def get_debug_level():
    """Get the current debug level."""
    return dsproc_get_debug_level()

def get_site():
    """Get the process site."""
    return dsproc_get_site()

def get_facility():
    """Get the process facility."""
    return dsproc_get_facility()

def get_name():
    """Get the process name."""
    return dsproc_get_name()

def get_datastream_id(
        char *site,
        char *facility,
        char *dsc_name,
        char *dsc_level,
        enums.DSRole role):
    """Get the ID of a datastream.
    
    @param site - site name, or NULL to find first match
    @param facility - facility name, or NULL to find first match
    @param dsc_name - datastream class name
    @param dsc_level - datastream class level
    @param role - specifies input or output datastream
    
    @retval datastream ID
    @retval -1 if the datastream has not beed defined

    """
    return dsproc_get_datastream_id(site, facility, dsc_name, dsc_level, role)

def get_input_datastream_id(char *dsc_name, char *dsc_level):
    """Get the ID of an input datastream.
    
    This function will generate an error if the specified datastream class has
    not been defined in the database as an input for this process.
    
    If an error occurs in this function it will be appended to the log and
    error mail messages, and the process status will be set appropriately.
    
    @param dsc_name - datastream class name
    @param dsc_level - datastream class level
    
    @retval datastream ID
    @retval -1 if an error occurs

    """
    return dsproc_get_input_datastream_id(dsc_name, dsc_level)

def get_input_datastream_ids():
    """Get the IDs of all input datastreams.
    
    This function will return an array of all input datastream ids. The memory
    used by the returned array is dynamically allocated and will be freed when
    the returned ndarray goes out of scope.
    
    If an error occurs in this function it will be appended to the log and
    error mail messages, and the process status will be set appropriately.
    
    @retval ndarray of datastream IDs
    @raise MemoryError if memory allocation error occurs

    """
    cdef int *ids
    cdef int count
    cdef np.npy_intp dims
    cdef np.ndarray[np.int32_t, ndim=1] ids_nd
    count = dsproc_get_input_datastream_ids(&ids)
    if count == -1:
        raise MemoryError
    if count == 0: # TODO: does this happen?
        return None
    dims = count
    ids_nd = np.PyArray_SimpleNewFromData(1, &dims, np.NPY_INT32, ids)
    # allow numpy to reclaim memory when array goes out of scope
    ids_nd.base = PyCObject_FromVoidPtr(ids, _free)
    return ids_nd

def get_output_datastream_id(char *dsc_name, char *dsc_level):
    """Get the ID of an output datastream.
    
    This function will generate an error if the specified datastream class
    has not been defined in the database as an output for this process.
    
    If an error occurs in this function it will be appended to the log and
    error mail messages, and the process status will be set appropriately.
    
    @param dsc_name - datastream class name
    @param dsc_level - datastream class level
    
    @retval datastream ID
    @retval -1 if an error occurs

    """
    return dsproc_get_output_datastream_id(dsc_name, dsc_level)

def get_output_datastream_ids():
    """Get the IDs of all output datastreams.
    
    This function will return an array of all input datastream ids. The memory
    used by the returned array is dynamically allocated and will be freed when
    the returned ndarray goes out of scope.
    
    If an error occurs in this function it will be appended to the log and
    error mail messages, and the process status will be set appropriately.
    
    @retval ndarray of datastream IDs
    @raise MemoryError if memory allocation error occurs

    """
    cdef int *ids
    cdef int count
    cdef np.npy_intp dims
    cdef np.ndarray[np.int32_t, ndim=1] ids_nd
    count = dsproc_get_output_datastream_ids(&ids)
    if count == -1:
        raise MemoryError
    if count == 0: # TODO: does this happen?
        return None
    dims = count
    ids_nd = np.PyArray_SimpleNewFromData(1, &dims, np.NPY_INT32, ids)
    # allow numpy to reclaim memory when array goes out of scope
    ids_nd.base = PyCObject_FromVoidPtr(ids, _free)
    return ids_nd

def datastream_name(int ds_id):
    """Returns the fully qualified datastream name.
    
    @param ds_id - datastream ID
    
    @retval the datastream name
    @retval None if the datastream ID is not valid

    """
    cdef const char *retval = dsproc_datastream_name(ds_id)
    if retval == NULL:
        return None
    return retval

def datastream_path(int ds_id):
    """Returns the path to the datastream directory.
    
    @param ds_id - datastream ID
    
    @retval the path to the datastream directory
    @retval None if the datastream path has not ben set

    """
    cdef const char *retval = dsproc_datastream_path(ds_id)
    if retval == NULL:
        return None
    return retval

def datastream_site(int ds_id):
    """Returns the datastream site.
    
    @param ds_id - datastream ID
    
    @retval the datastream site code
    @retval None if the datastream ID is not valid

    """
    cdef const char *retval = dsproc_datastream_site(ds_id)
    if retval == NULL:
        return None
    return retval

def datastream_facility(int ds_id):
    """ 995  * Returns the datastream facility.
    
    @param ds_id - datastream ID
    
    @retval the datastream facility code
    @retval None if the datastream ID is not valid

    """
    cdef const char *retval = dsproc_datastream_facility(ds_id)
    if retval == NULL:
        return None
    return retval

def datastream_class_name(int ds_id):
    """Returns the datastream class name.
    
    @param ds_id - datastream ID
    
    @retval the datastream class name
    @retval None if the datastream ID is not valid

    """
    cdef const char *retval = dsproc_datastream_class_name(ds_id)
    if retval == NULL:
        return None
    return retval

def datastream_class_level(int ds_id):
    """Returns the datastream class level.
    
    @param ds_id - datastream ID
    
    @retval the datastream class level
    @retval None if the datastream ID is not valid

    """
    cdef const char *retval = dsproc_datastream_class_level(ds_id)
    if retval == NULL:
        return None
    return retval

def dataset_name(cds3.core.Group dataset):
    """Returns the dataset name.
    
    @param dataset pointer to the dataset
    
    @retval name pointer to the dataset name
    @retval None if the specified dataset is None

    """
    cdef const char *retval
    if dataset is None:
        return None
    retval = dsproc_dataset_name(dataset.c_ob)
    if retval == NULL:
        return None
    return retval

def use_nc_extension():
    """ Set the default NetCDF file extension to 
    'nc' for output files. The NetCDF file extension used by
    ARM has historically been "cdf". This function can be used to
    change it to the new prefered extension of "nc", and must be
    called *before* calling dsproc_main().
    """
    dsproc_use_nc_extension()

def get_output_dataset(int ds_id, int obs_index=0):
    """Get an output dataset.
    
    This function will return a cds3.Group of the output dataset for the
    specifed datastream and observation. The obs_index should always be zero
    unless observation based processing is being used. This is because all
    input observations should have been merged into a single observation
    in the output datasets.
    
    @param ds_id output datastream ID
    @param obs_index the index of the obervation to get the dataset for
    
    @retval dataset cds3.Group to the output dataset
    @retval NULL if it does not exist
    
    @b Example: Get the dataset for an output datastream
    @code
    dsc_name = "example"
    dsc_level = "c1"
    ds_id = dsproc.get_output_datastream_id(dsc_name, dsc_level)
    dataset = dsproc.get_output_dataset(ds_id, 0)
    @endcode 

    """
    cdef cds3.core.Group group
    cdef CDSGroup *cds_group = NULL
    cds_group = dsproc_get_output_dataset(ds_id, obs_index)
    if cds_group == NULL:
        return None
    else:
        group = cds3.core.Group()
        group.set_group(cds_group)
        return group

def get_retrieved_dataset(int ds_id, int obs_index=0):
    """Get a retrieved dataset.
    
    This function will return a cds3.Group of the retrieved dataset for the
    specifed datastream and observation.
    
    The obs_index is used to specify which observation to get the dataset for.
    This value will typically be zero unless this function is called from a
    post_retrieval_hook() function, or the process is using observation based
    processing. In either of these cases the retrieved data will contain one
    "observation" for every file the data was read from on disk.
    
    It is also possible to have multiple observations in the retrieved data
    when a pre_transform_hook() is called if a dimensionality conflict
    prevented all observations from being merged.
    
    @param ds_id input datastream ID
    @param obs_index observation index (0 based indexing)
    
    @retval cds3.Group of the retrieved dataset
    @retval None if it does not exist
    
    @b Example: Loop over all retrieved datasets for an input datastream
    @code
    
    dsc_name = "example"
    dsc_level = "b1"
    ds_id = dsproc.get_input_datastream_id(dsc_name, dsc_level)
    obs_index = 0
    dataset = dsproc.get_retrieved_dataset(ds_id, obs_index)
    while dataset is not None:
        obs_index += 1
        dataset = dsproc.get_retrieved_dataset(ds_id, obs_index)
    @endcode
    
    """
    cdef cds3.core.Group group
    cdef CDSGroup *cds_group = NULL
    cds_group = dsproc_get_retrieved_dataset(ds_id, obs_index)
    if cds_group == NULL:
        return None
    else:
        group = cds3.core.Group()
        group.set_group(cds_group)
        return group

def get_transformed_dataset(
        char *coordsys_name,
        int ds_id,
        int obs_index=0):
    """Get a transformed dataset.
    
    This function will return a cds3.Group of the transformed dataset for the
    specifed coordinate system, datastream, and observation. The obs_index
    should always be zero unless observation based processing is being used.
    This is because all input observations should have been merged into a
    single observation in the transformed datasets.
    
    @param coordsys_name the name of the coordinate system as specified in the
    retriever definition or None if a coordinate system name was not specified.
    
    @param ds_id input datastream ID
    
    @param obs_index the index of the obervation to get the dataset for
    
    @retval cds3.Group of the output dataset
    @retval None if it does not exist

    """
    cdef cds3.core.Group group
    cdef CDSGroup *cds_group = NULL
    cds_group = dsproc_get_transformed_dataset(coordsys_name, ds_id, obs_index)
    if cds_group == NULL:
        return None
    else:
        group = cds3.core.Group()
        group.set_group(cds_group)
        return group

def get_dim(cds3.core.Group dataset, char *name):
    """Get a dimension from a dataset.
    
    @param dataset - pointer to the dataset
    @param name - name of the dimension
    
    @retval cds3.Dim of the dimension
    @retval None if the dimension does not exist

    """
    cdef cds3.core.Dim dim
    cdef CDSDim *cds_dim
    cds_dim = dsproc_get_dim(dataset.c_ob, name)
    if cds_dim == NULL:
        return None
    else:
        dim = cds3.core.Dim()
        dim.set_dim(cds_dim)
        return dim

def get_dim_length(cds3.core.Group dataset, char *name):
    """Get the length of a dimension in a dataset.
    
    @param dataset - pointer to the dataset
    @param name - name of the dimension
    
    @retval dimension length
    @retval 0 if the dimension does not exist or has 0 length
    
    """
    return dsproc_get_dim_length(dataset.c_ob, name)

def set_dim_length(cds3.core.Group dataset, char *name, size_t length):
    """Set the length of a dimension in a dataset.
    
    If an error occurs in this function it will be appended to the log and
    error mail messages, and the process status will be set appropriately.
    
    @param dataset - pointer to the dataset
    @param name - name of the dimension
    @param length - new length of the dimension
    
    @retval 1 if successful
    @retval 0 if the dimension does not exist
    @retval 0 if the dimension definition is locked
    @retval 0 if data has already been added to a variable using this dimension

    """
    return dsproc_set_dim_length(dataset.c_ob, name, length)

def change_att(
            cds3.core.Object parent,
            int          overwrite,
            char  *name,
            CDSDataType  type,
            object       value):
    """Change an attribute for a dataset or variable.
    
    This function will define the specified attribute if it does not exist.
    If the attribute does exist and the overwrite flag is set, the data type
    and value will be changed.
    
    If an error occurs in this function it will be appended to the log and
    error mail messages, and the process status will be set appropriately.
    
    @param parent - pointer to the parent CDSGroup or CDSVar
    @param overwrite - overwrite flag (1 = TRUE, 0 = FALSE)
    @param name - attribute name
    @param type - attribute data type
    @param value - pointer to the attribute value
    
    @retval 1 if successful
    @retval 0 if the parent object is not a group or variable
    @retval 0 if the parent group or variable definition is locked
    @retval 0 if the attribute definition is locked
    @retval 0 if a memory allocation error occurred

    """
    # TODO need to used fused types for the 'parent' param and also likely for
    # the conversion of the 'value' param
    cdef np.ndarray value_nd = np.asarray(value, cds_type_to_dtype_obj(type))
    if value_nd.ndim == 0:
        value_nd = value_nd[None] # add dummy dimension to scalar value
    assert value_nd.ndim == 1
    return dsproc_change_att(parent.cds_object, overwrite, name, type,
            len(value_nd), value_nd.data)

def get_att(cds3.core.Object parent, char  *name):
    """Get an attribute from a dataset or variable.
    
    @param parent - the parent cds3.Group or cds3.Var
    @param name - name of the attribute
    
    @retval pointer to the attribute
    @retval None if the attribute does not exist
    
    """
    cdef cds3.core.Att att
    cdef CDSAtt *cds_att
    cds_att = dsproc_get_att(parent.cds_object, name)
    if cds_att == NULL:
        return None
    else:
        att = cds3.core.Att()
        att.set_att(cds_att)
        return att

def get_att_text(cds3.core.Object parent, char *name):
    """Get a copy of an attribute value from a dataset or variable.
    
    This function will get a copy of an attribute value converted to a
    text string. If the data type of the attribute is not CDS_CHAR the
    cds_array_to_string() function is used to create the output string.
    
    If an error occurs in this function it will be appended to the log and
    error mail messages, and the process status will be set appropriately.
    
    @param parent - pointer to the parent CDSGroup or CDSVar
    @param name - name of the attribute
    
    @retval the output string
    @retval None if the attribute does not exist or has zero length
    @retval None if a memory allocation error occurs

    """
    cdef size_t length
    cdef char *text
    cdef object retval
    text = dsproc_get_att_text(parent.cds_object, name, &length, NULL)
    if text == NULL:
        return None
    retval = text
    free(text);
    return retval

def get_att_value(cds3.core.Object parent, char *name, CDSDataType type):
    """Get a copy of an attribute value from a dataset or variable.
    
    This function will get a copy of an attribute value casted into
    the specified data type. The functions cds_string_to_array() and
    cds_array_to_string() are used to convert between text (CDS_CHAR)
    and numeric data types.
    
    If an error occurs in this function it will be appended to the log and
    error mail messages, and the process status will be set appropriately.
    
    @param parent - pointer to the parent CDSGroup or CDSVar
    @param name - name of the attribute
    @param type - data type of the output array
    
    @retval ndarray of the output array
    @retval None if the attribute value has zero length (length == 0)
    @retval None if a memory allocation error occurs (length == (size_t)-1)

    """
    cdef size_t length
    cdef void *value = NULL
    cdef int dtype
    cdef np.ndarray value_nd
    cdef np.npy_intp dims
    value = dsproc_get_att_value(parent.cds_object, name, type, &length, NULL)
    if value == NULL:
        return None
    dtype = cds_type_to_dtype(type)
    dims = length
    value_nd = np.PyArray_SimpleNewFromData(1, &dims, dtype, value)
    # allow numpy to reclaim memory when array goes out of scope
    value_nd.base = PyCObject_FromVoidPtr(value, _free)
    return value_nd

def set_att(cds3.core.Object parent, int overwrite, char *name,
        CDSDataType type, object value):
    """Set the value of an attribute in a dataset or variable.
    
    This function will define the specified attribute if it does not exist.
    If the attribute does exist and the overwrite flag is set, the value will
    be set by casting the specified value into the data type of the attribute.
    The functions cds_string_to_array() and cds_array_to_string() are used to
    convert between text (CDS_CHAR) and numeric data types.
    
    If an error occurs in this function it will be appended to the log and
    error mail messages, and the process status will be set appropriately.
    
    @param parent - pointer to the parent CDSGroup or CDSVar
    @param overwrite - overwrite flag (1 = TRUE, 0 = FALSE)
    @param name - attribute name
    @param type - attribute data type
    @param value - pointer to the attribute value
    
    @retval 1 if successful
    @retval 0 if the parent object is not a group or variable
    @retval 0 if the parent group or variable definition is locked
    @retval 0 if the attribute definition is locked
    @retval 0 if a memory allocation error occurred

    """
    cdef int return_value
    cdef np.ndarray value_nd = np.asarray(value, cds_type_to_dtype_obj(type))
    if value_nd.ndim == 0:
        value_nd = value_nd[None] # add dummy dimension to a scalar value
    assert value_nd.ndim == 1
    return dsproc_set_att(parent.cds_object, overwrite, name, type,
            len(value_nd), value_nd.data)

def set_att_text(cds3.core.Object parent, char *name, char *value):
    """Set the value of an attribute in a dataset or variable.
    
    The cds_string_to_array() function will be used to set the attribute
    value if the data type of the attribute is not CDS_CHAR.
    
    If an error occurs in this function it will be appended to the log and
    error mail messages, and the process status will be set appropriately.
    
    @param parent - pointer to the parent CDSGroup or CDSVar
    @param name - name of the attribute
    @param value - the string
    
    @retval 1 if successful
    @retval 0 if the attribute does not exist
    @retval 0 if the attribute definition is locked
    @retval 0 if a memory allocation error occurred

    """
    return dsproc_set_att_text(parent.cds_object, name, value)

def set_att_value(cds3.core.Object parent, char *name,
        CDSDataType type, object value):
    """Set the value of an attribute in a dataset or variable.
    
    This function will set the value of an attribute by casting the
    specified value into the data type of the attribute. The functions
    cds_string_to_array() and cds_array_to_string() are used to convert
    between text (CDS_CHAR) and numeric data types.
    
    If an error occurs in this function it will be appended to the log and
    error mail messages, and the process status will be set appropriately.
    
    @param parent - the parent CDSGroup or CDSVar
    @param name - name of the attribute
    @param type - data type of the specified value
    @param value - the attribute value
    
    @retval 1 if successful
    @retval 0 if the attribute does not exist
    @retval 0 if the attribute definition is locked
    @retval 0 if a memory allocation error occurred

    """
    cdef np.ndarray value_nd
    if type == CDS_CHAR:
        value_nd = np.asarray(value, np.str)
        length = len(value_nd.data)
    else:
        value_nd = np.asarray(value, cds_type_to_dtype_obj(type))
    if value_nd.ndim == 0:
        value_nd = value_nd[None] # add dummy dimension to a scalar value
    assert value_nd.ndim == 1
    if type != CDS_CHAR:
        length = len(value_nd)
    return dsproc_set_att_value(parent.cds_object, name, type,
            length, value_nd.data)

def clone_var(
            cds3.core.Var src_var,
            cds3.core.Group dataset=None,
            char *var_name="",
            CDSDataType data_type=CDS_NAT,
            object dim_names=None, # should be iterable i.e. 1-d list/tuple
            bint copy_data=True):
    """Create a clone of an existing variable.
    
    If an error occurs in this function it will be appended to the log and
    error mail messages, and the process status will be set appropriately.
    
    @param src_var - the source variable to clone
    @param dataset - dataset to create the new variable in, or None to create
            the variable in the same dataset the source variable belongs to.
    @param var_name - name to use for the new variable, or None to use the
            source variable name.
    @param data_type - data type to use for the new variable, or CDS_NAT to use
            the same data type as the source variable.
    @param dim_names - the list of corresponding dimension names in the dataset
            the new variable will be created in, or None if the dimension names
            are the same.
    @param copy_data - flag indicating if the data should be copied,
            (1 == TRUE, 0 == FALSE)
    
    @retval the new variable
    @retval None if the variable already exists in the dataset
    @retval None if a memory allocation error occurred

    """
    cdef CDSVar *cds_var = NULL
    cdef cds3.core.Var var
    cdef const char **c_dim_names = NULL
    cdef CDSGroup *cds_group = NULL
    if data_type == CDS_NAT:
        data_type = src_var.c_ob.type
    if dataset is not None:
        cds_group = dataset.c_ob
    if dim_names is not None and len(dim_names) > 0:
        c_dim_names = <const_char**>malloc(len(dim_names) * sizeof(char*))
        for i in range(len(dim_names)):
            c_dim_names[i] = PyString_AsString(dim_names[i])
    # cython doesn't like assigning NULL to char* variables!
    # but dsproc3 expects var_name to be NULL (we signal via empty string)
    if len(var_name) > 0:
        cds_var = dsproc_clone_var(src_var.c_ob, cds_group, var_name, data_type,
                c_dim_names, copy_data)
    else:
        cds_var = dsproc_clone_var(src_var.c_ob, cds_group, NULL, data_type,
                c_dim_names, copy_data)
    if dim_names is not None:
        free(c_dim_names)
    if cds_var == NULL:
        return None
    else:
        var = cds3.core.Var()
        var.set_var(cds_var)
        return var

def define_var(
            cds3.core.Group dataset,
            char *name,
            CDSDataType type,
            #int ndims, # derived from dim_names
            object dim_names, # should be iterable i.e. 1-d list/tuple
            char *long_name="",
            char *standard_name="",
            char *units="",
            object valid_min=None,
            object valid_max=None,
            object missing_value=None,
            object fill_value=None):
    """Define a new variable in an existing dataset.
    
    This function will define a new variable with all standard attributes.
    Any of the attribute values can be NULL to indicate that the attribute
    should not be created.
    
    Description of Attributes:
    
    <b>long_name:</b>
    This is a one line description of the variable and should be suitable
    to use as a plot title for the variable.
    
    <b>standard_name:</b>
    This is defined in the CF Convention and describes the physical
    quantities being represented by the variable. Please refer to the
    "CF Standard Names" section of the CF Convention for the table of
    standard names.
    
    <b>units:</b>
    This is the units string to use for the variable and must be
    recognized by the UDUNITS-2 libary.
    
    <b>valid_min:</b>
    The smallest value that should be considered to be a valid data value.
    The specified value must be the same data type as the variable.
    
    <b>valid_max:</b>
    The largest value that should be considered to be a valid data value.
    The specified value must be the same data type as the variable.
    
    <b>missing_value:</b>
    This comes from an older NetCDF convention and has been used by ARM
    for almost 2 decades. The specified value must be the same data type
    as the variable.
    
    <b>_FillValue:</b>
    Most newer conventions specify the use of _FillValue over missing_value.
    The value of this attribute is also recognized by the NetCDF library and
    will be used to initialize the data values on disk when the variable is
    created. Tools like ncdump will also display fill values as _ so they
    can be easily identified in a text dump. The libdsproc3 library allows
    you to use both missing_value and _FillValue and they do not need to be
    the same. The specified value must be the same data type as the variable.
    
    If an error occurs in this function it will be appended to the log and
    error mail messages, and the process status will be set appropriately.
    
    @param dataset - pointer to the dataset
    @param name - name of the variable
    @param type - data type of the variable
    @param dim_names -  iterable i.e. 1-d list/tuple of dimension names
    @param long_name - string to use for the long_name attribute
    @param standard_name - string to use for the standard_name attribute
    @param units - string to use for the units attribute
    @param valid_min - void pointer to the valid_min
    @param valid_max - void pointer to the valid_max
    @param missing_value - void pointer to the missing_value
    @param fill_value - void pointer to the _FillValue
    
    @return
    - pointer to the new variable
    - NULL if an error occurred
    """
    cdef CDSVar *cds_var = NULL
    cdef cds3.core.Var var
    cdef np.dtype dtype
    cdef void *min_ptr=NULL, *max_ptr=NULL, *missing_ptr=NULL, *fill_ptr=NULL
    cdef char min_char, max_char, missing_char, fill_char
    cdef signed char min_byte, max_byte, missing_byte, fill_byte
    cdef short min_short, max_short, missing_short, fill_short
    cdef int min_int, max_int, missing_int, fill_int
    cdef float min_float, max_float, missing_float, fill_float
    cdef double min_double, max_double, missing_double, fill_double
    cdef const char **c_dim_names = <const_char**>malloc(len(dim_names) * sizeof(char*))
    cdef char *c_long_name
    cdef char *c_standard_name
    cdef char *c_units
    if len(long_name) != 0:
        c_long_name = long_name
    else:
        c_long_name = NULL
    if len(standard_name) != 0:
        c_standard_name = standard_name
    else:
        c_standard_name = NULL
    if len(units) != 0:
        c_units = units
    else:
        c_units = NULL
    for i in range(len(dim_names)):
        c_dim_names[i] = PyString_AsString(dim_names[i])
    if type == CDS_NAT:
        raise ValueError, "CDS_NAT"
    elif type == CDS_CHAR:
        if valid_min is not None:
            min_char = valid_min
            min_ptr = &min_char
        if valid_max is not None:
            max_char = valid_max
            max_ptr = &max_char
        if missing_value is not None:
            missing_char = missing_value
            missing_ptr = &missing_char
        if fill_value is not None:
            fill_char = fill_value
            fill_ptr = &fill_char
    elif type == CDS_BYTE:
        if valid_min is not None:
            min_byte = valid_min
            min_ptr = &min_byte
        if valid_max is not None:
            max_byte = valid_max
            max_ptr = &max_byte
        if missing_value is not None:
            missing_byte = missing_value
            missing_ptr = &missing_byte
        if fill_value is not None:
            fill_byte = fill_value
            fill_ptr = &fill_byte
    elif type == CDS_SHORT:
        if valid_min is not None:
            min_short = valid_min
            min_ptr = &min_short
        if valid_max is not None:
            max_short = valid_max
            max_ptr = &max_short
        if missing_value is not None:
            missing_short = missing_value
            missing_ptr = &missing_short
        if fill_value is not None:
            fill_short = fill_value
            fill_ptr = &fill_short
    elif type == CDS_INT:
        if valid_min is not None:
            min_int = valid_min
            min_ptr = &min_int
        if valid_max is not None:
            max_int = valid_max
            max_ptr = &max_int
        if missing_value is not None:
            missing_int = missing_value
            missing_ptr = &missing_int
        if fill_value is not None:
            fill_int = fill_value
            fill_ptr = &fill_int
    elif type == CDS_FLOAT:
        if valid_min is not None:
            min_float = valid_min
            min_ptr = &min_float
        if valid_max is not None:
            max_float = valid_max
            max_ptr = &max_float
        if missing_value is not None:
            missing_float = missing_value
            missing_ptr = &missing_float
        if fill_value is not None:
            fill_float = fill_value
            fill_ptr = &fill_float
    elif type == CDS_DOUBLE:
        if valid_min is not None:
            min_double = valid_min
            min_ptr = &min_double
        if valid_max is not None:
            max_double = valid_max
            max_ptr = &max_double
        if missing_value is not None:
            missing_double = missing_value
            missing_ptr = &missing_double
        if fill_value is not None:
            fill_double = fill_value
            fill_ptr = &fill_double
    else:
        raise ValueError, "invalid CDSType"

    cds_var = dsproc_define_var(dataset.c_ob, name, type, len(dim_names),
            c_dim_names, c_long_name, c_standard_name, c_units,
            min_ptr, max_ptr, missing_ptr, fill_ptr)

    if dim_names is not None:
        free(c_dim_names)
    if cds_var == NULL:
        return None
    else:
        var = cds3.core.Var()
        var.set_var(cds_var)
        return var


def delete_var(cds3.core.Var var):
    """Delete a variable from a dataset.
    
    If an error occurs in this function it will be appended to the log and
    error mail messages, and the process status will be set appropriately.
    
    @param var - pointer to the variable
    
    @retval 1 if the variable was deleted (or the input var was NULL)
    @retval 0 if the variable is locked
    @retval 0 if the group is locked

    """
    cdef int return_value = dsproc_delete_var(var.c_ob)
    if return_value == 1:
        var.set_var(NULL)
    return return_value

def get_coord_var(cds3.core.Var _var, int dim_index):
    """Get the coordinate variable for a variable's dimension.
    
    @param var - pointer to the variable
    @param dim_index - index of the dimension
    
    @retval pointer to the coordinate variable
    @retval None if not found

    """
    cdef CDSVar *cds_var
    cdef cds3.core.Var var
    cds_var = dsproc_get_coord_var(_var.c_ob, dim_index)
    if cds_var == NULL:
        return None
    var = cds3.core.Var()
    var.set_var(cds_var)
    return var

def get_dataset_vars(
            cds3.core.Group dataset,
            object var_names, # iterable of strings
            bint required,
            bint qc_vars=False, # whether to also return qc_vars
            bint aqc_vars=False): # whether to also return aqc_vars
    """Get variables and companion QC variables from a dataset.
    
    If nvars is 0 or var_names is None, the output vars array will contain
    the pointers to the variables that are not companion QC variables. In
    this case the variables in the vars array will be in the same order they
    appear in the dataset. The following time and location variables will be
    excluded from this array:
    
    - base_time
    - time_offset
    - time
    - lat
    - lon
    - alt
    
    If nvars and var_names are specified, the output vars array will contain
    an entry for every variable in the var_names list, and will be in the
    specified order. Variables that are not found in the dataset will have
    a NULL value if the required flag is set to 0. If the required flag is
    set to 1 and a variable does not exist, an error will be generated.
    
    If the qc_vars argument is not NULL it will contain the pointers to the
    companion qc_ variables. Likewise, if the aqc_vars argument is not NULL
    it will contain the pointers to the companion aqc_ variables. If a
    companion QC variable does not exist for a variable, the corresponding
    entry in the QC array will be NULL.
    
    The memory used by the returned arrays belongs to a 'dsproc_user_...'
    CDSVarGroup defined in the dataset and must *NOT* be freed by the calling
    process.
    
    If an error occurs in this function it will be appended to the log and
    error mail messages, and the process status will be set appropriately.
    
    @param dataset - the dataset
    @param var_names - list of variable names
    @param required - specifies if all variables in the names list are required

    @retval variables if qc_vars==False and aqc_vars==False
    @retval variables,qc_vars if qc_vars==True and aqc_vars==False
    @retval variables,aqc_vars if qc_vars==False and aqc_vars==True
    @retval variables,qc_vars,aqc_vars if qc_vars==True and aqc_vars==True
    @retval if error, None if qc_vars==False and aqc_vars==False
    @retval if error, None,None if qc_vars==True and aqc_vars==False
    @retval if error, None,None if qc_vars==False and aqc_vars==True
    @retval if error, None,None,None if qc_vars==True and aqc_vars==True

    """
    cdef int length
    cdef cds3.core.Var var_tmp
    cdef CDSVar **_vars = NULL
    cdef CDSVar **_qc_vars = NULL
    cdef CDSVar **_aqc_vars = NULL
    cdef const char **c_var_names = NULL
    if var_names:
        c_var_names = <const_char**>malloc((len(var_names)+1) * sizeof(char*))
        for i in range(len(var_names)):
            c_var_names[i] = PyString_AsString(var_names[i])
        c_var_names[len(var_names)] = NULL
    if qc_vars and aqc_vars:
        length = dsproc_get_dataset_vars(dataset.c_ob, c_var_names, required,
                &_vars, &_qc_vars, &_aqc_vars)
    elif qc_vars:
        length = dsproc_get_dataset_vars(dataset.c_ob, c_var_names, required,
                &_vars, &_qc_vars, NULL)
    elif aqc_vars:
        length = dsproc_get_dataset_vars(dataset.c_ob, c_var_names, required,
                &_vars, NULL, &_aqc_vars)
    else:
        length = dsproc_get_dataset_vars(dataset.c_ob, c_var_names, required,
                &_vars, NULL, NULL)
    if var_names:
        free(c_var_names)
    if -1 == length:
        if qc_vars and aqc_vars:
            return None,None,None
        elif qc_vars or aqc_vars:
            return None,None
        else:
            return None
    ret_vars = [None]*length
    ret_qc_vars = [None]*length
    ret_aqc_vars = [None]*length
    for i in range(length):
        if _vars and _vars[i]:
            var_tmp = cds3.core.Var()
            var_tmp.set_var(_vars[i])
            ret_vars[i] = var_tmp
        if qc_vars and _qc_vars and _qc_vars[i]:
            var_tmp = cds3.core.Var()
            var_tmp.set_var(_qc_vars[i])
            ret_qc_vars[i] = var_tmp
        if aqc_vars and _aqc_vars and _aqc_vars[i]:
            var_tmp = cds3.core.Var()
            var_tmp.set_var(_aqc_vars[i])
            ret_aqc_vars[i] = var_tmp
    if qc_vars and aqc_vars:
        return ret_vars,ret_qc_vars,ret_aqc_vars
    elif qc_vars:
        return ret_vars,ret_qc_vars
    elif aqc_vars:
        return ret_vars,ret_aqc_vars
    else:
        return ret_vars

def get_metric_var(cds3.core.Var var, char *metric):
    """Get a companion metric variable for a variable.
    
    Known metrics at the time of this writing (so there may be others):
    
    - "frac": the fraction of available input values used
    - "std": the standard deviation of the calculated value
    
    @param var - pointer to the variable
    @param metric - name of the metric
    
    @retval the metric variable
    @retval None if not found

    """
    cdef CDSVar *cds_var
    cdef cds3.core.Var metric_var
    cds_var = dsproc_get_metric_var(var.c_ob, metric)
    if cds_var == NULL:
        return None
    metric_var = cds3.core.Var()
    metric_var.set_var(cds_var)
    return metric_var

def get_output_var(int ds_id, char *var_name, int obs_index=0):
    """Get a variable from an output dataset.
    
    The obs_index should always be zero unless observation based processing is
    being used. This is because all input observations should have been merged
    into a single observation in the output datasets.
    
    @param ds_id - output datastream ID
    @param var_name - variable name
    @param obs_index - the index of the obervation to get the dataset for
    
    @retval pointer to the output variable
    @retval None if it does not exist

    """
    cdef CDSVar *cds_var
    cdef cds3.core.Var var
    cds_var = dsproc_get_output_var(ds_id, var_name, obs_index)
    if cds_var == NULL:
        return None
    var = cds3.core.Var()
    var.set_var(cds_var)
    return var

def get_qc_var(cds3.core.Var _var):
    """Get the companion QC variable for a variable.
    
    @param var - pointer to the variable
    
    @retval pointer to the QC variable
    @retval None if not found

    """
    cdef CDSVar *cds_var
    cdef cds3.core.Var var
    cds_var = dsproc_get_qc_var(_var.c_ob)
    if cds_var == NULL:
        return None
    var = cds3.core.Var()
    var.set_var(cds_var)
    return var

def get_retrieved_var(char *var_name, int obs_index=0):
    """Get a primary variable from the retrieved data.
    
    This function will find a variable in the retrieved data that was
    explicitly requested by the user in the retriever definition.
    
    The obs_index is used to specify which observation to pull the variable
    from. This value will typically be zero unless this function is called
    from a post_retrieval_hook() function, or the process is using observation
    based processing. In either of these cases the retrieved data will
    contain one "observation" for every file the data was read from on disk.
    
    It is also possible to have multiple observations in the retrieved data
    when a pre_transform_hook() is called if a dimensionality conflict
    prevented all observations from being merged.
    
    @param var_name - variable name
    @param obs_index - the index of the obervation to get the variable from
    
    @retval pointer to the retrieved variable
    @retval None if not found

    """
    cdef CDSVar *cds_var
    cdef cds3.core.Var var
    cds_var = dsproc_get_retrieved_var(var_name, obs_index)
    if cds_var == NULL:
        return None
    var = cds3.core.Var()
    var.set_var(cds_var)
    return var

def get_transformed_var(char *var_name, int obs_index=0):
    """Get a primary variable from the transformed data.
    
    This function will find a variable in the transformed data that was
    explicitly requested by the user in the retriever definition.
    
    The obs_index is used to specify which observation to pull the variable
    from. This value will typically be zero unless the process is using
    observation based processing. If this is the case the transformed data will
    contain one "observation" for every file the data was read from on disk.
    
    @param var_name - variable name
    @param obs_index - the index of the obervation to get the variable from
    
    @retval pointer to the transformed variable
    @retval None if not found

    """
    cdef CDSVar *cds_var
    cdef cds3.core.Var var
    cds_var = dsproc_get_transformed_var(var_name, obs_index)
    if cds_var == NULL:
        return None
    var = cds3.core.Var()
    var.set_var(cds_var)
    return var

def get_trans_coordsys_var(char *coordsys_name, char *var_name, int obs_index=0):
    """Get a variable from a transformation coordinate system.
    
    Unlike the dsproc_get_transformed_var() function, this function will find
    any variable in the specified transformation coordinate system.
    
    The obs_index is used to specify which observation to pull the variable
    from. This value will typically be zero unless the process is using
    observation based processing. If this is the case the transformed data will
    contain one "observation" for every file the data was read from on disk.
    
    @param coordsys_name - coordinate system name
    @param var_name - variable name
    @param obs_index - the index of the obervation to get the variable from
    
    @retval the transformed variable
    @retval None if not found

    """
    cdef CDSVar *cds_var
    cdef cds3.core.Var var
    cds_var = dsproc_get_trans_coordsys_var(coordsys_name, var_name, obs_index)
    if cds_var == NULL:
        return None
    var = cds3.core.Var()
    var.set_var(cds_var)
    return var

def get_var(cds3.core.Group dataset, char *name):
    """Get a variable from a dataset.
    
    @param dataset - pointer to the dataset
    @param name - name of the variable
    
    @retval the variable
    @retval None if the variable does not exist

    """
    cdef CDSVar *cds_var
    cdef cds3.core.Var var
    cds_var = dsproc_get_var(dataset.c_ob, name)
    if cds_var == NULL:
        return None
    var = cds3.core.Var()
    var.set_var(cds_var)
    return var

def var_name(cds3.core.Var var):
    """Returns the variable name.
    
    @param var - the variable
    
    @retval the variable name
    @retval None if the specified variable is None

    """
    cdef const char *retval
    if var is None:
        return None
    retval = dsproc_var_name(var.c_ob)
    if retval == NULL:
        return None
    return retval

def var_sample_count(cds3.core.Var var):
    """Returns the number of samples in a variable's data array.
    
    The sample_count for a variable is the number of samples stored
    along the variable's first dimension.
    
    @param var - the variable
    
    @retval number of samples in the variable's data array

    """
    return dsproc_var_sample_count(var.c_ob)

def var_sample_size(cds3.core.Var var):
    """Returns the sample size of a variable.
    
    Variables with less than 2 dimensions will always have a sample_size of 1.
    The sample_size for variables with 2 or more dimensions is the product of
    all the dimension lengths starting with the 2nd dimension.
    
    @param var - pointer to the variable
    
    @return sample size of the variable

    """
    return dsproc_var_sample_size(var.c_ob)

def alloc_var_data(cds3.core.Var var, size_t sample_start, size_t sample_count):
    """Allocate memory for a variable's data array.
    
    This function will allocate memory as necessary to ensure that the
    variable's data array is large enough to store another sample_count
    samples starting from sample_start.
    
    The data type of the returned array will be the same as the variable’s
    data type.

    If an error occurs in this function it will be appended to the log and
    error mail messages, and the process status will be set appropriately.
    
    @param var - the variable
    @param sample_start - start sample (0 based indexing)
    @param sample_count - number of new samples
    
    @retval data array starting at the specified start sample
    @retval None if one of the variable's static dimensions has 0 length
    @retval None if the variable has no dimensions, and sample_start is not
            equal to 0 or sample_count is not equal to 1.
    @retval None if the first variable dimension is not unlimited, and
            sample_start + sample_count would exceed the dimension length.
    @retval None if a memory allocation error occurred

    """
    cdef void *data = NULL
    data = dsproc_alloc_var_data(var.c_ob, sample_start, sample_count)
    if data == NULL:
        return None
    return var.get_datap(sample_start)

def alloc_var_data_index(cds3.core.Var var, size_t sample_start,
        size_t sample_count):
    """Allocate memory for a variable's data array.
    
    This function is the same as dsproc.alloc_var_data() except that
    it returns a data index starting at the specified start sample
    (see dsproc.get_var_data_index()). For variables that have less than
    two dimensions this function is identical to dsproc_alloc_var_data().
    It is up to the calling process to cast the return value of this
    function into the proper data type.
    
    If an error occurs in this function it will be appended to the log and
    error mail messages, and the process status will be set appropriately.
    
    @param var - pointer to the variable
    @param sample_start - start sample (0 based indexing)
    @param sample_count - number of new samples
    
    @retval the data index into the variable's data array starting at the
            specified start sample
    @retval None if one of the variable's static dimensions has 0 length
    @retval None if the variable has no dimensions, and sample_start is not
            equal to 0 or sample_count is not equal to 1.
    @retval None if the first variable dimension is not unlimited, and
            sample_start + sample_count would exceed the dimension length.
    @retval None if a memory allocation error occurred

    """
    cdef void *data = NULL
    data = dsproc_alloc_var_data_index(var.c_ob, sample_start, sample_count)
    if data == NULL:
        return None
    return var.get_datap(sample_start)

def get_var_data_index(cds3.core.Var var):
    """Get a data index for a multi-dimensional variable.
    
    This function will return a data index that can be used to access the data
    in a variable using the traditional x[i][j][k] syntax.  It is up to the
    calling process to cast the return value of this function into the proper
    data type.
    
    Note: If the variable has less than 2 dimensions, the pointer to the
    variable’s data array will be returned.
    
    If an error occurs in this function it will be appended to the log and
    error mail messages, and the process status will be set appropriately.
    
    @param var - the variable
    
    @retval the data index into the variable’s data array
    @retval None if the pointer to the variable was NULL
    @retval None if no data has been stored in the variable (var.sample_count == 0)
    @retval None if a memory allocation error occurs

    """
    cdef void *data = NULL
    data = dsproc_get_var_data_index(var.c_ob)
    if data == NULL:
        return None
    return var.get_datap()

def get_var_data(cds3.core.Var var, CDSDataType type, size_t sample_start,
        size_t sample_count=0, np.ndarray data=None):
    """Get a copy of the data from a dataset variable.

    This function will get the data from a variable casted into the specified
    data type. All missing values used in the data will be converted to a single
    missing value appropriate for the requested data type. The missing value
    used will be the first value returned by cds_get_var_missing_values() if
    that value is within the range of the requested data type, otherwise, the
    default fill value for the requested data type will be used.
    
    If an error occurs in this function it will be appended to the log and
    error mail messages, and the process status will be set appropriately.
    
    @todo TODO: 
    @param var - pointer to the variable
    @param type - data type of the output missing_value and data array
    @param sample_start - start sample (0 based indexing)
    
    @retval the output data array and missing value
    @retval None if the pointer to the variable was NULL
    @retval None if the variable has no data for sample_start (sample_count == 0)
    @retval None if a memory allocation error occurs (sample_count == (size_t)-1)

    """
    cdef void* datap = NULL
    cdef void* missing_c = _alloc_single(type)
    cdef int ndims = var.c_ob.ndims
    cdef size_t sample_size = 0
    cdef np.dtype dtype = cds_type_to_dtype_obj(type)
    if sample_count == 0 or data is None:
        datap = dsproc_get_var_data(var.c_ob, type, sample_start,
                &sample_count, missing_c, NULL)
        if datap == NULL:
            return None,None
        dims = <np.npy_intp*>malloc(sizeof(np.npy_intp) * ndims)
        for i in range(ndims):
            dims[i] = var.c_ob.dims[i].length
        dims[0] = sample_count
        data = np.PyArray_SimpleNewFromData(ndims, dims, dtype.num, datap)
        free(dims)
        # allow numpy to reclaim memory when array goes out of scope
        data.base = PyCObject_FromVoidPtr(datap, _free)
        return data, _convert_single(type, missing_c)
    else:
        sample_size = cds_var_sample_size(var.c_ob)
        assert data.flags['C_CONTIGUOUS']
        assert data.size >= sample_count*sample_size
        datap = dsproc_get_var_data(var.c_ob, type, sample_start,
                &sample_count, missing_c, data.data)
        if datap == NULL:
            return None,None
        return data, _convert_single(type, missing_c)

def get_var_missing_values(cds3.core.Var var):
    """Get the missing values for a CDS Variable.

    This function will return an array containing all values specified by
    the missing_value and _FillValue attributes (in that order), and will
    be the same data type as the variable. If the _FillValue attribute does
    not exist but a default fill value has been defined, it will be used
    instead.
    
    If an error occurs in this function it will be appended to the log and
    error mail messages, and the process status will be set appropriately.
    
    @param var - pointer to the variable
    @param values - output: pointer to the array of missing values, the
    data type of this array will be the same as the variable
    
    @retval number of missing values
    @retval 0 if there are no missing or fill values
    @retval -1 if a memory allocation error occurs

    """
    cdef int retval
    cdef np.npy_intp size
    cdef void *values
    cdef CDSDataType type = var.c_ob.type
    cdef np.ndarray array
    retval = dsproc_get_var_missing_values(var.c_ob, &values)
    if retval < 0:
        return None
    elif retval == 0:
        return np.array([]) # empty, size 0 array of some type
    else:
        size = retval
        array = np.PyArray_SimpleNewFromData(1, &size,
                cds_type_to_dtype(type), values)
        # allow numpy to reclaim memory when array goes out of scope
        array.base = PyCObject_FromVoidPtr(values, _free)
        return array

def init_var_data(
        cds3.core.Var var,
        size_t sample_start,
        size_t sample_count,
        bint use_missing):
    """Initialize the data values for a dataset variable.
    
    This function will make sure enough memory is allocated for the specified
    samples and initializing the data values to either the variable's missing
    value (use_missing == True), or 0 (use_missing == False).
    
    The search order for missing values is:
    
    - missing_value attribute
    - _FillValue attribute
    - variable's default missing value
    
    If the variable does not have any missing or fill values defined the
    default fill value for the variable's data type will be used and the
    default fill value for the variable will be set.
    
    If the specified start sample is greater than the variable's current sample
    count, the hole between the two will be filled with the first missing or
    fill value defined for the variable.
    
    If an error occurs in this function it will be appended to the log and
    error mail messages, and the process status will be set appropriately.
    
    @param var - the variable
    @param sample_start - start sample of the data to initialize
            (0 based indexing)
    @param sample_count - number of samples to initialize
    @param use_missing - flag indicating if the variables missing value
            should be used (1 == TRUE, 0 == fill with zeros)
    
    @retval the specifed start sample in the variable's data array
    @retval None if the specified sample count is zero
    @retval None if one of the variable's static dimensions has 0 length
    @retval None if the variable has no dimensions, and sample_start is not
            equal to 0 or sample_count is not equal to 1.
    @retval None if the first variable dimension is not unlimited, and
            sample_start + sample_count would exceed the dimension length.
    @retval None if a memory allocation error occurred

    """
    cdef void *data = NULL
    data = dsproc_init_var_data(var.c_ob, sample_start, sample_count,
            use_missing)
    if data == NULL:
        return None
    return var.get_datap(sample_start)

def init_var_data_index(
            cds3.core.Var var,
            size_t sample_start,
            size_t sample_count,
            int use_missing):
    """Initialize the data values for a dataset variable.
    
    This function will make sure enough memory is allocated for the specified
    samples and initializing the data values to either the variable's missing
    value (use_missing == 1), or 0 (use_missing == 0).
    
    The search order for missing values is:
    
    - missing_value attribute
    - _FillValue attribute
    - variable's default missing value
    
    If the variable does not have any missing or fill values defined the
    default fill value for the variable's data type will be used and the
    default fill value for the variable will be set.
    
    If the specified start sample is greater than the variable's current sample
    count, the hole between the two will be filled with the first missing or
    fill value defined for the variable.
    
    If an error occurs in this function it will be appended to the log and
    error mail messages, and the process status will be set appropriately.
    
    @param var - pointer to the variable
    @param sample_start - start sample of the data to initialize (0 based indexing)
    @param sample_count - number of samples to initialize
    @param use_missing - flag indicating if the variables missing value
            should be used (1 == TRUE, 0 == fill with zeros)
    
    @retval pointer to the specifed start sample in the variable's data array
    @retval None if the specified sample count is zero
    @retval None if one of the variable's static dimensions has 0 length
    @retval None if the variable has no dimensions, and sample_start is not
            equal to 0 or sample_count is not equal to 1.
    @retval None if the first variable dimension is not unlimited, and
            sample_start + sample_count would exceed the dimension length.
    @retval None if a memory allocation error occurred

    """
    cdef void *data = NULL
    data = dsproc_init_var_data_index(var.c_ob, sample_start, sample_count,
            use_missing)
    if data == NULL:
        return None
    return var.get_datap(sample_start)

def set_var_data(
            cds3.core.Var var,
            size_t sample_start,
            size_t sample_count,
            object missing_value,
            np.ndarray data_nd):
    """Set the data values for a dataset variable.
    
    This function will set the data values of a variable by casting the values
    in the input data array into the data type of the variable. All missing
    values in the input data array will be converted to the first missing value
    used by the variable as returned by cds_get_var_missing_values(). If the
    variable does not have a missing_value or _FillValue attribute defined, the
    default fill value for the variable's data type will be used.
    
    For multi-dimensional variables, the specified data array must be stored
    linearly in memory with the last dimension varying the fastest.
    
    If an error occurs in this function it will be appended to the log and
    error mail messages, and the process status will be set appropriately.
    
    @param var - pointer to the variable
    @param type - data type of the input missing_value and data array
    @param sample_start - start sample of the new data (0 based indexing)
    @param sample_count - number of new samples
    @param missing_value - pointer to the missing value used in the data array,
    or None if the data does not contain any missing values
    @param data - pointer to the input data array
    
    @retval the specifed start sample in the variable's data array
    @retval None if one of the variable's static dimensions has 0 length
    @retval None if the variable has no dimensions, and sample_start is not
            equal to 0 or sample_count is not equal to 1.
    @retval None if the first variable dimension is not unlimited, and
            sample_start + sample_count would exceed the dimension length.
    @retval None if a memory allocation error occurred

    """
    cdef void *data = NULL
    cdef CDSDataType type
    cdef void *missing_c = NULL
    type = <CDSDataType>dtype_to_cds_type(data_nd.dtype)
    if missing_value is not None:
        missing_c = _alloc_single(type, missing_value)
    data = dsproc_set_var_data(var.c_ob, type, sample_start, sample_count,
            missing_c, data_nd.data)
    if missing_value is not None:
        free(missing_c)
    if data == NULL:
        return None
    return var.get_datap(sample_start)

def get_base_time(cds3.core.Object cds_object):
    """Get the base time of a dataset or time variable.
    
    This function will convert the units attribute of a time variable to
    seconds since 1970. If the input object is a CDSGroup, the specified
    dataset and then its parent datasets will be searched until a "time"
    or "time_offset" variable is found.
    
    @param cds_object - a CDSGroup or CDSVar
    
    @retval base time in seconds since 1970 UTC
    @retval 0 if not found

    """
    return dsproc_get_base_time(cds_object.cds_object)

#size_t  dsproc_get_time_range(
#            void      *cds_object,
#            timeval_t *start_time,
#            timeval_t *end_time)
def get_time_range(cds3.core.Object cds_object):
    """Get the time range of a dataset or time variable.
    
    This function will get the start and end times of a time variable.
    If the input object is a CDSGroup, the specified dataset and then
    its parent datasets will be searched until a "time" or "time_offset"
    variable is found.
    
    @param cds_object - a CDSGroup or CDSVar
    @param start_time - pointer to the timeval_t structure to store the
    start time in.
    @param end_time - pointer to the timeval_t structure to store the
    end time in.
    
    @retval number of time values, start=(sec,ms), end=(sec,ms)
    @retval empty list, empty list if no time values were found

    """
    cdef timeval_t start
    cdef timeval_t end
    cdef size_t num_times
    num_times = dsproc_get_time_range(cds_object.cds_object, &start, &end)
    return num_times, (start.tv_sec, start.tv_usec), (end.tv_sec, end.tv_usec)

def get_time_var(cds3.core.Object cds_object):
    """Get the time variable used by a dataset.
    
    This function will find the first dataset that contains either the "time"
    or "time_offset" variable and return a pointer to that variable.
    
    @param cds_object - pointer to a CDSGroup or CDSVar
    
    @retval the time variable
    @retval None if not found

    """
    cdef CDSVar *cds_var
    cdef cds3.core.Var var
    cds_var = dsproc_get_time_var(cds_object.cds_object)
    if cds_var == NULL:
        return None
    var = cds3.core.Var()
    var.set_var(cds_var)
    return var

def get_sample_times(cds3.core.Object cds_object, size_t sample_start):
    """Get the sample times for a dataset or time variable.
    
    This function will convert the data values of a time variable to seconds
    since 1970. If the input object is a CDSGroup, the specified dataset and
    then its parent datasets will be searched until a "time" or "time_offset"
    variable is found.
    
    Note: If the sample times can have fractional seconds the
    dsproc_get_sample_timevals() function should be used instead.
    
    If an error occurs in this function it will be appended to the log
    
    error mail messages, and the process status will be set appropriately.
    
    @param cds_object - pointer to a CDSGroup or CDSVar
    @param sample_start - start sample (0 based indexing)
    
    @retval list of sample times in seconds since 1970 
    @retval None if there is no data for sample_start (sample_count == 0)
    @retval None if an error occurred (sample_count == (size_t)-1)

    """
    cdef list retval
    cdef time_t *sample_times = NULL
    cdef size_t sample_count = 0
    sample_times = dsproc_get_sample_times(cds_object.cds_object,
            sample_start, &sample_count, NULL)
    if sample_times == NULL:
        return None
    retval = [long(sample_times[i]) for i in range(sample_count)]
    free(sample_times)
    return retval

#timeval_t *dsproc_get_sample_timevals(
#            void      *cds_object,
#            size_t     sample_start,
#            size_t    *sample_count,
#            timeval_t *sample_times)
def get_sample_timevals(
            cds3.core.Object cds_object,
            size_t sample_start):
            #size_t *sample_count, in/out
            #timeval_t *sample_times) out
    """Get the sample times for a dataset or time variable.
    
    This function will convert the data values of a time variable to an
    array of timeval_t structures. If the input object is a CDSGroup, the
    specified dataset and then its parent datasets will be searched until
    a "time" or "time_offset" variable is found.
    
    Note: Consider using the cds.get_sample_times() function if the sample
    times can not have fractional seconds.
    
    If an error occurs in this function it will be appended to the log and
    error mail messages, and the process status will be set appropriately.
    
    @param cds_object - pointer to a CDSGroup or CDSVar
    @param sample_start - start sample (0 based indexing)
    
    @retval list of sample times in seconds since 1970
    @retval None if the variable has no data for sample_start (sample_count == 0)
    @retval None if an error occurred (sample_count == (size_t)-1)

    """
    cdef list retval
    cdef timeval_t *sample_times = NULL
    cdef size_t sample_count = 0
    sample_times = dsproc_get_sample_timevals(cds_object.cds_object,
            sample_start, &sample_count, NULL)
    if sample_times == NULL:
        return None
    retval = [(long(sample_times[i].tv_sec),long(sample_times[i].tv_usec))
            for i in range(sample_count)]
    free(sample_times)
    return retval

def set_base_time(cds3.core.Object cds_obj, char *long_name, size_t base_time):
    """
    Set the base time of a dataset or time variable.
    This function will set the base time for a time variable and 
    adjust all attributes and data values as necessary. If the 
    input object is one of the standard time variables 
    ("time", "time_offset", or "base_time"), all standard time variables 
    that exist in its parent dataset will also be updated. If the input 
    object is a CDSGroup, the specified dataset and then its parent datasets 
    will be searched until a "time" or "time_offset" variable is found. 
    All standard time variables that exist in this dataset will then be updated.

    For the base_time variable the data value will be set and the "string" 
    attribute will be set to the string representation of the base_time value. 
    The "long_name" and "units" attributes will also be set to "Base time in Epoch" 
    and "seconds since 1970-1-1 0:00:00 0:00" respectively.

    For the time_offset variable the "units" attribute will set to the 
    string representation of the base_time value, and the "long_name" 
    attribute will be set to "Time offset from base_time".

    For all other time variables the "units" attribute will be set to the 
    string representation of the base_time value, and the "long_name" 
    attribute will be set to the specified value. If a long_name attribute 
    is not specified, the string "Time offset from midnight" will be used 
    for base times of midnight, and "Sample times" will be used for all other base times.

    Any existing data in a time variable will also be adjusted for the new base_time value.

    If an error occurs in this function it will be appended to the log and 
    error mail messages, and the process status will be set appropriately.
 
    @param cds_object - pointer to a CDSGroup or CDSVar
    @param long_name  - string to use for the long_name attribute, or 
                        NULL to use the default value
    @param base_time  - base time in seconds since 1970

    @return
    - 1 if successful
    - 0 if an error occurred

    """
    cdef int retval
    retval = dsproc_set_base_time(cds_obj.cds_object, long_name, base_time)
    return retval

def set_sample_times(cds3.core.Object obj, size_t sample_start,
        object sample_times):
    cdef int retval
    cdef time_t *sample_times_c = NULL
    cdef size_t sample_count = len(sample_times)
    sample_times_c = <time_t*>malloc(sizeof(time_t) * sample_count)
    for i in range(len(sample_times)):
        sample_times_c[i] = sample_times[i]
    retval = dsproc_set_sample_times(<void*>obj.cds_object,
            sample_start, sample_count, sample_times_c)
    free(sample_times_c)
    return retval

def set_sample_timevals(cds3.core.Object obj, size_t sample_start,
        object sample_times):
    """Set the sample times for a dataset or time variable.
    
    This function will set the data values for a time variable by subtracting
    the base time (as defined by the units attribute) and converting the
    remainder to the data type of the variable.
    
    If the input object is one of the standard time variables:
    
    - time
    - time_offset
    - base_time
    
    All standard time variables that exist in its parent dataset will also be
    updated.
    
    If the input object is a CDSGroup, the specified dataset and then its parent
    datasets will be searched until a "time" or "time_offset" variable is found.
    All standard time variables that exist in this dataset will then be updated.
    
    If the specified sample_start value is 0 and a base time value has not
    already been set, the base time will be set using the time of midnight
    just prior to the first sample time.
    
    The data type of the time variable(s) must be either CDS_FLOAT or
    or CDS_DOUBLE. However, CDS_DOUBLE is usually recommended.
    
    If an error occurs in this function it will be appended to the log and
    error mail messages, and the process status will be set appropriately.
    
    @param cds_object - pointer to a CDSGroup or CDSVar
    @param sample_start - start sample (0 based indexing)
    @param sample_count - number of samples in the times array
    @param sample_times - pointer to the array of sample times
    in seconds since 1970 UTC.
    
    @return
    - 1 if successful
    - 0 if an error occurred

    """
    cdef int retval
    cdef timeval_t *sample_times_c = NULL
    cdef size_t sample_count = len(sample_times)
    sample_times_c = <timeval_t*>malloc(sizeof(timeval_t) * sample_count)
    for i in range(len(sample_times)):
        sample_times_c[i].tv_sec,sample_times_c[i].tv_usec = sample_times[i]
    retval = dsproc_set_sample_timevals(obj.cds_object,
            sample_start, sample_count, sample_times_c)
    free(sample_times_c)
    return retval

def add_var_output_target(cds3.core.Var var, int ds_id, char *var_name):
    """Add an output target for a variable.
    
    This function will add an output target for the variable.
    
    If an error occurs in this function it will be appended to the log and
    error mail messages, and the process status will be set appropriately.
    
    @param var - pointer to the variable
    @param ds_id - output datastream ID
    @param var_name - name of the variable in the output datastream
    
    @retval 1 if successful
    @retval 0 if an error occurred

    """
    return dsproc_add_var_output_target(var.c_ob, ds_id, var_name)

def copy_var_tag(cds3.core.Var src_var, cds3.core.Var dest_var):
    """Copy a variable tag from one variable to another.
    
    If an error occurs in this function it will be appended to the log and
    error mail messages, and the process status will be set appropriately.
    
    @param src_var - pointer to the source variable
    @param dest_var - pointer to the destination variable
    
    @retval 1 if successful
    @retval 0 if an error occurred

    """
    return dsproc_copy_var_tag(src_var.c_ob, dest_var.c_ob)

def delete_var_tag(cds3.core.Var var):
    """Delete a variable tag.

    @param var - pointer to the variable

    """
    dsproc_delete_var_tag(var.c_ob)

def get_source_ds_id(cds3.core.Var var):
    """"Get the ID of the input datastream the variable was retrieved from

    @param var - pointer to the variable

    @retval input datastream ID
    @retval -1 if the variable was not explicitly requested by the user in 
            the retriever definition
    """
    cdef int retval
    retval = dsproc_get_source_ds_id(var.c_ob)
    return retval

def get_source_var_name(cds3.core.Var var):
    """"Get the name of the source variable read in from the input file.

    The memory used by the returned variable name belongs to the internal
    variable tag and must not be freed or altered by the calling process.

    @param var - pointer to the variable

    @retval pointer to the name of the source variable
    @retval NULL if the variable was not explicitily requested by user in
            the retriever definition

    """
    cdef const char *retval
    retval = dsproc_get_source_var_name(var.c_ob)
    if retval == NULL:
        return None
    return retval

def get_source_ds_name(cds3.core.Var var):
    """"Get the name of the input datastream the variable was retrieved 
    from.

    The memory used by the returned variable name belongs to the internal
    variable tag and must not be freed or altered by the calling process.

    @param var - pointer to the variable

    @retval pointer to the name of the source variable
    @retval NULL if the variable was not explicitily requested by user in
            the retriever definition

    """
    cdef const char *retval
    retval = dsproc_get_source_ds_name(var.c_ob)
    if retval == NULL:
        return None
    return retval


#int     dsproc_get_var_output_targets(
#            CDSVar      *var,
#            VarTarget ***targets)
def get_var_output_targets(cds3.core.Var var):
    """Get the output targets defined for the specified variable.
    
    The memory used by the output array belongs to the internal variable
    tag and must not be freed or altered by the calling process.
    
    @param var - pointer to the variable
    @param targets - output: pointer to the array of pointers
    to the VarTarget structures.
    
    @return
    - number of variable targets
    - 0 if no variable targets have been defined

    """
    cdef VarTarget **targets
    cdef int count
    cdef list retval = []
    cdef PyVarTarget target
    count = dsproc_get_var_output_targets(var.c_ob, &targets)
    for i in range(count):
        target = PyVarTarget()
        target.set_vartarget(targets[i])
        retval.append(target)
    return retval

def set_var_coordsys_name(cds3.core.Var var, char *coordsys_name):
    return dsproc_set_var_coordsys_name(var.c_ob, coordsys_name)

def set_var_flags(cds3.core.Var var, int flags):
    return dsproc_set_var_flags(var.c_ob, flags)

def set_var_output_target(cds3.core.Var var, int ds_id, char *var_name):
    return dsproc_set_var_output_target(var.c_ob, ds_id, var_name)

def unset_var_flags(cds3.core.Var var, int flags):
    '''
    Unset the control flags for a variable.

    See dsproc_set_var_flags() for flags and descriptions

    @parmam var   - variable
    @param  flags - flags to set
    '''
    dsproc_unset_var_flags(var.c_ob, flags)

#ctypedef struct VarDQR:
#    char  *id          # DQR ID
#    char  *desc        # description
#    char  *ds_name     # datastream name
#    char  *var_name    # variable name
#    int          code        # code number
#    char  *color       # code color
#    char  *code_desc   # code description
#    time_t       start_time  # start time in seconds since 1970
#    time_t       end_time    # end time in seconds since 1970
#    size_t       start_index # start time index in dataset
#    size_t       end_index   # end time index in dataset
#
#int dsproc_get_var_dqrs(CDSVar *var, VarDQR ***dqrs)

def dump_dataset(
        cds3.core.Group dataset,
        char *outdir,
        char *prefix,
        time_t file_time,
        char *suffix,
        int flags):
    return dsproc_dump_dataset(dataset.c_ob, outdir, prefix, file_time, suffix,
            flags)

def dump_output_datasets(char *outdir, char *suffix, int flags):
    return dsproc_dump_output_datasets(outdir, suffix, flags)

def dump_retrieved_datasets(char *outdir, char *suffix, int flags):
    return dsproc_dump_retrieved_datasets(outdir, suffix, flags)

def dump_transformed_datasets(char *outdir, char *suffix, int flags):
    return dsproc_dump_transformed_datasets(outdir, suffix, flags)

def copy_file(char *src_file, char *dest_file):
    return dsproc_copy_file(src_file, dest_file)

def move_file(char *src_file, char *dest_file):
    return dsproc_move_file(src_file, dest_file)

#FILE   *dsproc_open_file(char *file)
#
def add_datastream_file_patterns(int ds_id, object patterns, int ignore_case):
    """
    Adds file patterns to look for when creating the list of 
    files in the datastream directory.  By default all files in the directory
    will be listed. 
    If an error occurs, it will be appened to the log and error mail messages,
    and the process status will be set appropriately.
    @param  ds_id       -  datastream ID
    @param  patterns    -  list of extened regex file patterns (man regcomp)
    @param  ignore_case - ignore case in file_patterns 
    cdef int return_value
    """
    cdef const char **c_patterns = <const_char**>malloc(len(patterns) * sizeof(char*))
    for i in range(len(patterns)):
        c_patterns[i] = PyString_AsString(patterns[i])
    return_value = dsproc_add_datastream_file_patterns(ds_id, len(patterns),
            c_patterns, ignore_case)
    free(c_patterns)
    return return_value

def set_datastream_file_extension(int ds_id, char *extension):
    """
    Set the datastream file extension.
    @param ds_id - datastream ID
    @param extenstion - file extension
    """
    dsproc_set_datastream_file_extension(ds_id, extension)

def get_datastream_files(int ds_id):
    cdef int count
    cdef char **file_list = NULL
    list = []
    count = dsproc_get_datastream_files(ds_id, &file_list)
    for i in range(count):
        list.append(file_list[i])
    return list

# KLG working here
def set_datastream_split_mode(int ds_id, enums.SplitMode split_mode, 
        double split_start, double split_interval):
    """
    Set teh file splitting mode for output files
    Default for VAPs:  always create a new file when data is stored
    split_mode = SPLIT_ON_STORE
    split_start = ignored
    split_interval = ignored
    Default for ingests: daily files that split at midnight
    split_mode = SPLIT_ON_HOURS
    split_start = 0
    split_interval = 24
    @param ds_id           - datastream ID
    @param split_mode     - the file splitting mode (see SplitMode)
    @param split_interval - teh split interval (see Split Mode)
    """
    dsproc_set_datastream_split_mode(ds_id, split_mode, split_start, split_interval)

#    dsproc_set_file_name_compare_function(ds_id, function)

def rename(
        int ds_id,
        char *file_path,
        char *file_name,
        time_t begin_time,
        time_t end_time):
    return dsproc_rename(ds_id, file_path, file_name, begin_time, end_time)

#int     dsproc_rename_tv(
#            int              ds_id,
#            char      *file_path,
#            char      *file_name,
#            timeval_t *begin_time,
#            timeval_t *end_time)

def rename_bad(int ds_id, char *file_path, char *file_name, time_t file_time):
    return dsproc_rename_bad(ds_id, file_path, file_name, file_time)

def set_rename_preserve_dots(int ds_id, int preserve_dots):
    dsproc_set_rename_preserve_dots(ds_id, preserve_dots)

##############################################################################
# ported from dsproc3_internal.h
##############################################################################

def initialize(argv, proc_model, proc_version, proc_names):
    cdef char **c_argv = NULL
    cdef const char **c_proc_names = NULL
    if argv:
        c_argv = <char**>malloc(sizeof(char*) * len(argv))
        if c_argv is NULL:
            raise MemoryError()
        for idx,s in enumerate(argv):
            c_argv[idx] = s
    if proc_names:
        c_proc_names = <const_char**>malloc(sizeof(char*) * len(proc_names))
        if c_proc_names is NULL:
            raise MemoryError()
        for idx,s in enumerate(proc_names):
            c_proc_names[idx] = s
    try:
        dsproc_initialize(len(argv), c_argv, proc_model, proc_version,
                len(proc_names), c_proc_names)
    finally:
        free(c_argv)
        free(c_proc_names)

#int dsproc_start_processing_loop(
#        time_t *interval_begin,
#        time_t *interval_end)
#
#int dsproc_retrieve_data(
#        time_t     begin_time,
#        time_t     end_time,
#        CDSGroup **ret_data)
#
#int dsproc_merge_retrieved_data()
#
#int dsproc_transform_data(
#        CDSGroup **trans_data)
#
#int dsproc_create_output_datasets()
#
#int dsproc_store_output_datasets()
#
def finish():
    return dsproc_finish()
#
#void dsproc_debug(
#        char *func,
#        char *file,
#        int         line,
#        int         level,
#        char *format, ...)
#
#void dsproc_error(
#        char *func,
#        char *file,
#        int         line,
#        char *status,
#        char *format, ...)
#
#void dsproc_log(
#        char *func,
#        char *file,
#        int         line,
#        char *format, ...)
#
#void dsproc_mentor_mail(
#        char *func,
#        char *file,
#        int         line,
#        char *format, ...)
#
#void dsproc_warning(
#        char *func,
#        char *file,
#        int         line,
#        char *format, ...)
#void dsproc_disable(char *message)
#void dsproc_disable_db_updates()
#void dsproc_disable_lock_file()
#void dsproc_disable_mail_messages()

def set_processing_interval(time_t begin_time, time_t end_time):
    """
    Set the begin and end times for the current processing interval.
    This function can be used to override the begin and end times of 
    the current processing interval and should be called from the 
    pre-retrieval hook function.

    @param begin_time -  begin time in seconds since 1970.
    @param end_time   -  end time in seconds since 1970.
    """
    dsproc_set_processing_interval(begin_time, end_time)

def set_processing_interval_offset(time_t offset):
    """
    Set the offset to apply to the processing interval.
    This function can be used to shift the processing interval
    and should be called from either the init-process or
    pre-retrieval hook function.

    @param offset -  offset in seconds
    """
    dsproc_set_processing_interval_offset(offset)

def set_retriever_time_offsets(int ds_id, time_t begin_time, time_t end_time):
    """
    Set the time offsets to use when retrieving data.
    This function can be used to override the begin and end time 
    offsets specified in the retriever definition and should be
    called from the pre-retrieval hook function.

    @param ds_id      -  input datastream ID
    @param begin_time -  begin time in seconds since 1970.
    @param end_time   -  end time in seconds since 1970.
    """
    dsproc_set_retriever_time_offsets(ds_id, begin_time, end_time)

#char *dsproc_lib_version()


def get_status():
    return dsproc_get_status()

#char *dsproc_get_type()
#char *dsproc_get_version()
#
#time_t      dsproc_get_max_run_time()
#time_t      dsproc_get_start_time()
#time_t      dsproc_get_time_remaining()
#time_t      dsproc_get_min_valid_time()
#time_t      dsproc_get_data_interval()
#time_t      dsproc_get_processing_interval(time_t *begin, time_t *end)

def set_status(char *status):
    """Set the process status"""
    dsproc_set_status(status)

#int     dsproc_init_datastream(
#            char  *site,
#            char  *facility,
#            char  *dsc_name,
#            char  *dsc_level,
#            DSRole       role,
#            char  *path,
#            DSFormat     format,
#            int          flags)
#
#void    dsproc_update_datastream_data_stats(
#            int              ds_id,
#            int              num_records,
#            timeval_t *begin_time,
#            timeval_t *end_time)
#
#void    dsproc_update_datastream_file_stats(
#            int              ds_id,
#            double           file_size,
#            timeval_t *begin_time,
#            timeval_t *end_time)
#
#int     dsproc_validate_datastream_data_time(
#            int              ds_id,
#            timeval_t *data_time)
#
def set_datastream_flags(int ds_id, int flags):
    """
    Set the control flags for a datastream.
    DS_STANDARD_QC for all 'b' level datastreams
    DS_FILTER_NANS for all 'a' and 'b' level datastreams
    DS_OVERLAP_CHECK for all output datastreams
    Control flags:
    DS_STANDARD_QC = Apply standard QC before storing a dataset.
    DS_FILTER_NANS = Replace NaN and Inf values with missing values 
                     before storing a dataset.
    DS_OVERLAP_CHECK = Check for overlap with previously processed data. 
                       This flag will be ignored and the overlap check will 
                       be skipped if reprocessing mode is enabled.
    DS_PRESERVE_OBS = Preserve distinct observations when retrieving 
                      and storing data. Only observations that start within 
                      the current processing interval will be read in.
    DS_DISABLE_MERGE = Do not merge multiple observations in retrieved data. 
                       Only data for the current processing interval will be read in.
    DS_SKIP_TRANSFORM = Skip the transformation logic for all variables in this datastream.

    @param ds_id -  datastream ID
    @param flags -  flags to set
    """
    dsproc_set_datastream_flags(ds_id, flags)

#void    dsproc_set_datastream_format(int ds_id, DSFormat format)
def set_datastream_path(int ds_id, char *path):
    """
    Set the path to the datastream direcotry.

    Default datastream path set if path = NULL:
        dsenv_get_collection_dir() for level 0 input datastream
        dsenv_get_datastream_fir() for all other datastremas

    If an error occurs in this function it will be appended to the 
    log and error mail messages, and the process status will be 
    set appropriately.

    @param ds_id -  datastream ID
    @param path -   path to the datastream directory

    @return
    - 1 if successful
    - 0 if an error occurred
    """
    return(dsproc_set_datastream_path(ds_id, path))
#
#void    dsproc_unset_datastream_flags(int ds_id, int flags)
def unset_datastream_flags(int ds_id, int flags):
    """
    Unset the control flags for a datastream.
    See dsproc_set_datastream_flags() for flags and descriptions.

    @param ds_id -  datastream ID
    @param flags -  flags to set
    """
    dsproc_unset_datastream_flags(ds_id, flags)

def create_output_dataset(int ds_id, time_t data_time, int set_location ):

    cdef cds3.core.Group group
    cdef CDSGroup *cds_group = NULL
    cds_group = dsproc_create_output_dataset(ds_id, data_time, set_location)
    if cds_group == NULL:
        return None
    else:
        group = cds3.core.Group()
        group.set_group(cds_group)
        return group

#int     dsproc_dataset_pass_through(
#            CDSGroup *in_cds,
#            CDSGroup *out_cds,
#            int       flags)
#
def map_datasets(cds3.core.Group in_parent, cds3.core.Group out_dataset, int flags):
    """
    Map data from input datsets to output datasets.  
 
    This function maps all input data to output datasets if an output dataset is
    not specified.  By default only the data within the current processing 
    interval will be mapped to the output dataset.  This can be changed using the 
    set_map_time_range() function.  

    If an error occurs an error message will be appended to the log and error mail 
    messages, and the process status will be set appropriately.

    @param in_parent    - the parent group of all input datasets, typically the ret_data 
                          or trans_data parents.
    @param out_dataset  - the output dataset, or NULL to map all input data to output
                          datsets
    @param flags        - reserved for control flags
    
    @return
      -  1 if successful
      -  0 if an error occured
    """
    cdef int status

    status = dsproc_map_datasets(in_parent.c_ob, out_dataset.c_ob, flags)

    return status

def set_map_time_range(time_t begin_time, time_t end_time):
    """Set the time range to use in subsequent calls to map_datasets()

    @param begin_time - Only map data whose time is >= this begin time
    @param end_time   - Only map data whose time is < this end time

    """
    dsproc_set_map_time_range(begin_time, end_time)


#
#int     dsproc_map_var(
#            CDSVar *in_var,
#            size_t  in_sample_start,
#            size_t  sample_count,
#            CDSVar *out_var,
#            size_t  out_sample_start,
#            int     flags)
#
#int     dsproc_set_dataset_location(CDSGroup *cds)
#
def store_dataset(int ds_id, int newfile):
    """Store the output dataset.

    This function will
    - Filter out duplicate records in the dataset, and verify that the 
      record times are in chronological order. Duplicate records are
      defined has having identical times and data values.

    - Filter all NaN and Inf values for variables that have a missing
      value defined for datastreams that have the DS_FILTER_NANS flag 
      set. This should only be used if the DS_STANDARD_QC flag is also set,
      or for datasets that do not have any QC variables defined. 
 
    - Apply standard missing value, min, max, and delta QC checks for 
      atastreams that have the DS_STANDARD_QC flag set. This is the default
      or b level datastreams. (see the dsproc_set_datastream_flags() function).

    - Filter out all records that are duplicates of previously stored data, 
      and verify that the records do not overlap any previously stored data.
      This check is currently being skipped if we are in reprocessing mode, 
      and the file splitting mode is SPLIT_ON_STORE (the default for VAPs).

    - Verify that none of the record times are in the future.
 
    - Merge datasets with existing files and only split on defined intervals 
      or when metadata values change. The default for VAPs is to create a
      new file for every dataset stored, and the default for ingests is to
      create daily files that split at midnight UTC (see the 
      dsproc_set_datastream_split_mode() function). 

     If an error occurs in this function it will be appended to the log and 
     error mail messages, and the process status will be set appropriately.

    @param ds_id   - datastream ID
    @param newfile - specifies fi a new file should be created

    @return
      - number of samples stored
      - zero if no data found in dataset, or if all data samples were
        dumplicates of previously stored data
      - -1 if an error occured
    """
    return dsproc_store_dataset(ds_id, newfile)

def qc_limit_checks(cds3.core.Var var, cds3.core.Var qc_var, 
        int missing_flag, int min_flag, int max_flag):
    """Perform QC limit checks.

    This function will uses the following attributes to determine the missing_value,
    and min, max limits:

    - missing_value -r _FillValue, or default NetCDF fill value
    - valid_min
    - valid_max

    If a flag are set to zero that test is disabled

    If and error occurs in this function it will be appended to the log and error mail
    messages, and the process status will be set appropriately.
   

    @param var   - pointer to the variable
    @param qc_var - pointer to the qc variable
    @param misisng_flag - QC flag to use for missing_values
    @param min_flag - QC flag to use for values below the minimum
    @param max_flag - QC flag to use for values above the maximum

    @return
      - 1 if successful
      - 0 if an error occurred
    """
    return dsproc_qc_limit_checks(var.c_ob, qc_var.c_ob, missing_flag, min_flag, max_flag)

def get_bad_qc_mask(cds3.core.Var qc_var):
    """ Get the QC mask used to determine bad QC values.

    This function will use the bit assessment attributes to create a mask with all 
    bits set for bad assessment values. It will first check for field level bit 
    assessment attributes, and then for the global attributes if they are not found.

    The mask can be used to test to see if a bad bit is set by applying a bit-wise
    and operation.  If that returns zero then a bit with a bad assessment is set.

    @param qc_var - pointer to the QC variable
    
    @return qc_mask - the QC mask with all bad bits set.
    """

    return dsproc_get_bad_qc_mask(qc_var.c_ob)


#CDSAtt *dsproc_get_dsdod_att(
#            int         ds_id,
#            char *var_name,
#            char *att_name)
#
#CDSDim *dsproc_get_dsdod_dim(
#            int         ds_id,
#            char *dim_name)
#
#void   *dsproc_get_dsdod_att_value(
#            int          ds_id,
#            char  *var_name,
#            char  *att_name,
#            CDSDataType  type,
#            size_t      *length,
#            void        *value)
#
#char   *dsproc_get_dsdod_att_text(
#            int         ds_id,
#            char *var_name,
#            char *att_name,
#            size_t     *length,
#            char       *value)
#
#size_t  dsproc_get_dsdod_dim_length(
#            int         ds_id,
#            char *dim_name)
#int     dsproc_set_dsdod_att_value(
#            int          ds_id,
#            char  *var_name,
#            char  *att_name,
#            CDSDataType  type,
#            size_t       length,
#            void        *value)
#
#int     dsproc_set_dsdod_att_text(
#            int         ds_id,
#            char *var_name,
#            char *att_name,
#            char *format, ...)
#
#int     dsproc_set_dsdod_dim_length(
#            int         ds_id,
#            char *dim_name,
#            size_t      dim_length)
#
def set_input_source(char *status):
    """Set the input source string to use when new datasets are created.
       This function will set the string to use for the input_source 
       global attribute value when new datasets are created. This value 
       will only be set in datasets that have the input_source attribute 
       defined.
    """
    dsproc_set_status(status)

#int     dsproc_set_runtime_metadata(int ds_id, CDSGroup *cds)
#
#int     dsproc_update_datastream_dsdods(time_t data_time)
#
#int     dsproc_db_connect()

def db_disconnect():
    dsproc_db_disconnect()

#int     dsproc_get_input_ds_classes(DSClass ***ds_classes)
#int     dsproc_get_output_ds_classes(DSClass ***ds_classes)
#
#int     dsproc_get_location(ProcLoc **proc_loc)
#
#int     dsproc_get_config_value(
#            char  *config_key,
#            char       **config_value)
#
#int     dsproc_dqrdb_connect()
#void    dsproc_dqrdb_disconnect()
#
#void    dsproc_free_dqrs(DQR **dqrs)
#int     dsproc_get_dqrs(
#            char *site,
#            char *facility,
#            char *dsc_name,
#            char *dsc_level,
#            char *var_name,
#            time_t      start_time,
#            time_t      end_time,
#            DQR      ***dqrs)

#void dsproc_bad_file_warning(
#        char *sender,
#        char *func,
#        char *src_file,
#        int         src_line,
#        char *file_name,
#        char *format, ...)
#
#void dsproc_bad_line_warning(
#        char *sender,
#        char *func,
#        char *src_file,
#        int         src_line,
#        char *file_name,
#        int         line_num,
#        char *format, ...)
#
#void dsproc_bad_record_warning(
#        char *sender,
#        char *func,
#        char *src_file,
#        int         src_line,
#        char *file_name,
#        int         rec_num,
#        char *format, ...)
#
#int dsproc_load_transform_params_file(
#        CDSGroup   *group,
#        char *site,
#        char *facility,
#        char *name,
#        char *level)

def _ingest_main_loop():
    cdef int       ndsid
    cdef int      *dsids
    cdef int       dsid
    cdef const char *level
    cdef int       nfiles
    cdef char    **files
    cdef const char *input_dir
    cdef time_t    loop_start
    cdef time_t    loop_end
    cdef time_t    time_remaining
    cdef int       status
    cdef int       dsi
    cdef int       fi
    ndsids = dsproc_get_input_datastream_ids(&dsids)

    if ndsids == 0: 
        error("Could Not Find Input Datastream Class In Database\n","Could not find an input datastream defined in the database\n")
        return

    dsid = -1
    for dsi in range(ndsids):
        level = dsproc_datastream_class_level(dsids[dsi])
        if level[0] == '0':
            if dsid == -1: 
                dsid = dsids[dsi]             
            else:
                error("Could Not Find Input Datastream Class In Database\n",
                    "Too many level 0 input datastreams defined in database\n"
                    "  -> ingests only support one level 0 input datastream\n");
                return

    # Get the list of input files
    nfiles = dsproc_get_datastream_files(dsid, &files)
    if nfiles <= 0:
        if nfiles == 0:
            log("No data files found to process in: %s\n",
               dsproc_datastream_path(dsid))
        return

    input_dir = dsproc_datastream_path(dsid)

    # Loop over all input files
    loop_start = 0 
    loop_end = 0 
    for fi in range(nfiles):
        #Check the run time
        time_remaining = dsproc_get_time_remaining()
        if time_remaining >=0:
            if time_remaining == 0:
                break
            if loop_end - loop_start > time_remaining:
                log("\nStopping ingest before max run time of %d seconds is exceeded\n",
                   <int>dsproc_get_max_run_time())
                dsproc_set_status("Maximum Run Time Limit Exceeded")
                break 
    
        # Process the file
        val = fi+1
        debug_lv1("PROCESSING FILE #%d: %s\n", val, files[fi])
        log("\nProcessing: %s/%s\n", input_dir, files[fi])
        loop_start = time.time()
        dsproc_set_input_dir(input_dir)
        dsproc_set_input_source(files[fi])
        status = _run_process_file_hook(input_dir, files[fi])
        if status == -1:
            break
        loop_end = time.time()

    dsproc_free_file_list(files)

def _vap_main_loop(int proc_model):
    cdef time_t    interval_begin
    cdef time_t    interval_end
    cdef int       status
    cdef cds3.core.Group ret_data = cds3.core.Group()
    cdef cds3.core.Group trans_data = cds3.core.Group()
    while dsproc_start_processing_loop(&interval_begin, &interval_end):
        # Run the pre_retrieval_hook function
        status = _run_pre_retrieval_hook(interval_begin, interval_end)
        if status == -1: break
        if status ==  0: continue
        # Retrieve the data for the current processing interval
        if proc_model & DSP_RETRIEVER:
            status = dsproc_retrieve_data(
                    interval_begin, interval_end, &ret_data.c_ob)
            if status == -1: break
            if status ==  0: continue
        # Run the post_retrieval_hook function
        status = _run_post_retrieval_hook(
                interval_begin, interval_end, ret_data)
        if status == -1: break
        if status ==  0: continue
        # Merge the observations in the retrieved data
        if not dsproc_merge_retrieved_data(): break
        # Run the pre_transform_hook function
        status = _run_pre_transform_hook(
                interval_begin, interval_end, ret_data)
        if status == -1: break
        if status ==  0: continue
        # Perform the data transformations for transform VAPs
        if proc_model & DSP_TRANSFORM:
            status = dsproc_transform_data(&trans_data.c_ob)
            if status == -1: break
            if status ==  0: continue
        # Run the post_transform_hook function
        status = _run_post_transform_hook(
            interval_begin, interval_end, trans_data)
        if status == -1: break
        if status ==  0: continue
        # Create output datasets
        if not dsproc_create_output_datasets(): break
        # Run the user's data processing function
        if trans_data:
            status = _run_process_data_hook(
                interval_begin, interval_end, trans_data)
        else:
            status = _run_process_data_hook(
                interval_begin, interval_end, ret_data)
        if status == -1: break
        if status ==  0: continue
        # Store all output datasets
        if not dsproc_store_output_datasets(): break


def main(argv, proc_model, proc_version, proc_names=None):
    """Datasystem Process Main Function.
    
    @param  argc         - command line argument count
    @param  argv         - command line argument vector
    @param  proc_model   - processing model to use
    @param  proc_version - process version
    @param  nproc_names  - number of valid process names
    @param  proc_names   - list of valid process names
    
    @return
      - suggested program exit value
        (0 = successful, 1 = failure)

    """
    cdef int exit_value
    #****************************************************************
    # Initialize the data system process.
    #
    # This function will not return if the -h (help) or -v (version)
    # option was specified on the command line, or if an error occurs
    # (i.e. could not connect to database, the process is not defined
    # in the database, the log file could not be opened/created, a
    # memory allocation error, etc...).
    #****************************************************************
    if proc_names is None:
        proc_names = []
    initialize(argv, proc_model, proc_version, proc_names)

    #****************************************************************
    # Call the user's init_process() function.
    #
    # The _run_init_process_hook() function will call the
    # user's init_process() function if one was set using the
    # dsproc_set_init_process_hook() function.
    #
    # When writing bindings for other languages the target language
    # must implement its own methods for setting and calling the
    # various user defined hook functions, and for storing a reference
    # to the user defined data structure.
    #****************************************************************
    if not _run_init_process_hook():
        exit_value = finish()
        sys.exit(exit_value)

    #****************************************************************
    # Disconnect from the database until it is needed again
    #****************************************************************
    dsproc_db_disconnect()

    #****************************************************************
    # Call the appropriate data processing loop
    #****************************************************************
    if proc_model == PM_INGEST:
        _ingest_main_loop() 
    else:
        _vap_main_loop(proc_model)

    #****************************************************************
    # Call the user's finish_process() function.
    #
    # The _run_finish_process_hook() function will call the
    # user's finish_process() function if one was set using the
    # dsproc_set_finish_process_hook() function.
    #
    # When writing bindings for other languages the target language
    # must implement its own methods for setting and calling the
    # various user defined hook functions, and for storing a reference
    # to the user defined data structure.
    #****************************************************************
    _run_finish_process_hook()

    #****************************************************************
    # Finish the data system process.
    #
    # This function will update the database and logs with process
    # status and metrics, close the logs, send any generated mail
    # messages, and cleanup all allocated memory.
    #****************************************************************
    exit_value = finish()

    return exit_value
