/******************************************************************************
 *
 * Module Name: exdebug - Support for stores to the AML Debug Object
 *
 *****************************************************************************/

/******************************************************************************
 *
 * 1. Copyright Notice
 *
 * Some or all of this work - Copyright (c) 1999 - 2015, Intel Corp.
 * All rights reserved.
 *
 * 2. License
 *
 * 2.1. This is your license from Intel Corp. under its intellectual property
 * rights. You may have additional license terms from the party that provided
 * you this software, covering your right to use that party's intellectual
 * property rights.
 *
 * 2.2. Intel grants, free of charge, to any person ("Licensee") obtaining a
 * copy of the source code appearing in this file ("Covered Code") an
 * irrevocable, perpetual, worldwide license under Intel's copyrights in the
 * base code distributed originally by Intel ("Original Intel Code") to copy,
 * make derivatives, distribute, use and display any portion of the Covered
 * Code in any form, with the right to sublicense such rights; and
 *
 * 2.3. Intel grants Licensee a non-exclusive and non-transferable patent
 * license (with the right to sublicense), under only those claims of Intel
 * patents that are infringed by the Original Intel Code, to make, use, sell,
 * offer to sell, and import the Covered Code and derivative works thereof
 * solely to the minimum extent necessary to exercise the above copyright
 * license, and in no event shall the patent license extend to any additions
 * to or modifications of the Original Intel Code. No other license or right
 * is granted directly or by implication, estoppel or otherwise;
 *
 * The above copyright and patent license is granted only if the following
 * conditions are met:
 *
 * 3. Conditions
 *
 * 3.1. Redistribution of Source with Rights to Further Distribute Source.
 * Redistribution of source code of any substantial portion of the Covered
 * Code or modification with rights to further distribute source must include
 * the above Copyright Notice, the above License, this list of Conditions,
 * and the following Disclaimer and Export Compliance provision. In addition,
 * Licensee must cause all Covered Code to which Licensee contributes to
 * contain a file documenting the changes Licensee made to create that Covered
 * Code and the date of any change. Licensee must include in that file the
 * documentation of any changes made by any predecessor Licensee. Licensee
 * must include a prominent statement that the modification is derived,
 * directly or indirectly, from Original Intel Code.
 *
 * 3.2. Redistribution of Source with no Rights to Further Distribute Source.
 * Redistribution of source code of any substantial portion of the Covered
 * Code or modification without rights to further distribute source must
 * include the following Disclaimer and Export Compliance provision in the
 * documentation and/or other materials provided with distribution. In
 * addition, Licensee may not authorize further sublicense of source of any
 * portion of the Covered Code, and must include terms to the effect that the
 * license from Licensee to its licensee is limited to the intellectual
 * property embodied in the software Licensee provides to its licensee, and
 * not to intellectual property embodied in modifications its licensee may
 * make.
 *
 * 3.3. Redistribution of Executable. Redistribution in executable form of any
 * substantial portion of the Covered Code or modification must reproduce the
 * above Copyright Notice, and the following Disclaimer and Export Compliance
 * provision in the documentation and/or other materials provided with the
 * distribution.
 *
 * 3.4. Intel retains all right, title, and interest in and to the Original
 * Intel Code.
 *
 * 3.5. Neither the name Intel nor any other trademark owned or controlled by
 * Intel shall be used in advertising or otherwise to promote the sale, use or
 * other dealings in products derived from or relating to the Covered Code
 * without prior written authorization from Intel.
 *
 * 4. Disclaimer and Export Compliance
 *
 * 4.1. INTEL MAKES NO WARRANTY OF ANY KIND REGARDING ANY SOFTWARE PROVIDED
 * HERE. ANY SOFTWARE ORIGINATING FROM INTEL OR DERIVED FROM INTEL SOFTWARE
 * IS PROVIDED "AS IS," AND INTEL WILL NOT PROVIDE ANY SUPPORT, ASSISTANCE,
 * INSTALLATION, TRAINING OR OTHER SERVICES. INTEL WILL NOT PROVIDE ANY
 * UPDATES, ENHANCEMENTS OR EXTENSIONS. INTEL SPECIFICALLY DISCLAIMS ANY
 * IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGEMENT AND FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * 4.2. IN NO EVENT SHALL INTEL HAVE ANY LIABILITY TO LICENSEE, ITS LICENSEES
 * OR ANY OTHER THIRD PARTY, FOR ANY LOST PROFITS, LOST DATA, LOSS OF USE OR
 * COSTS OF PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES, OR FOR ANY INDIRECT,
 * SPECIAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THIS AGREEMENT, UNDER ANY
 * CAUSE OF ACTION OR THEORY OF LIABILITY, AND IRRESPECTIVE OF WHETHER INTEL
 * HAS ADVANCE NOTICE OF THE POSSIBILITY OF SUCH DAMAGES. THESE LIMITATIONS
 * SHALL APPLY NOTWITHSTANDING THE FAILURE OF THE ESSENTIAL PURPOSE OF ANY
 * LIMITED REMEDY.
 *
 * 4.3. Licensee shall not export, either directly or indirectly, any of this
 * software or system incorporating such software without first obtaining any
 * required license or other approval from the U. S. Department of Commerce or
 * any other agency or department of the United States Government. In the
 * event Licensee exports any such software from the United States or
 * re-exports any such software from a foreign destination, Licensee shall
 * ensure that the distribution and export/re-export of the software is in
 * compliance with all laws, regulations, orders, or other restrictions of the
 * U.S. Export Administration Regulations. Licensee agrees that neither it nor
 * any of its subsidiaries will export/re-export any technical data, process,
 * software, or service, directly or indirectly, to any country for which the
 * United States government or any agency thereof requires an export license,
 * other governmental approval, or letter of assurance, without first obtaining
 * such license, approval or letter.
 *
 *****************************************************************************/

#include "acpi.h"
#include "accommon.h"
#include "acnamesp.h"
#include "acinterp.h"
#include "acparser.h"


#define _COMPONENT          ACPI_EXECUTER
        ACPI_MODULE_NAME    ("exdebug")


static ACPI_OPERAND_OBJECT  *AcpiGbl_TraceMethodObject = NULL;

/* Local prototypes */

#ifdef ACPI_DEBUG_OUTPUT
static const char *
AcpiExGetTraceEventName (
    ACPI_TRACE_EVENT_TYPE   Type);
#endif


#ifndef ACPI_NO_ERROR_MESSAGES
/*******************************************************************************
 *
 * FUNCTION:    AcpiExDoDebugObject
 *
 * PARAMETERS:  SourceDesc          - Object to be output to "Debug Object"
 *              Level               - Indentation level (used for packages)
 *              Index               - Current package element, zero if not pkg
 *
 * RETURN:      None
 *
 * DESCRIPTION: Handles stores to the AML Debug Object. For example:
 *              Store(INT1, Debug)
 *
 * This function is not compiled if ACPI_NO_ERROR_MESSAGES is set.
 *
 * This function is only enabled if AcpiGbl_EnableAmlDebugObject is set, or
 * if ACPI_LV_DEBUG_OBJECT is set in the AcpiDbgLevel. Thus, in the normal
 * operational case, stores to the debug object are ignored but can be easily
 * enabled if necessary.
 *
 ******************************************************************************/

void
AcpiExDoDebugObject (
    ACPI_OPERAND_OBJECT     *SourceDesc,
    UINT32                  Level,
    UINT32                  Index)
{
    UINT32                  i;
    UINT32                  Timer;


    ACPI_FUNCTION_TRACE_PTR (ExDoDebugObject, SourceDesc);


    /* Output must be enabled via the DebugObject global or the DbgLevel */

    if (!AcpiGbl_EnableAmlDebugObject &&
        !(AcpiDbgLevel & ACPI_LV_DEBUG_OBJECT))
    {
        return_VOID;
    }

    /*
     * We will emit the current timer value (in microseconds) with each
     * debug output. Only need the lower 26 bits. This allows for 67
     * million microseconds or 67 seconds before rollover.
     */
    Timer = ((UINT32) AcpiOsGetTimer () / 10); /* (100 nanoseconds to microseconds) */
    Timer &= 0x03FFFFFF;

    /*
     * Print line header as long as we are not in the middle of an
     * object display
     */
    if (!((Level > 0) && Index == 0))
    {
        AcpiOsPrintf ("[ACPI Debug %.8u] %*s", Timer, Level, " ");
    }

    /* Display the index for package output only */

    if (Index > 0)
    {
       AcpiOsPrintf ("(%.2u) ", Index-1);
    }

    if (!SourceDesc)
    {
        AcpiOsPrintf ("[Null Object]\n");
        return_VOID;
    }

    if (ACPI_GET_DESCRIPTOR_TYPE (SourceDesc) == ACPI_DESC_TYPE_OPERAND)
    {
        AcpiOsPrintf ("%s ", AcpiUtGetObjectTypeName (SourceDesc));

        if (!AcpiUtValidInternalObject (SourceDesc))
        {
           AcpiOsPrintf ("%p, Invalid Internal Object!\n", SourceDesc);
           return_VOID;
        }
    }
    else if (ACPI_GET_DESCRIPTOR_TYPE (SourceDesc) == ACPI_DESC_TYPE_NAMED)
    {
        AcpiOsPrintf ("%s: %p\n",
            AcpiUtGetTypeName (((ACPI_NAMESPACE_NODE *) SourceDesc)->Type),
            SourceDesc);
        return_VOID;
    }
    else
    {
        return_VOID;
    }

    /* SourceDesc is of type ACPI_DESC_TYPE_OPERAND */

    switch (SourceDesc->Common.Type)
    {
    case ACPI_TYPE_INTEGER:

        /* Output correct integer width */

        if (AcpiGbl_IntegerByteWidth == 4)
        {
            AcpiOsPrintf ("0x%8.8X\n",
                (UINT32) SourceDesc->Integer.Value);
        }
        else
        {
            AcpiOsPrintf ("0x%8.8X%8.8X\n",
                ACPI_FORMAT_UINT64 (SourceDesc->Integer.Value));
        }
        break;

    case ACPI_TYPE_BUFFER:

        AcpiOsPrintf ("[0x%.2X]\n", (UINT32) SourceDesc->Buffer.Length);
        AcpiUtDumpBuffer (SourceDesc->Buffer.Pointer,
            (SourceDesc->Buffer.Length < 256) ?
                SourceDesc->Buffer.Length : 256, DB_BYTE_DISPLAY, 0);
        break;

    case ACPI_TYPE_STRING:

        AcpiOsPrintf ("[0x%.2X] \"%s\"\n",
            SourceDesc->String.Length, SourceDesc->String.Pointer);
        break;

    case ACPI_TYPE_PACKAGE:

        AcpiOsPrintf ("[Contains 0x%.2X Elements]\n",
            SourceDesc->Package.Count);

        /* Output the entire contents of the package */

        for (i = 0; i < SourceDesc->Package.Count; i++)
        {
            AcpiExDoDebugObject (SourceDesc->Package.Elements[i],
                Level+4, i+1);
        }
        break;

    case ACPI_TYPE_LOCAL_REFERENCE:

        AcpiOsPrintf ("[%s] ", AcpiUtGetReferenceName (SourceDesc));

        /* Decode the reference */

        switch (SourceDesc->Reference.Class)
        {
        case ACPI_REFCLASS_INDEX:

            AcpiOsPrintf ("0x%X\n", SourceDesc->Reference.Value);
            break;

        case ACPI_REFCLASS_TABLE:

            /* Case for DdbHandle */

            AcpiOsPrintf ("Table Index 0x%X\n", SourceDesc->Reference.Value);
            return_VOID;

        default:

            break;
        }

        AcpiOsPrintf ("  ");

        /* Check for valid node first, then valid object */

        if (SourceDesc->Reference.Node)
        {
            if (ACPI_GET_DESCRIPTOR_TYPE (SourceDesc->Reference.Node) !=
                    ACPI_DESC_TYPE_NAMED)
            {
                AcpiOsPrintf (" %p - Not a valid namespace node\n",
                    SourceDesc->Reference.Node);
            }
            else
            {
                AcpiOsPrintf ("Node %p [%4.4s] ", SourceDesc->Reference.Node,
                    (SourceDesc->Reference.Node)->Name.Ascii);

                switch ((SourceDesc->Reference.Node)->Type)
                {
                /* These types have no attached object */

                case ACPI_TYPE_DEVICE:
                    AcpiOsPrintf ("Device\n");
                    break;

                case ACPI_TYPE_THERMAL:
                    AcpiOsPrintf ("Thermal Zone\n");
                    break;

                default:

                    AcpiExDoDebugObject ((SourceDesc->Reference.Node)->Object,
                        Level+4, 0);
                    break;
                }
            }
        }
        else if (SourceDesc->Reference.Object)
        {
            if (ACPI_GET_DESCRIPTOR_TYPE (SourceDesc->Reference.Object) ==
                    ACPI_DESC_TYPE_NAMED)
            {
                AcpiExDoDebugObject (((ACPI_NAMESPACE_NODE *)
                    SourceDesc->Reference.Object)->Object,
                    Level+4, 0);
            }
            else
            {
                AcpiExDoDebugObject (SourceDesc->Reference.Object,
                    Level+4, 0);
            }
        }
        break;

    default:

        AcpiOsPrintf ("%p\n", SourceDesc);
        break;
    }

    ACPI_DEBUG_PRINT_RAW ((ACPI_DB_EXEC, "\n"));
    return_VOID;
}
#endif


/*******************************************************************************
 *
 * FUNCTION:    AcpiExInterpreterTraceEnabled
 *
 * PARAMETERS:  Name                - Whether method name should be matched,
 *                                    this should be checked before starting
 *                                    the tracer
 *
 * RETURN:      TRUE if interpreter trace is enabled.
 *
 * DESCRIPTION: Check whether interpreter trace is enabled
 *
 ******************************************************************************/

static BOOLEAN
AcpiExInterpreterTraceEnabled (
    char                    *Name)
{

    /* Check if tracing is enabled */

    if (!(AcpiGbl_TraceFlags & ACPI_TRACE_ENABLED))
    {
        return (FALSE);
    }

    /*
     * Check if tracing is filtered:
     *
     * 1. If the tracer is started, AcpiGbl_TraceMethodObject should have
     *    been filled by the trace starter
     * 2. If the tracer is not started, AcpiGbl_TraceMethodName should be
     *    matched if it is specified
     * 3. If the tracer is oneshot style, AcpiGbl_TraceMethodName should
     *    not be cleared by the trace stopper during the first match
     */
    if (AcpiGbl_TraceMethodObject)
    {
        return (TRUE);
    }
    if (Name &&
        (AcpiGbl_TraceMethodName &&
         ACPI_STRCMP (AcpiGbl_TraceMethodName, Name)))
    {
        return (FALSE);
    }
    if ((AcpiGbl_TraceFlags & ACPI_TRACE_ONESHOT) &&
        !AcpiGbl_TraceMethodName)
    {
        return (FALSE);
    }

    return (TRUE);
}


/*******************************************************************************
 *
 * FUNCTION:    AcpiExGetTraceEventName
 *
 * PARAMETERS:  Type            - Trace event type
 *
 * RETURN:      Trace event name.
 *
 * DESCRIPTION: Used to obtain the full trace event name.
 *
 ******************************************************************************/

#ifdef ACPI_DEBUG_OUTPUT

static const char *
AcpiExGetTraceEventName (
    ACPI_TRACE_EVENT_TYPE   Type)
{
    switch (Type)
    {
    case ACPI_TRACE_AML_METHOD:

        return "Method";

    case ACPI_TRACE_AML_OPCODE:

        return "Opcode";

    case ACPI_TRACE_AML_REGION:

        return "Region";

    default:

        return "";
    }
}

#endif


/*******************************************************************************
 *
 * FUNCTION:    AcpiExTracePoint
 *
 * PARAMETERS:  Type                - Trace event type
 *              Begin               - TRUE if before execution
 *              Aml                 - Executed AML address
 *              Pathname            - Object path
 *
 * RETURN:      None
 *
 * DESCRIPTION: Internal interpreter execution trace.
 *
 ******************************************************************************/

void
AcpiExTracePoint (
    ACPI_TRACE_EVENT_TYPE   Type,
    BOOLEAN                 Begin,
    UINT8                   *Aml,
    char                    *Pathname)
{

    ACPI_FUNCTION_NAME (ExTracePoint);


    if (Pathname)
    {
        ACPI_DEBUG_PRINT ((ACPI_DB_TRACE_POINT,
                "%s %s [0x%p:%s] execution.\n",
                AcpiExGetTraceEventName (Type), Begin ? "Begin" : "End",
                Aml, Pathname));
    }
    else
    {
        ACPI_DEBUG_PRINT ((ACPI_DB_TRACE_POINT,
                "%s %s [0x%p] execution.\n",
                AcpiExGetTraceEventName (Type), Begin ? "Begin" : "End",
                Aml));
    }
}


/*******************************************************************************
 *
 * FUNCTION:    AcpiExStartTraceMethod
 *
 * PARAMETERS:  MethodNode          - Node of the method
 *              ObjDesc             - The method object
 *              WalkState           - current state, NULL if not yet executing
 *                                    a method.
 *
 * RETURN:      None
 *
 * DESCRIPTION: Start control method execution trace
 *
 ******************************************************************************/

void
AcpiExStartTraceMethod (
    ACPI_NAMESPACE_NODE     *MethodNode,
    ACPI_OPERAND_OBJECT     *ObjDesc,
    ACPI_WALK_STATE         *WalkState)
{
    ACPI_STATUS             Status;
    char                    *Pathname = NULL;
    BOOLEAN                 Enabled = FALSE;


    ACPI_FUNCTION_NAME (ExStartTraceMethod);


    if (MethodNode)
    {
        Pathname = AcpiNsGetNormalizedPathname (MethodNode, TRUE);
    }

    Status = AcpiUtAcquireMutex (ACPI_MTX_NAMESPACE);
    if (ACPI_FAILURE (Status))
    {
        goto Exit;
    }

    Enabled = AcpiExInterpreterTraceEnabled (Pathname);
    if (Enabled && !AcpiGbl_TraceMethodObject)
    {
        AcpiGbl_TraceMethodObject = ObjDesc;
        AcpiGbl_OriginalDbgLevel = AcpiDbgLevel;
        AcpiGbl_OriginalDbgLayer = AcpiDbgLayer;
        AcpiDbgLevel = ACPI_TRACE_LEVEL_ALL;
        AcpiDbgLayer = ACPI_TRACE_LAYER_ALL;

        if (AcpiGbl_TraceDbgLevel)
        {
            AcpiDbgLevel = AcpiGbl_TraceDbgLevel;
        }
        if (AcpiGbl_TraceDbgLayer)
        {
            AcpiDbgLayer = AcpiGbl_TraceDbgLayer;
        }
    }
    (void) AcpiUtReleaseMutex (ACPI_MTX_NAMESPACE);

Exit:
    if (Enabled)
    {
        ACPI_TRACE_POINT (ACPI_TRACE_AML_METHOD, TRUE,
                ObjDesc ? ObjDesc->Method.AmlStart : NULL, Pathname);
        if (Pathname)
        {
        }
    }
    if (Pathname)
    {
        ACPI_FREE (Pathname);
    }
}


/*******************************************************************************
 *
 * FUNCTION:    AcpiExStopTraceMethod
 *
 * PARAMETERS:  MethodNode          - Node of the method
 *              ObjDesc             - The method object
 *              WalkState           - current state, NULL if not yet executing
 *                                    a method.
 *
 * RETURN:      None
 *
 * DESCRIPTION: Stop control method execution trace
 *
 ******************************************************************************/

void
AcpiExStopTraceMethod (
    ACPI_NAMESPACE_NODE     *MethodNode,
    ACPI_OPERAND_OBJECT     *ObjDesc,
    ACPI_WALK_STATE         *WalkState)
{
    ACPI_STATUS             Status;
    char                    *Pathname = NULL;
    BOOLEAN                 Enabled;


    ACPI_FUNCTION_NAME (ExStopTraceMethod);


    if (MethodNode)
    {
        Pathname = AcpiNsGetNormalizedPathname (MethodNode, TRUE);
    }

    Status = AcpiUtAcquireMutex (ACPI_MTX_NAMESPACE);
    if (ACPI_FAILURE (Status))
    {
        goto ExitPath;
    }

    Enabled = AcpiExInterpreterTraceEnabled (NULL);

    (void) AcpiUtReleaseMutex (ACPI_MTX_NAMESPACE);

    if (Enabled)
    {
        ACPI_TRACE_POINT (ACPI_TRACE_AML_METHOD, FALSE,
                ObjDesc ? ObjDesc->Method.AmlStart : NULL, Pathname);
        if (Pathname)
        {
        }
    }

    Status = AcpiUtAcquireMutex (ACPI_MTX_NAMESPACE);
    if (ACPI_FAILURE (Status))
    {
        goto ExitPath;
    }

    /* Check whether the tracer should be stopped */

    if (AcpiGbl_TraceMethodObject == ObjDesc)
    {
        /* Disable further tracing if type is one-shot */

        if (AcpiGbl_TraceFlags & ACPI_TRACE_ONESHOT)
        {
            AcpiGbl_TraceMethodName = NULL;
        }

        AcpiDbgLevel = AcpiGbl_OriginalDbgLevel;
        AcpiDbgLayer = AcpiGbl_OriginalDbgLayer;
        AcpiGbl_TraceMethodObject = NULL;
    }

    (void) AcpiUtReleaseMutex (ACPI_MTX_NAMESPACE);

ExitPath:
    if (Pathname)
    {
        ACPI_FREE (Pathname);
    }
}


/*******************************************************************************
 *
 * FUNCTION:    AcpiExStartTraceOpcode
 *
 * PARAMETERS:  Op                  - The parser opcode object
 *              WalkState           - current state, NULL if not yet executing
 *                                    a method.
 *
 * RETURN:      None
 *
 * DESCRIPTION: Start opcode execution trace
 *
 ******************************************************************************/

void
AcpiExStartTraceOpcode (
    ACPI_PARSE_OBJECT       *Op,
    ACPI_WALK_STATE         *WalkState)
{

    ACPI_FUNCTION_NAME (ExStartTraceOpcode);


    if (AcpiExInterpreterTraceEnabled (NULL) &&
        (AcpiGbl_TraceFlags & ACPI_TRACE_OPCODE))
    {
        ACPI_TRACE_POINT (ACPI_TRACE_AML_OPCODE, TRUE,
                Op->Common.Aml, Op->Common.AmlOpName);
    }
}


/*******************************************************************************
 *
 * FUNCTION:    AcpiExStopTraceOpcode
 *
 * PARAMETERS:  Op                  - The parser opcode object
 *              WalkState           - current state, NULL if not yet executing
 *                                    a method.
 *
 * RETURN:      None
 *
 * DESCRIPTION: Stop opcode execution trace
 *
 ******************************************************************************/

void
AcpiExStopTraceOpcode (
    ACPI_PARSE_OBJECT       *Op,
    ACPI_WALK_STATE         *WalkState)
{

    ACPI_FUNCTION_NAME (ExStopTraceOpcode);


    if (AcpiExInterpreterTraceEnabled (NULL) &&
        (AcpiGbl_TraceFlags & ACPI_TRACE_OPCODE))
    {
        ACPI_TRACE_POINT (ACPI_TRACE_AML_OPCODE, FALSE,
                Op->Common.Aml, Op->Common.AmlOpName);
    }
}
