#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "vcl.h"
#include "vrt.h"
#include "cache/cache.h"
#include "vcc_if.h"

#include "vtree.h"

extern char **environ;

typedef struct variable {
    unsigned magic;
    #define VARIABLE_MAGIC 0xcb181fe6
    const char *name;
    const char *value;
    VRB_ENTRY(variable) tree;
} variable_t;

typedef VRB_HEAD(variables, variable) variables_t;

static int
variablecmp(const variable_t *v1, const variable_t *v2)
{
    return strcmp(v1->name, v2->name);
}

VRB_PROTOTYPE_STATIC(variables, variable, tree, variablecmp);
VRB_GENERATE_STATIC(variables, variable, tree, variablecmp);

static unsigned loads = 0;
static variables_t variables;

/******************************************************************************
 * VMOD EVENTS.
 *****************************************************************************/

static variable_t *
new_variable(char *name, size_t len, char *value)
{
    variable_t *result;
    ALLOC_OBJ(result, VARIABLE_MAGIC);
    AN(result);

    result->name = strndup(name, len);
    AN(result->name);

    result->value = strdup(value);
    AN(result->value);

    return result;
}

static void
free_variable(variable_t *variable)
{
    free((void *) variable->name);
    variable->name = NULL;

    free((void *) variable->value);
    variable->value = NULL;

    FREE_OBJ(variable);
}

static void
init()
{
    VRB_INIT(&variables);
    for (int i = 0; environ[i]; i++) {
        char *ptr = strchr(environ[i], '=');
        if (ptr != NULL) {
            variable_t *variable = new_variable(
                environ[i], ptr - environ[i], ptr + 1);
            VRB_INSERT(variables, &variables, variable);
        }
    }
}

static void
fini()
{
    variable_t *variable, *tmp;
    VRB_FOREACH_SAFE(variable, variables, &variables, tmp) {
        CHECK_OBJ_NOTNULL(variable, VARIABLE_MAGIC);
        VRB_REMOVE(variables, &variables, variable);
        free_variable(variable);
    }
}

int
event_function(VRT_CTX, struct vmod_priv *vcl_priv, enum vcl_event_e e)
{
    switch (e) {
        case VCL_EVENT_LOAD:
            if (loads == 0) {
                init();
            }
            loads++;
            break;

        case VCL_EVENT_DISCARD:
            assert(loads > 0);
            loads--;
            if (loads == 0) {
                fini();
            }
            break;

        default:
            break;
    }

    return 0;
}

/******************************************************************************
 * env.is_set();
 * env.get();
 *****************************************************************************/

static variable_t *
find(const char *name)
{
    variable_t variable;
    variable.name = name;
    return VRB_FIND(variables, &variables, &variable);
}

VCL_BOOL
vmod_is_set(VRT_CTX, VCL_STRING name)
{
    return find(name) != NULL;
}

VCL_STRING
vmod_get(VRT_CTX, VCL_STRING name)
{
    const char *result = NULL;

    variable_t *variable = find(name);
    if (variable != NULL) {
        CHECK_OBJ_NOTNULL(variable, VARIABLE_MAGIC);
        if (ctx->ws != NULL) {
            result = WS_Copy(ctx->ws, variable->value, -1);
            AN(result);
        } else {
            result = variable->value;
        }
    }

    return result;
}
