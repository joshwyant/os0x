#pragma once

#include <efi.h>

// For getting the number of processors (and such)
// https://uefi.org/specs/PI/1.8/V2_DXE_Boot_Services_Protocols.html

#define EFI_MP_SERVICES_PROTOCOL_GUID \
    {0x3fdda605, 0xa76e, 0x4f46, {0xad, 0x29, 0x12, 0xf4, 0x53, 0x1b, 0x3d, 0x08}}

typedef struct _EFI_MP_SERVICES_PROTOCOL EFI_MP_SERVICES_PROTOCOL;

extern EFI_GUID gEfiMpServiceProtocolGuid;

typedef EFI_STATUS(EFIAPI *EFI_MP_SERVICES_GET_NUMBER_OF_PROCESSORS)(
    IN EFI_MP_SERVICES_PROTOCOL *This,
    OUT UINTN *NumberOfProcessors,
    OUT UINTN *NumberOfEnabledProcessors);

//*******************************************************
// StatusFlag Bits Definition
//*******************************************************
#define PROCESSOR_AS_BSP_BIT 0x00000001
#define PROCESSOR_ENABLED_BIT 0x00000002
#define PROCESSOR_HEALTH_STATUS_BIT 0x00000004

// EFI_CPU_PHYSICAL_LOCATION
//*******************************************************
typedef struct
{
    UINT32 Package;
    UINT32 Core;
    UINT32 Thread;
} EFI_CPU_PHYSICAL_LOCATION;

typedef struct
{
    UINT32 Package;
    UINT32 Module;
    UINT32 Tile;
    UINT32 Die;
    UINT32 Core;
    UINT32 Thread;
} EFI_CPU_PHYSICAL_LOCATION2;

#define CPU_V2_EXTENDED_TOPOLOGY BIT24
typedef union
{
    EFI_CPU_PHYSICAL_LOCATION2 Location2;
} EXTENDED_PROCESSOR_INFORMATION;

//*******************************************************
// EFI_PROCESSOR_INFORMATION
//*******************************************************
typedef struct
{
    UINT64 ProcessorId;
    UINT32 StatusFlag;
    EFI_CPU_PHYSICAL_LOCATION Location;
    EXTENDED_PROCESSOR_INFORMATION ExtendedInformation;
} EFI_PROCESSOR_INFORMATION;

typedef EFI_STATUS(EFIAPI *EFI_MP_SERVICES_GET_PROCESSOR_INFO)(
    IN EFI_MP_SERVICES_PROTOCOL *This,
    IN UINTN *ProcessorNumber,
    OUT EFI_PROCESSOR_INFORMATION *ProcessorInfoBuffer);

#define END_OF_CPU_LIST 0xffffffff

typedef VOID(EFIAPI *EFI_AP_PROCEDURE)(
    IN VOID *ProcedureArgument);

typedef EFI_STATUS(EFIAPI *EFI_MP_SERVICES_STARTUP_ALL_APS)(
    IN EFI_MP_SERVICES_PROTOCOL *This,
    IN EFI_AP_PROCEDURE Procedure,
    IN BOOLEAN SingleThread,
    IN EFI_EVENT WaitEvent OPTIONAL,
    IN UINTN TimeoutInMicroSeconds,
    IN VOID *ProcedureArgument OPTIONAL,
    OUT UINTN **FailedCpuList OPTIONAL);

typedef EFI_STATUS(EFIAPI *EFI_MP_SERVICES_STARTUP_THIS_AP)(
    IN EFI_MP_SERVICES_PROTOCOL *This,
    IN EFI_AP_PROCEDURE Procedure,
    IN UINTN ProcessorNumber,
    IN EFI_EVENT WaitEvent OPTIONAL,
    IN UINTN TimeoutInMicroseconds,
    IN VOID *ProcedureArgument OPTIONAL,
    OUT BOOLEAN *Finished OPTIONAL);

typedef EFI_STATUS(EFIAPI *EFI_MP_SERVICES_SWITCH_BSP)(
    IN EFI_MP_SERVICES_PROTOCOL *This,
    IN UINTN ProcessorNumber,
    IN BOOLEAN EnableOldBSP);

typedef EFI_STATUS(EFIAPI *EFI_MP_SERVICES_ENABLEDISABLEAP)(
    IN EFI_MP_SERVICES_PROTOCOL *This,
    IN UINTN ProcessorNumber,
    IN BOOLEAN EnableAP,
    IN UINT32 *HealthFlag OPTIONAL);

typedef EFI_STATUS(EFIAPI *EFI_MP_SERVICES_WHOAMI)(
    IN EFI_MP_SERVICES_PROTOCOL *This,
    OUT UINTN *ProcessorNumber);

typedef struct _EFI_MP_SERVICES_PROTOCOL
{
    EFI_MP_SERVICES_GET_NUMBER_OF_PROCESSORS GetNumberOfProcessors;
    EFI_MP_SERVICES_GET_PROCESSOR_INFO GetProcessorInfo;
    EFI_MP_SERVICES_STARTUP_ALL_APS StartupAllAPs;
    EFI_MP_SERVICES_STARTUP_THIS_AP StartupThisAP;
    EFI_MP_SERVICES_SWITCH_BSP SwitchBSP;
    EFI_MP_SERVICES_ENABLEDISABLEAP EnableDisableAP;
    EFI_MP_SERVICES_WHOAMI WhoAmI;
} EFI_MP_SERVICES_PROTOCOL;
