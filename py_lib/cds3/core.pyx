#cython: embedsignature=True
# file cds3.pyx

from libc.stdlib cimport malloc,free
from libc.stdio cimport fflush
from cpython.ref cimport PyObject
from cds3.ccds3 cimport *
from cds3.core cimport *
cimport ccds3_enums

# TODO: This is forcing python 3.1 or earlier i.e. does not work with 3.2
cdef extern from "Python.h":
    PyObject* PyCObject_FromVoidPtr(void *cobj, void (*destr)(void *))
# The stdio.pxd provided by Cython doesn't have this
cdef extern from *:
    ctypedef char const_char "const char"
cdef extern from "stdio.h" nogil:
    FILE *fdopen(int fildes, const_char *OPENTYPE)

# numpy
import numpy as np
cimport numpy as np
np.import_array()

cdef inline int cds_type_to_dtype(CDSDataType type) except -1:
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

cdef void _free(void *address):
    """Helper for freeing externally allocated memory 'owned' by ndarrays."""
    free(address)

##########################################################################
# generic attribute functions that apply to both Group and Var instances
##########################################################################

cdef inline Att _change_att(void *parent, int overwrite, char *name,
        CDSDataType type, object value):
    """Change an attribute of a CDS group or variable.
    
    This function will define the specified attribute if it does not exist.
    If the attribute does exist and the overwrite flag is set, the data type
    and value will be changed.
    
    Error messages from this function are sent to the message handler
    (see msngr_init_log() and msngr_init_mail()).
    
    @param parent - pointer to the parent CDSGroup or CDSVar
    @param overwrite - overwrite flag (1 = TRUE, 0 = FALSE)
    @param name - attribute name
    @param type - attribute data type
    @param value - Python value(s), internally converted to numpy.ndarray
    
    @retval the Att instance
    @retval None if the parent object is not a group or variable
    @retval None if the parent group or variable is locked, or the attribute is
            locked
    @retval None if a memory allocation error occurred

    """
    cdef CDSAtt *cds_att
    cdef Att att
    cdef np.ndarray value_nd = np.asarray(value, cds_type_to_dtype_obj(type))
    if value_nd.ndim == 0:
        value_nd = value_nd[None] # add dummy dimension to a scalar value
    assert value_nd.ndim == 1
    cds_att = cds_change_att(parent, overwrite, name, type,
            len(value_nd), value_nd.data)
    if cds_att == NULL:
        return None
    att = Att()
    att.set_att(cds_att)
    return att

cdef inline Att _define_att(void *parent, char *name, CDSDataType type,
        object value):
    """Define a CDS Attribute on a group or variable.
    
    This function will first check if an attribute with the same
    definition already exists in the specified group or variable.
    If it does, the existing attribute will be returned.
    
    Error messages from this function are sent to the message
    handler (see msngr_init_log() and msngr_init_mail()).
    
    @param parent - pointer to the parent CDSGroup or CDSVar
    @param name - attribute name
    @param type - attribute data type
    @param value - Python value(s), internally converted to numpy.ndarray
    
    @retval the Att instance
    @retval None if the parent object is not a group or variable
    @retval None if the parent group or variable is locked
    @retval None if a memory allocation error occurred
    @retval None if an attribute with the same name but different definition
            has already been defined
    
    """
    cdef CDSAtt *cds_att
    cdef Att att
    cdef np.ndarray value_nd = np.asarray(value, cds_type_to_dtype_obj(type))
    if value_nd.ndim == 0:
        value_nd = value_nd[None] # add dummy dimension to a scalar value
    assert value_nd.ndim == 1
    cds_att = cds_define_att(parent, name, type, len(value_nd), value_nd.data)
    if cds_att == NULL:
        return None
    att = Att()
    att.set_att(cds_att)
    return att

cdef inline Att _define_att_text(void *parent, char *name, char *format):
    """Define a CDS Text Attribute for a group or variable.
    
    See cds_define_att() for details.
    
    Error messages from this function are sent to the message
    handler (see msngr_init_log() and msngr_init_mail()).
    
    @param parent - pointer to the parent CDSGroup or CDSVar
    @param name - attribute name
    @param format - string value
    
    @retval the Att instance
    @retval None if the parent object is not a group or variable
    @retval None if the parent group or variable is locked
    @retval None if a memory allocation error occurred
    @retval None if an attribute with the same name but a differnt value
            has already been defined
    
    """
    cdef CDSAtt *cds_att
    cdef Att att
    cds_att = cds_define_att_text(parent, name, format)
    if cds_att == NULL:
        return None
    att = Att()
    att.set_att(cds_att)
    return att

cdef inline Att _get_att(void *parent, char *name):
    """Get a CDS Attribute from a group or variable.
    
    This function will search the specified parent group
    or variable for an attribute with the specified name.
    
    @param parent - pointer to the parent group or variable
    @param name - name of the attribute
    
    @retval the Att instance
    @retval None if not found
    
    """
    cdef CDSAtt *cds_att
    cdef Att att
    cds_att = cds_get_att(parent, name)
    if cds_att == NULL:
        return None
    att = Att()
    att.set_att(cds_att)
    return att

cdef inline Att _set_att(void *parent, int overwrite, char *name,
        CDSDataType type, object value):
    """Set an attribute of a CDS group or variable.
    
    This function will define the specified attribute if it does not exist.
    If the attribute does exist and the overwrite flag is set, the value will
    be set by casting the specified value into the data type of the attribute.
    The functions cds_string_to_array() and cds_array_to_string() are used to
    convert between text (CDS_CHAR) and numeric data types.
    
    Error messages from this function are sent to the message handler
    (see msngr_init_log() and msngr_init_mail()).
    
    @param parent - pointer to the parent CDSGroup or CDSVar
    @param overwrite - overwrite flag (1 = TRUE, 0 = FALSE)
    @param name - attribute name
    @param type - attribute data type
    @param value - pointer to the attribute value
    
    @retval the Att instance
    @retval None if the parent object is not a group or variable
    @retval None if the parent group or variable is locked,
            or the attribute is locked
    @retval None if a memory allocation error occurred

    """
    cdef CDSAtt *cds_att
    cdef Att att
    cdef np.ndarray value_nd = np.asarray(value, cds_type_to_dtype_obj(type))
    if value_nd.ndim == 0:
        value_nd = value_nd[None] # add dummy dimension to a scalar value
    assert value_nd.ndim == 1
    cds_att = cds_set_att(parent, overwrite, name, type,
            len(value_nd), value_nd.data)
    if cds_att == NULL:
        return None
    att = Att()
    att.set_att(cds_att)
    return att


##########################################################################
# class definitions
##########################################################################


cdef class Object:
    """CDSObject"""

    def __cinit__(self):
        """TODO"""
        self.cds_object = NULL

    def __init__(self):
        """TODO"""
        self.user_data = {}

    cdef set_object(self, CDSObject *obj):
        """TODO"""
        self.cds_object = obj

    def __dealloc__(self):
        """TODO"""
        pass

    def get_name(self):
        """Returns the object name."""
        return self.cds_object.name

    def get_path(self):
        """Returns the object path."""
        return cds_get_object_path(self.cds_object)

    def set_definition_lock(self, int value):
        """Sets the definition lock."""
        cds_set_definition_lock(self.cds_object, value)

    def delete_user_data(self, char *key):
        """Deletes the user data with the specified key."""
        if key in self.user_data:
            del self.user_data[key]

    def get_user_data(self, char *key):
        """Returns the user data with the specified key, or None if invalid."""
        return self.user_data.get(key)

    def set_user_data(self, char *key, value):
        """Sets the user data with the given key to the given value."""
        self.user_data[key] = value
        return 1

    def get_user_data_dict(self):
        """Return all user data key/value entries as a dictionary."""
        return self.user_data

    def get_parent(self):
        """Get the parent of this object, or None if there is no parent."""
        cdef CDSObject *parent = NULL
        cdef CDSObjectType type
        cdef Group group
        cdef Dim dim
        cdef Att att
        cdef Var var
        cdef VarGroup vargroup
        cdef VarArray vararray
        parent = self.cds_object.parent
        if not parent:
            return None
        else:
            type = parent.obj_type
        if type == ccds3_enums.CDS_GROUP:
            group = Group()
            group.set_group(<CDSGroup*>parent)
            return group
        elif type == ccds3_enums.CDS_DIM:
            dim = Dim()
            dim.set_dim(<CDSDim*>parent)
            return dim
        elif type == ccds3_enums.CDS_ATT:
            att = Att()
            att.set_att(<CDSAtt*>parent)
            return att
        elif type == ccds3_enums.CDS_VAR:
            var = Var()
            var.set_var(<CDSVar*>parent)
            return var
        elif type == ccds3_enums.CDS_VARGROUP:
            vargroup = VarGroup()
            vargroup.set_vargroup(<CDSVarGroup*>parent)
            return vargroup
        elif type == ccds3_enums.CDS_VARARRAY:
            vararray = VarArray()
            vararray.set_vararray(<CDSVarArray*>parent)
            return vararray
        else:
            raise ValueError, "CDSObjectType not recognized"

    def get_obj_type(self):
        """Returns the object type."""
        return self.cds_object.obj_type

    def get_def_lock(self):
        """Returns the definition lock."""
        return self.cds_object.def_lock


cdef class Group(Object):
    """CDSGroup"""

    ###################################################################
    # Factory (static) methods
    ###################################################################

    @staticmethod
    def define(Group parent, char *name):
        """Define a CDS Group.
        
        This function will first check if a group with the same name
        already exists in the parent group. If it does, the existing
        group will be returned.
        
        Error messages from this function are sent to the message
        handler (see msngr_init_log() and msngr_init_mail()).
        
        @param parent - pointer to the parent group, or
                None to create the root group
        @param name - group name
        
        @return the Group instance
        @retval None if the parent group is locked
        @retval None if a memory allocation error occurred
        
        """
        cdef CDSGroup *cds_group
        cdef Group group
        if parent is None:
            cds_group = cds_define_group(NULL, name)
        else:
            cds_group = cds_define_group(parent.c_ob, name)
        if cds_group == NULL:
            return None
        group = Group()
        group.set_group(cds_group)
        return group

    @staticmethod
    def delete(Group group):
        """Delete a CDS Group.
        
        Error messages from this function are sent to the message
        handler (see msngr_init_log() and msngr_init_mail()).
        
        @param group - the Group instance
        
        @retval 1 if the group was deleted
        @retval 0 if the group is locked
        @retval 0 if the parent group is locked

        """
        cdef int retval = cds_delete_group(group.c_ob)
        if retval == 1:
            group.set_group(NULL)
            del group
        return retval

    ###################################################################
    # Cython initialization/destruction of the class.
    ###################################################################

    def __cinit__(self):
        """TODO"""
        self.c_ob = NULL

    def __init__(self):
        """TODO"""
        pass

    cdef set_group(self, CDSGroup *group):
        """TODO"""
        self.cds_object = <CDSObject*>group
        self.c_ob = group

    def __dealloc__(self):
        """TODO"""
        pass

    ###################################################################
    # Getters for the CDSGroup struct.
    ###################################################################

    def get_ndims(self):
        """Returns the number of dimensions for this Group."""
        return self.c_ob.ndims

    def get_dims(self):
        """Returns all Dim instances for this Group."""
        cdef int ndims = self.c_ob.ndims
        cdef Dim dim
        dims = []
        for i in range(ndims):
            dim = Dim()
            dim.set_dim(self.c_ob.dims[i])
            dims.append(dim)
        return dims

    def get_natts(self):
        return self.c_ob.natts

    def get_atts(self):
        cdef int natts = self.c_ob.natts
        cdef Att att
        atts = []
        for i in range(natts):
            att = Att()
            att.set_att(self.c_ob.atts[i])
            atts.append(att)
        return atts

    def get_nvars(self):
        return self.c_ob.nvars

    def get_vars(self):
        cdef int nvars = self.c_ob.nvars
        cdef Var var
        vars = []
        for i in range(nvars):
            var = Var()
            var.set_var(self.c_ob.vars[i])
            vars.append(var)
        return vars

    def get_ngroups(self):
        return self.c_ob.ngroups

    def get_groups(self):
        cdef int ngroups = self.c_ob.ngroups
        cdef Group group
        groups = []
        for i in range(ngroups):
            group = Group()
            group.set_group(self.c_ob.groups[i])
            groups.append(group)
        return groups

    def get_nvargroups(self):
        return self.c_ob.nvargroups

    def get_vargroups(self):
        cdef int nvargroups = self.c_ob.nvargroups
        cdef VarGroup vargroup
        vargroups = []
        for i in range(nvargroups):
            vargroup = vargroup()
            vargroup.set_vargroup(self.c_ob.vargroups[i])
            vargroups.append(vargroup)
        return vargroups

    ###################################################################
    # The following functions are based on the cds_* functions
    ###################################################################

    def rename(self, char *name):
        """Rename a CDS Group.
        
        Error messages from this function are sent to the message
        handler (see msngr_init_log() and msngr_init_mail()).
        
        @param name - pointer to the new group name
        
        @retval 1 if the group was renamed
        @retval 0 if a group with the new name already exists
        @retval 0 if the group is locked
        @retval 0 if the parent group is locked
        @retval 0 if a memory allocation error occured
        
        """
        return cds_rename_group(self.c_ob, name)

    def define_dim(self, char *name, size_t length, int is_unlimited):
        """Define a CDS Dimension.
        
        This function will first check if a dimension with the same definition
        already exists in the specified group. If it does, the existing
        dimension will be returned.
        
        Error messages from this function are sent to the message handler (see
        msngr_init_log() and msngr_init_mail()).
        
        @param name - dimension name
        @param length - dimension length
        @param is_unlimited - is dimension unlimited (0 = FALSE, 1 = TRUE)
        
        @retval the Dim instance
        @retval None if a static dimension with the same name but different
                length has already been defined for the specified group.
        @retval None if the group is locked
        @retval None if a memory allocation error occurred
        
        """
        cdef CDSDim *c_dim = cds_define_dim(
                self.c_ob, name, length, is_unlimited)
        if c_dim == NULL:
            return None
        dim = Dim()
        dim.set_dim(c_dim)
        return dim

    def get_dim(self, char *name):
        """Get a CDS Dimension.
        
        This function will search the specified group then and all ancestor
        groups for a dimension with the specified name.  The first dimension
        found will be returned.
        
        @param name - name of the dimension
        
        @retval the Dim
        @retval None if not found
        
        """
        cdef CDSDim *c_dim = cds_get_dim(self.c_ob, name)
        if c_dim == NULL:
            return None
        dim = Dim()
        dim.set_dim(c_dim)
        return dim

    def change_att(self, int overwrite, char *name, CDSDataType type,
            object value):
        """Change an attribute of a CDS group or variable.
        
        This function will define the specified attribute if it does not exist.
        If the attribute does exist and the overwrite flag is set, the data
        type and value will be changed.
        
        Error messages from this function are sent to the message handler
        (see msngr_init_log() and msngr_init_mail()).
        
        @param overwrite - overwrite flag (1 = TRUE, 0 = FALSE)
        @param name - attribute name
        @param type - attribute data type
        @param value - Python value(s), internally converted to numpy.ndarray
        
        @retval the Att instance
        @retval None if the parent object is not a group or variable
        @retval None if the parent group or variable is locked, or the
                attribute is locked
        @retval None if a memory allocation error occurred

        """
        return _change_att(self.c_ob, overwrite, name, type, value)

    def define_att(self, char *name, CDSDataType type, object value):
        """Define a CDS Attribute.

        This function will first check if an attribute with the same definition
        already exists in the specified group or variable.  If it does, the
        existing attribute will be returned.
        
        Error messages from this function are sent to the message
        handler (see msngr_init_log() and msngr_init_mail()).
        
        @param name - attribute name
        @param type - attribute data type
        @param length - attribute length
        @param value - Python value(s), internally converted to numpy.ndarray
        
        @retval the Att instance
        @retval None if the parent object is not a group or variable
        @retval None if the parent group or variable is locked
        @retval None if a memory allocation error occurred
        @retval None if an attribute with the same name but different
                definition has already been defined

        """
        return _define_att(self.c_ob, name, type, value)

    def define_att_text(self, char *name, char *text):
        """Define a CDS Text Attribute.
        
        See cds_define_att() for details.
        
        Error messages from this function are sent to the message
        handler (see msngr_init_log() and msngr_init_mail()).
        
        @param name - attribute name
        @param format - string value
        
        @retval the new Att
        @retval None if the parent object is not a group or variable
        @retval None if the parent group or variable is locked
        @retval None if a memory allocation error occurred
        @retval None if an attribute with the same name but a differnt value
                has already been defined
        
        """
        return _define_att_text(self.c_ob, name, text)

    def get_att(self, char *name):
        """Get a CDS Attribute.

        This function will search the specified parent group or variable for an
        attribute with the specified name.
        
        @param name - name of the attribute
        
        @retval the Att
        @retval None if not found

        """
        return _get_att(self.c_ob, name)

    def set_att(self, int overwrite, char *name, CDSDataType type,
            object value):
        """Set an attribute of a CDS group or variable.
        
        This function will define the specified attribute if it does not exist.
        If the attribute does exist and the overwrite flag is set, the value
        will be set by casting the specified value into the data type of the
        attribute.  The functions cds_string_to_array() and
        cds_array_to_string() are used to convert between text (CDS_CHAR) and
        numeric data types.
        
        Error messages from this function are sent to the message handler
        (see msngr_init_log() and msngr_init_mail()).
        
        @param overwrite - overwrite flag (1 = TRUE, 0 = FALSE)
        @param name - attribute name
        @param type - attribute data type
        @param value - Python value(s), internally converted to numpy.ndarray
        
        @retval the Att instance
        @retval None if the parent object is not a group or variable
        @retval None if the parent group or variable is locked, or the
                attribute is locked
        @retval None if a memory allocation error occurred
        
        """
        return _set_att(self.c_ob, overwrite, name, type, value)

    def define_var(self, char *name, CDSDataType type, object dim_names):
        """Define a CDS Variable.
        
        This function will first check if a variable with the same definition
        already exists in the specified group. If it does, the existing
        variable will be returned.
        
        Error messages from this function are sent to the message
        handler (see msngr_init_log() and msngr_init_mail()).
        
        @param name - variable name
        @param type - variable data type
        @param dim_names - iterable of strings or a single string
        
        @retval the Var instance
        @retval None if the group is locked
        @retval None if a dimension has not been defined
        @retval None if the unlimited dimension is not first
        @retval None if a memory allocation error occurred
        @retval None if a variable with the same name but different definition
                has already been defined
        
        """
        cdef CDSVar *c_var = NULL
        cdef size_t ndims = 0
        cdef const char **c_dim_names = NULL
        if isinstance(dim_names, basestring):
            ndims = 1
            c_dim_names = <const_char**>malloc(ndims * sizeof(char*))
            c_dim_names[0] = dim_names
        else:
            ndims = len(dim_names)
            c_dim_names = <const_char**>malloc(ndims * sizeof(char*))
            for idx in range(ndims):
                c_dim_names[idx] = dim_names[idx]
        c_var = cds_define_var(self.c_ob, name, type, ndims, c_dim_names)
        free(c_dim_names)
        if c_var == NULL:
            return None
        var = Var()
        var.set_var(c_var)
        return var

    def get_var(self, char *name):
        """Get a CDS Variable.
        
        This function will search the specified group for a variable with the
        specified name.
        
        @param name - name of the variable
        
        @retval the Var instance
        @retval None if not found
        
        """
        cdef CDSVar *c_var = cds_get_var(self.c_ob, name)
        if c_var == NULL:
            return None
        var = Var()
        var.set_var(c_var)
        return var

    def get_group(self, char *name):
        """Search for a group with the specificed name within this group.
        
        @param name - name of the child group
        
        @retval the child Group instance
        @retval None if not found
        
        """
        cdef CDSGroup *cds_group
        cdef Group group
        cds_group = cds_get_group(self.c_ob, name)
        if cds_group == NULL:
            return None
        group = Group()
        group.set_group(cds_group)
        return group

    def reset_sample_counts(self, int unlim_vars, int static_vars):
        cds_reset_sample_counts(self.c_ob, unlim_vars, static_vars)


cdef class Dim(Object):
    """CDSDim"""

    @staticmethod
    def delete(Dim dim):
        """Delete a CDS Dimension.
        
        This function will also delete all variables that use the specified
        dimension.
        
        Error messages from this function are sent to the message handler
        (see msngr_init_log() and msngr_init_mail()).
        
        @param dim - the Dim instance
        
        @retval 1 if the dimension was deleted
        @retval 0 if the dimension is locked
        @retval 0 if the group is locked
        
        """
        cdef int retval = cds_delete_dim(dim.c_ob)
        if retval == 1:
            dim.set_dim(NULL)
            del dim
        return retval

    def __cinit__(self):
        """TODO"""
        self.c_ob = NULL

    def __init__(self):
        """TODO"""
        pass

    cdef set_dim(self, CDSDim *dim):
        """TODO"""
        self.cds_object = <CDSObject*>dim
        self.c_ob = dim

    def __dealloc__(self):
        """TODO"""
        pass

    def get_length(self):
        """Get the length of a CDS Dimension.
        
        @retval the length
        
        """
        return self.c_ob.length

    def is_unlimited(self):
        """Indicates whether a CDS Dimension is unlimited.
        
        @retval True if unlimited
        @retval False otherwise
        
        """
        if self.c_ob.is_unlimited == 0:
            return False
        return True

    def change_length(self, size_t length):
        """Change the length of a CDS Dimension.
        
        Error messages from this function are sent to the message handler
        (see msngr_init_log() and msngr_init_mail()).
        
        @param length - new dimension length
        
        @retval 1 if the dimension length was changed
        @retval 1 if the new length was equal to the old length
        @retval 1 if this is an unlimited dimension
        @retval 0 if the dimension is locked
        @retval 0 if data has already been added to a variable using this
                dimension
        
        """
        return cds_change_dim_length(self.c_ob, length)

    def get_var(self):
        """Get the coordinate variable for a CDS Dimension.
        
        @retval the coordinate Var instance
        @retval None if not found
        
        """
        cdef CDSVar *cds_var
        cdef Var var
        cds_var = cds_get_dim_var(self.c_ob)
        if cds_var == NULL:
            return None
        var = Var()
        var.set_var(cds_var)
        return var

    def rename(self, char *name):
        """Rename a CDS Dimension.
        
        This function will also rename the coordinate variable for the
        dimension if one exists.
        
        Error messages from this function are sent to the message handler
        (see msngr_init_log() and msngr_init_mail()).
        
        @param name - pointer to the new dimension name
        
        @retval 1 if the dimension was deleted
        @retval 0 if a dimension with the new name already exists
        @retval 0 if the dimension is locked
        @retval 0 if the group is locked
        @retval 0 if the coordinate variable could not be renamed (see
                cds_rename_var())
        @retval 0 if a memory allocation error occured
        
        """
        return cds_rename_dim(self.c_ob, name)


cdef class Att(Object):
    """CDSAtt"""

    @staticmethod
    def delete(Att att):
        """Delete a CDS Attribute.
        
        Error messages from this function are sent to the message
        handler (see msngr_init_log() and msngr_init_mail()).
        
        @param att - pointer to the attribute
        
        @retval 1 if the attribute was deleted
        @retval 0 if the attribute is locked
        @retval 0 if the parent group or variable is locked
        
        """
        cdef int retval = cds_delete_att(att.c_ob)
        if retval == 1:
            att.set_att(NULL)
            del att
        return retval

    def __cinit__(self):
        """TODO"""
        self.c_ob = NULL

    def __init__(self):
        """TODO"""
        pass

    cdef set_att(self, CDSAtt *att):
        """TODO"""
        self.cds_object = <CDSObject*>att
        self.c_ob = att

    def __dealloc__(self):
        """TODO"""
        pass

    def get_type(self):
        return self.c_ob.type

    def get_length(self):
        return self.c_ob.length

    def change_text(self, char *text):
        """Change the type and value of a CDS Attribute.
        
        This function will change the data type of an attribute to CDS_CHAR and
        set the new value.
        
        Error messages from this function are sent to the message
        handler (see msngr_init_log() and msngr_init_mail()).
        
        @param text - the new value
        
        @retval 1 if the attribute value was changed
        @retval 0 if the attribute is locked
        @retval 0 if a memory allocation error occurred
        
        """
        return cds_change_att_text(self.c_ob, text)

    def change_value(self, CDSDataType type, object value):
        """Change the type and value of a CDS Attribute.
        
        Error messages from this function are sent to the message
        handler (see msngr_init_log() and msngr_init_mail()).
        
        @param type - new attribute data type
        @param value - Python value(s), internally converted to numpy.ndarray
        
        @retval 1 if successful
        @retval 0 if the attribute is locked
        @retval 0 if a memory allocation error occurred
        
        """
        cdef np.ndarray value_nd = np.asarray(value, cds_type_to_dtype_obj(type))
        if value_nd.ndim == 0:
            value_nd = value_nd[None] # add dummy dimension to a scalar value
        assert value_nd.ndim == 1
        return cds_change_att_value(self.c_ob, type, len(value_nd),
                value_nd.data)

    def get_text(self):
        """Get a copy of a CDS attribute value.
        
        This function will get a copy of an attribute value converted to a text
        string. If the data type of the attribute is not CDS_CHAR the
        cds_array_to_string() function is used to create the output string.
        
        Memory will be allocated for the returned string if the output string
        is NULL. In this case the calling process is responsible for freeing
        the allocated memory.
        
        Error messages from this function are sent to the message handler
        (see msngr_init_log() and msngr_init_mail()).
        
        @retval the string
        @retval None if the attribute value has zero length (length == 0)
        @retval None if a memory allocation error occurs (length == (size_t)-1)

        """
        cdef size_t length
        cdef char *retval
        cdef object retval_py
        retval = cds_get_att_text(self.c_ob, &length, NULL)
        if retval == NULL:
            return None
        retval_py = retval # copies the string to a Python string
        free(retval)
        return retval

    def get_value(self, CDSDataType type):
        """Get a copy of a CDS attribute value.
        
        This function will get a copy of an attribute value casted into the
        specified data type. The functions cds_string_to_array() and
        cds_array_to_string() are used to convert between text (CDS_CHAR) and
        numeric data types.
        
        Memory will be allocated for the returned array if the output array is
        NULL. In this case the calling process is responsible for freeing the
        allocated memory.
        
        Error messages from this function are sent to the message handler
        (see msngr_init_log() and msngr_init_mail()).
        
        @param type - data type of the output array
        
        @retval the output ndarray
        @retval None if the attribute value has zero length (length == 0)
        @retval None if a memory allocation error occurs (length == (size_t)-1)

        """
        cdef void* value_ptr
        cdef size_t length
        cdef np.ndarray value_nd
        cdef np.npy_intp dims
        value_ptr = cds_get_att_value(self.c_ob, type, &length, NULL)
        dims = length
        value_nd = np.PyArray_SimpleNewFromData(1, &dims,
                cds_type_to_dtype(type), value_ptr)
        # allow numpy to reclaim memory when array goes out of scope
        value_nd.base = PyCObject_FromVoidPtr(value_ptr, _free)
        return value_nd

    def rename(self, char *name):
        """Rename a CDS Attribute.
        
        Error messages from this function are sent to the message
        handler (see msngr_init_log() and msngr_init_mail()).
        
        @param name - pointer to the new attribute name
        
        @retval 1 if the attribute was deleted
        @retval 0 if an attribute with the new name already exists
        @retval 0 if the attribute is locked
        @retval 0 if the parent group or variable is locked
        @retval 0 if a memory allocation error occured
        
        """
        return cds_rename_att(self.c_ob, name)

    def set_text(self, text):
        """Set the value of a CDS attribute.
        
        The cds_string_to_array() function will be used to set the attribute
        value if the data type of the attribute is not CDS_CHAR.
        
        Error messages from this function are sent to the message
        handler (see msngr_init_log() and msngr_init_mail()).
        
        @param text to set
        
        @retval 1 if successful
        @retval 0 if the attribute is locked
        @retval 0 if a memory allocation error occurred
        
        """
        return cds_set_att_text(self.c_ob, text)

    def set_value(self, CDSDataType type, object value):
        """Set the value of a CDS attribute.
        
        This function will set the value of an attribute by casting the
        specified value into the data type of the attribute. The functions
        cds_string_to_array() and cds_array_to_string() are used to convert
        between text (CDS_CHAR) and numeric data types.
        
        Error messages from this function are sent to the message
        handler (see msngr_init_log() and msngr_init_mail()).
        
        @param type - data type of the specified value
        @param value - pointer to the attribute value
        
        @retval 0 if the attribute is locked
        @retval 0 if a memory allocation error occurred
        
        """
        cdef np.ndarray value_nd = np.asarray(value, cds_type_to_dtype_obj(type))
        if value_nd.ndim == 0:
            value_nd = value_nd[None] # add dummy dimension to a scalar value
        assert value_nd.ndim == 1
        return cds_set_att_value(self.c_ob, type, len(value_nd),
                value_nd.data)


cdef class Var(Object):
    """CDSVar"""

    ###################################################################
    # Factory (static) methods
    ###################################################################
    @staticmethod
    def delete(Var var):
        """Delete a CDS Variable.
        
        Error messages from this function are sent to the message
        handler (see msngr_init_log() and msngr_init_mail()).
        
        @param var - pointer to the variable
        
        @retval 1 if the variable was deleted
        @retval 0 if the variable is locked
        @retval 0 if the group is locked
        
        """
        cdef int retval = cds_delete_var(var.c_ob)
        if retval == 1:
            var.set_var(NULL)
            del var
        return retval

    ###################################################################
    # Cython initialization/destruction of the class.
    ###################################################################

    def __cinit__(self):
        """TODO"""
        self.c_ob = NULL

    def __init__(self):
        """TODO"""
        pass

    cdef set_var(self, CDSVar *var):
        """TODO"""
        self.cds_object = <CDSObject*>var
        self.c_ob = var

    def __dealloc__(self):
        """TODO"""
        pass

    ###################################################################
    # Getters for the CDSVar struct.
    ###################################################################

    def get_type(self):
        return self.c_ob.type

    def get_ndims(self):
        return self.c_ob.ndims

    def get_dims(self):
        cdef int ndims = self.c_ob.ndims
        cdef Dim dim
        dims = []
        for i in range(ndims):
            dim = Dim()
            dim.set_dim(self.c_ob.dims[i])
            dims.append(dim)
        return dims

    def get_natts(self):
        return self.c_ob.natts

    def get_atts(self):
        cdef int natts = self.c_ob.natts
        cdef Att att
        atts = []
        for i in range(natts):
            att = Att()
            att.set_att(self.c_ob.atts[i])
            atts.append(att)
        return atts

    def get_sample_count(self):
        return self.c_ob.sample_count

    property sample_count:
        def __get__(self):
            return self.c_ob.sample_count

    def get_alloc_count(self):
        return self.c_ob.alloc_count

    def get_default_fill(self):
        cdef void *fill_ptr = self.c_ob.default_fill
        cdef CDSDataType type = self.c_ob.type
        if fill_ptr == NULL:
            return None
        elif type == CDS_NAT:
            return None
        elif type == CDS_CHAR:
            return (<char*>fill_ptr)[0]
        elif type == CDS_BYTE:
            return (<signed char*>fill_ptr)[0]
        elif type == CDS_SHORT:
            return (<short*>fill_ptr)[0]
        elif type == CDS_INT:
            return (<int*>fill_ptr)[0]
        elif type == CDS_FLOAT:
            return (<float*>fill_ptr)[0]
        elif type == CDS_DOUBLE:
            return (<double*>fill_ptr)[0]

    ###################################################################
    # The following functions are based on the cds_* functions
    ###################################################################

    def get_coord_var(self, int dim_index):
        """Get the coordinate variable for a CDS Variable's dimension.
        
        @param dim_index - index of the dimension to get the coordinate
        variable for
        
        @retval pointer to the coordinate variable
        @retval None if not found
        
        """
        cdef CDSVar *c_var = cds_get_coord_var(self.c_ob, dim_index)
        if c_var == NULL:
            return None
        var = Var()
        var.set_var(c_var)
        return var

    def rename(self, char *name):
        """Rename a CDS Variable.
        
        Error messages from this function are sent to the message handler
        (see msngr_init_log() and msngr_init_mail()).
        
        @param name - pointer to the new variable name
        
        @retval 1 if the variable was renamed
        @retval 0 if a variable with the new name already exists
        @retval 0 if the variable is locked
        @retval 0 if the group is locked
        @retval 0 if a memory allocation error occured
        
        """
        return cds_rename_var(self.c_ob, name)

    def is_unlimited(self):
        """Check if a CDS Variable has an unlimited dimension.
        
        @retval 1 if the variable has an unlimited dimension
        @retval 0 if this variable does not have an unlimited dimension
        
        """
        return cds_var_is_unlimited(self.c_ob)

    def has_dim(self, char *name):
        """Check if a CDS Variable has the specified dimension.
        
        @param name - name of the dimension
        
        @retval pointer to the dimension
        @retval None if not found
        
        """
        cdef CDSDim *c_dim = cds_var_has_dim(self.c_ob, name)
        if c_dim == NULL:
            return None
        dim = Dim()
        dim.set_dim(c_dim)
        return dim

    def get_sample_size(self):
        """Get the sample size of a CDS Variable.
        
        The sample dimension is always the first dimension defined
        for a variable. If this dimension has unlimited length it
        is also refered to as the record dimension.
        
        Variables with less than 2 dimensions will always have a
        sample_size of 1. The sample_size for variables with 2 or
        more dimensions is the product of all the dimension lengths
        starting with the 2nd dimension.
        
        @return sample size of the variable
        
        """
        return cds_var_sample_size(self.c_ob)

    def alloc_data(self, size_t sample_start, size_t sample_count):
        """Allocate memory for a CDS variable's data array.
        
        This function will allocate memory as necessary to ensure that the
        variable's data array is large enough to store another sample_count
        samples starting from sample_start. The calling process must cast the
        returned data pointer to the proper data type if it is going to be used
        directly. The cds_set_var_data() function can be used if the variable's
        data type is not known at compile time.
        
        The data array returned by this function belongs to the CDS variable
        and will be freed when the variable is destroyed. The calling process
        must *not* attempt to free this memory.
        
        The memory allocated by this function will *not* be initialized. It
        is the responsibility of the calling process to set the data values.
        If necessary, the cds_init_array() function can be used to initialize
        this memory. However, if the specified start sample is greater than the
        variable's current sample count, the hole between the two will be filled
        with the first missing value defined for the variable. The search order
        for missing values is:
        
        - missing_value attribute
        - _FillValue attribute
        - variable's default missing value
        
        If the variable does not have any missing or fill values defined the
        default fill value for the variable's data type will be used and the
        default fill value for the variable will be set.
        
        This function will also update the length of the variable's first
        dimension if it is unlimited and its length is less than sample_start
        plus sample_count.
        
        Error messages from this function are sent to the message handler
        (see msngr_init_log() and msngr_init_mail()).
        
        @param sample_start - start sample (0 based indexing)
        @param sample_count - number of new samples
        
        @retval pointer to the specifed start sample in the variable data array
        @retval None if the specified sample count is zero
        @retval None if one of the variable's static dimensions has 0 length
        @retval None if the variable has no dimensions, and sample_start is not equal
                to 0 or sample_count is not equal to 1.
        @retval None if the first variable dimension is not unlimited, and
                sample_start + sample_count would exceed the dimension length.
        @retval None if a memory allocation error occurred
        
        """
        cdef void *ptr
        ptr = cds_alloc_var_data(self.c_ob, sample_start, sample_count)
        if ptr == NULL:
            return None
        return self.get_datap()[sample_start:]

    def alloc_data_index(self, size_t sample_start, size_t sample_count):
        """
        Allocate memory for a CDS variable's data array.
        
        This function behaves the same as cds_alloc_var_data() except that it
        returns a data index starting at the specified start sample, see
        cds_create_data_index() for details. For variables that have less than
        two dimensions this function is identical to cds_alloc_var_data().
        
        The data index returned by this function belongs to the CDS variable
        and will be freed when the variable is destroyed. The calling process
        must *not* attempt to free this memory.
        
        Error messages from this function are sent to the message handler
        (see msngr_init_log() and msngr_init_mail()).
        
        @param var - pointer to the variable
        @param sample_start - start sample (0 based indexing)
        @param sample_count - number of new samples
        
        @retval the data index into the variables data array starting at the
                specified start sample.
        @retval None if the specified sample count is zero
        @retval None if one of the variable's static dimensions has 0 length
        @retval None if the variable has no dimensions, and sample_start is not
                equal to 0 or sample_count is not equal to 1.
        @retval None if the first variable dimension is not unlimited, and
                sample_start + sample_count would exceed the dimension length.
        @retval None if a memory allocation error occurred

        """
        cdef void *ptr = NULL
        ptr = cds_alloc_var_data_index(self.c_ob, sample_start, sample_count)
        if ptr == NULL:
            return None
        return self.get_datap()

    def change_type(self, CDSDataType type):
        """Change the data type of a CDS Variable.
        
        This function will change the data type of a CDS variable. All data and
        data attribute values (see cds_add_data_att()) will be converted to the
        new data type.
        
        All missing values defined for the variable will be mapped to the new
        data type using the cds_get_missing_values_map() function.
        
        If the range of the new data type is less than the range of the
        variable's data type, all out-of-range values will be set to the
        min/max values of the new data type.
        
        The variable's data index will also be destroyed if the size of the new
        data type is not equal to the size of the old data type. The calling
        process is responsible for recreating the data index if necessary (see
        cds_create_var_data_index()).
        
        Error messages from this function are sent to the message handler (see
        msngr_init_log() and msngr_init_mail()).
        
        @param type - new data type
        
        @retval 1 if successful
        @retval 0 if an error occurred

        """
        return cds_change_var_type(self.c_ob, type)

    def change_units(self, CDSDataType type, char *units):
        """Change the units of a CDS Variable.
        
        This function will change the data type and units of a CDS variable.
        All data and data attribute values (see cds_add_data_att()) will be
        converted to the new data type and units.
        
        All missing values defined for the variable will be mapped to the new
        data type using the cds_get_missing_values_map() function.
        
        If the range of the new data type is less than the range of the
        variable's data type, all out-of-range values will be set to the
        min/max values of the new data type.
        
        The variable's data index will also be destroyed if the size of the new
        data type is not equal to the size of the old data type. The calling
        process is responsible for recreating the data index if necessary (see
        cds_create_var_data_index()).
        
        Error messages from this function are sent to the message handler (see
        msngr_init_log() and msngr_init_mail()).
        
        @param type - new data type
        @param units - new units
        
        @retval 1 if successful
        @retval 0 if an error occurred

        """
        return cds_change_var_units(self.c_ob, type, units)

    def create_data_index(self):
        cdef void *ptr
        ptr = cds_get_var_datap(self.c_ob, 0)
        if ptr == NULL:
            return None
        return self.get_datap()

    def delete_data(self):
        """Delete the data for a CDS variable."""
        cds_delete_var_data(self.c_ob)

    def get_data(self, CDSDataType type, size_t sample_start):
        """Get the data from a CDS variable.
        
        This function will get the data from a variable casted into the
        specified data type. All missing values used in the data will be
        converted to a single missing value appropriate for the requested data
        type. The missing value used will be the first value returned by the
        cds_get_missing_values_map() function. If no missing values are defined
        for the variable data, the missing_value returned will be the default
        fill value for the requested data type.
        
        If the range of the output data type is less than the range of the
        variable's data type, all out-of-range values will be set to the
        min/max values of the output data type.
        
        Memory will be allocated for the returned data array if the output
        array is NULL. In this case the calling process is responsible for
        freeing the allocated memory. If an output data array is specified it
        must be large enough to hold (sample_count * cds_var_sample_size(var))
        values of the specified data type.
        
        For multi-dimensional variables, the values in the output data array
        will be stored linearly in memory with the last dimension varying the
        fastest.
        
        Error messages from this function are sent to the message handler (see
        msngr_init_log() and msngr_init_mail()).
        
        @param type - data type of the output missing_value and data array
        @param sample_start - start sample (0 based indexing)
        @retval missing_value - output: missing value
        @param data - pointer to the output data array
        or NULL to dynamically allocate the memory needed.
        
        @retval pointer to the output data array
        @retval None if the variable has no data for sample_start
                (sample_count == 0)
        @retval None if a memory allocation error occurs
                (sample_count == (size_t)-1)
        
        """
        cdef void *ptr
        cdef np.ndarray array
        cdef np.npy_intp *dims
        cdef int ndims = self.c_ob.ndims
        cdef size_t sample_count
        cdef void *missing_ptr
        cdef char missing_char
        cdef signed char missing_signed_char
        cdef short missing_short
        cdef int missing_int
        cdef float missing_float
        cdef double missing_double
        cdef object missing_py
        if type == CDS_NAT:
            raise ValueError, "CDS_NAT"
        elif type == CDS_CHAR:
            missing_ptr = &missing_char
        elif type == CDS_BYTE:
            missing_ptr = &missing_signed_char
        elif type == CDS_SHORT:
            missing_ptr = &missing_short
        elif type == CDS_INT:
            missing_ptr = &missing_int
        elif type == CDS_FLOAT:
            missing_ptr = &missing_float
        elif type == CDS_DOUBLE:
            missing_ptr = &missing_double
        else:
            raise ValueError, "Unknown CDSDataType"
        ptr = cds_get_var_data(self.c_ob, type, sample_start, &sample_count,
                missing_ptr, NULL)
        if ptr == NULL:
            return None,None
        dims = <np.npy_intp*>malloc(sizeof(np.npy_intp) * ndims)
        for i in range(ndims):
            dims[i] = self.c_ob.dims[i].length
        dims[0] = sample_count
        array = np.PyArray_SimpleNewFromData(ndims, dims,
                cds_type_to_dtype(type), ptr)
        # allow numpy to reclaim memory when array goes out of scope
        array.base = PyCObject_FromVoidPtr(ptr, _free)
        free(dims)
        if type == CDS_NAT:
            raise ValueError, "CDS_NAT"
        elif type == CDS_CHAR:
            missing_py = missing_char
        elif type == CDS_BYTE:
            missing_py = missing_signed_char
        elif type == CDS_SHORT:
            missing_py = missing_short
        elif type == CDS_INT:
            missing_py = missing_int
        elif type == CDS_FLOAT:
            missing_py = missing_float
        elif type == CDS_DOUBLE:
            missing_py = missing_double
        else:
            raise ValueError, "Unknown CDSDataType"
        return array,missing_py

    cpdef np.ndarray get_datap(self, size_t sample_start=0):
        """Get an ndarray for the the data in a CDS variable.
        
        @param sample_start - start sample
        
        @retval the variable data as an ndarray
        @retval None if the variable has no data for sample_start
        
        """
        cdef void *ptr = NULL
        cdef size_t sample_count = self.c_ob.sample_count
        cdef np.ndarray array
        cdef np.npy_intp *dims
        cdef int ndims = self.c_ob.ndims
        cdef np.dtype type = cds_type_to_dtype_obj(self.c_ob.type)
        ptr = cds_get_var_datap(self.c_ob, 0)
        if ptr == NULL:
            return None
        dims = <np.npy_intp*>malloc(sizeof(np.npy_intp) * ndims)
        for i in range(ndims):
            dims[i] = self.c_ob.dims[i].length
        dims[0] = sample_count
        array = np.PyArray_SimpleNewFromData(ndims, dims, type.num, ptr)
        free(dims)
        if 0 == sample_start:
            return array
        else:
            return array[sample_start:]

    def get_missing_values(self):
        # WHERE JEFF LEFT OFF
        raise NotImplementedError, "TODO"

    def get_units(self):
        return cds_get_var_units(self.c_ob)

    def init_data(self, size_t sample_start, size_t sample_count,
            int use_missing):
        raise NotImplementedError, "TODO"

    def init_data_index(self, size_t sample_start, size_t sample_count,
            int use_missing):
        """Initialize the data values for a CDS variable.
        
        This function behaves the same as cds_init_var_data() except that it
        returns a data index starting at the specified start sample, see
        cds_create_data_index() for details. For variables that have less than
        two dimensions this function is identical to cds_init_var_data().
        
        The data index returned by this function belongs to the CDS variable
        and will be freed when the variable is destroyed. The calling process
        must *not* attempt to free this memory.
        
        Error messages from this function are sent to the message handler
        (see msngr_init_log() and msngr_init_mail()).
        
        @param var - pointer to the variable
        @param sample_start - start sample of the data to initialize
        (0 based indexing)
        @param sample_count - number of samples to initialize
        @param use_missing - flag indicating if the variables missing value
        should be used (1 == TRUE, 0 == fill with zeros)
        
        @retval the data index into the variables data array starting at the
                specified start sample.
        @retval None if the specified sample count is zero
        @retval None if one of the variable's static dimensions has 0 length
        @retval None if the variable has no dimensions, and sample_start is not
                equal to 0 or sample_count is not equal to 1.
        @retval None if the first variable dimension is not unlimited, and
                sample_start + sample_count would exceed the dimension length.
        @retval None if a memory allocation error occurred

        """
        cdef void *ptr = cds_init_var_data_index(self.c_ob, sample_start,
                sample_count, use_missing)
        if ptr == NULL:
            return None
        return self.get_datap(sample_start)

    def set_default_fill_value(self, object fill_value):
        raise NotImplementedError, "TODO"

    def set_data(self, CDSDataType type, size_t sample_start, size_t
            sample_count, object missing_value, object data):
        raise NotImplementedError, "TODO"


cdef class VarGroup(Object):
    """CDSVarGroup"""

    def __cinit__(self):
        self.c_ob = NULL

    def __init__(self):
        pass

    cdef set_vargroup(self, CDSVarGroup *vargroup):
        self.cds_object = <CDSObject*>vargroup
        self.c_ob = vargroup

    def __dealloc__(self):
        pass


cdef class VarArray(Object):
    """CDSVarArray"""

    def __cinit__(self):
        self.c_ob = NULL

    def __init__(self):
        pass

    cdef set_vararray(self, CDSVarArray *vararray):
        self.cds_object = <CDSObject*>vararray
        self.c_ob = vararray

    def __dealloc__(self):
        pass


def print_all(object file_like, Group group, int flags):
    cdef FILE *fp
    cdef int fd
    file_like.flush()
    fd = file_like.fileno()
    fp = fdopen(fd, "w")
    assert fp != NULL
    cds_print(fp, group.c_ob, flags)
    fflush(fp)

def print_att(object file_like, char *indent, int  min_width, Att att):
    cdef FILE *fp
    cdef int fd
    file_like.flush()
    fd = file_like.fileno()
    fp = fdopen(fd, "w")
    assert fp != NULL
    cds_print_att(fp, indent, min_width, att.c_ob)
    fflush(fp)

def print_atts(object file_like, char *indent, Object parent):
    cdef FILE *fp
    cdef int fd
    file_like.flush()
    fd = file_like.fileno()
    fp = fdopen(fd, "w")
    assert fp != NULL
    cds_print_atts(fp, indent, parent.cds_object)
    fflush(fp)

def print_dim(object file_like, char *indent, int min_width, Dim dim):
    cdef FILE *fp
    cdef int fd
    file_like.flush()
    fd = file_like.fileno()
    fp = fdopen(fd, "w")
    assert fp != NULL
    cds_print_dim(fp, indent, min_width, dim.c_ob)
    fflush(fp)

def print_dims(object file_like, char *indent, Group group):
    cdef FILE *fp
    cdef int fd
    file_like.flush()
    fd = file_like.fileno()
    fp = fdopen(fd, "w")
    assert fp != NULL
    cds_print_dims(fp, indent, group.c_ob)
    fflush(fp)

def print_var(object file_like, char *indent, Var var, int flags):
    cdef FILE *fp
    cdef int fd
    file_like.flush()
    fd = file_like.fileno()
    fp = fdopen(fd, "w")
    assert fp != NULL
    cds_print_var(fp, indent, var.c_ob, flags)
    fflush(fp)

def print_vars(object file_like, char *indent, Group group, int flags):
    cdef FILE *fp
    cdef int fd
    file_like.flush()
    fd = file_like.fileno()
    fp = fdopen(fd, "w")
    assert fp != NULL
    cds_print_vars(fp, indent, group.c_ob, flags)
    fflush(fp)

def print_var_data(object file_like, char *label, char *indent, Var var):
    cdef FILE *fp
    cdef int fd
    file_like.flush()
    fd = file_like.fileno()
    fp = fdopen(fd, "w")
    assert fp != NULL
    cds_print_var_data(fp, label, indent, var.c_ob)
    fflush(fp)

def print_data(object file_like, char *indent, Group group):
    cdef FILE *fp
    cdef int fd
    file_like.flush()
    fd = file_like.fileno()
    fp = fdopen(fd, "w")
    assert fp != NULL
    cds_print_data(fp, indent, group.c_ob)
    fflush(fp)

def print_group(object file_like, char *indent, Group group, int flags):
    cdef FILE *fp
    cdef int fd
    file_like.flush()
    fd = file_like.fileno()
    fp = fdopen(fd, "w")
    assert fp != NULL
    cds_print_group(fp, indent, group.c_ob, flags)
    fflush(fp)

def print_groups(object file_like, char *indent, Group group, int flags):
    cdef FILE *fp
    cdef int fd
    file_like.flush()
    fd = file_like.fileno()
    fp = fdopen(fd, "w")
    assert fp != NULL
    cds_print_groups(fp, indent, group.c_ob, flags)
    fflush(fp)

def print_vararray(object file_like, char *indent, VarArray vararray, int flags):
    cdef FILE *fp
    cdef int fd
    file_like.flush()
    fd = file_like.fileno()
    fp = fdopen(fd, "w")
    assert fp != NULL
    cds_print_vararray(fp, indent, vararray.c_ob, flags)
    fflush(fp)

def print_vargroup(object file_like, char *indent, VarGroup vargroup, int flags):
    cdef FILE *fp
    cdef int fd
    file_like.flush()
    fd = file_like.fileno()
    fp = fdopen(fd, "w")
    assert fp != NULL
    cds_print_vargroup(fp, indent, vargroup.c_ob, flags)
    fflush(fp)

def print_vargroups(object file_like, char *indent, Group group, int flags):
    cdef FILE *fp
    cdef int fd
    file_like.flush()
    fd = file_like.fileno()
    fp = fdopen(fd, "w")
    assert fp != NULL
    cds_print_vargroups(fp, indent, group.c_ob, flags)
    fflush(fp)
