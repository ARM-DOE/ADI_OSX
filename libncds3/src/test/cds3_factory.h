/*******************************************************************************
*
*  COPYRIGHT (C) 2010 Battelle Memorial Institute.  All Rights Reserved.
*
********************************************************************************
*
*  Author:
*     name:  Brian Ermold
*     phone: (509) 375-2277
*     email: brian.ermold@pnl.gov
*
********************************************************************************
*
*  REPOSITORY INFORMATION:
*    $Revision: 6126 $
*    $Author: ermold $
*    $Date: 2011-03-24 06:56:57 +0000 (Thu, 24 Mar 2011) $
*    $Version: afl-libncds3-1.1-0 $
*
*******************************************************************************/

typedef struct DimDef   DimDef;
typedef struct AttDef   AttDef;
typedef struct VarDef   VarDef;
typedef struct GroupDef GroupDef;

struct DimDef {
    const char *name;
    size_t      length;
    int         is_unlimited;
};

struct AttDef {
    const char  *name;
    CDSDataType  type;
    size_t       length;
    void        *value;
};

struct VarDef {
    const char   *name;
    CDSDataType   type;
    const char  **dim_names;
    AttDef       *atts;
    int           nsamples;
    void         *data;
};

struct GroupDef {
    const char *name;
    DimDef     *dims;
    AttDef     *atts;
    VarDef     *vars;
    GroupDef   *groups;
};

int       cds_factory_define_dims(CDSGroup *group, DimDef *defs);
int       cds_factory_define_atts(void *parent, AttDef *defs);
int       cds_factory_define_vars(CDSGroup *group, VarDef *defs);
CDSGroup *cds_factory_define_groups(CDSGroup *parent, GroupDef *def);
