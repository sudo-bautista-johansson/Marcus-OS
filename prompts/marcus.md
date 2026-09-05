# MARCUS OS — KRYO BARE-METAL DEVELOPMENT PROMPT

Quiero desarrollar un sistema operativo real llamado **MARCUS OS**, construido principalmente utilizando mi lenguaje de programación de sistemas **Kryo**.

El asistente de IA del sistema se llama **Marcus**.

El objetivo NO es crear una simulación de un sistema operativo, un mockup, una aplicación que parezca un OS ni una demo falsa.

El objetivo es crear un **sistema operativo realmente arrancable**, inicialmente para x86_64/QEMU, cuyo kernel esté escrito predominantemente en Kryo.

---

# 1. ESTADO ACTUAL DE KRYO

Antes de realizar cambios, inspecciona el compilador existente.

Kryo actualmente dispone de un pipeline aproximadamente así:

```text
Source (.kyo)
    ↓
Lexical Analysis
    ↓
Parsing
    ↓
Name Resolution
    ↓
Type Checking
    ↓
Borrow Checking
    ↓
Constant Propagation
    ↓
Dead Code Elimination
    ↓
HIR
    ↓
MIR
    ↓
LLVM IR
    ↓
clang / LLVM
    ↓
lld-link
    ↓
Executable
```

El compilador actualmente puede demostrar:

* funciones;
* recursion;
* if/else;
* while;
* structs;
* tuples;
* arrays fijos;
* slices;
* Option;
* Result;
* HashMap;
* Math;
* async/await;
* Futures;
* Memory Domains;
* Bounded types;
* BoundedVec;
* contratos `requires`;
* y borrow checking.

También existe integración con LLVM 16.

Un build actualmente produce LLVM IR como:

```llvm
define dso_local i32 @main() {
  entry:
  %0 = alloca i32
  store i32 10, i32* %0
  ret i32 30
}
```

El objetivo ahora es llevar Kryo desde su estado actual de compilador de propósito general hacia un lenguaje capaz de producir código **bare-metal**.

---

# 2. REGLA FUNDAMENTAL

NO inventes características de Kryo.

NO asumas que una característica es realmente implementada sólo porque existe una sintaxis para ella.

Para cada característica, determina:

```text
Syntax
   ↓
AST
   ↓
HIR
   ↓
MIR
   ↓
LLVM IR
   ↓
Machine Code
   ↓
Runtime behavior
```

Si algo solamente está simulado en el runtime, documentarlo como tal.

Si algo existe en el parser pero no llega correctamente a LLVM, implementarlo.

Si algo funciona en userspace pero no puede utilizarse bare-metal, determinar qué dependencia lo impide.

---

# 3. BUGS Y LIMITACIONES ACTUALES

Durante la auditoría debes investigar explícitamente estos problemas observados.

## HashMap

El showcase ejecuta:

```kryo
let mut system_registry = HashMap::new();

system_registry.insert("core_freq_mhz", 4200);
system_registry.insert("active_threads", 16);
system_registry.insert("cache_lines", 64);

let total_registry_entries = system_registry.len();
```

pero produce:

```text
HashMap total de claves: 0
```

Investigar y corregir el problema.

No modificar simplemente el output esperado.

El `HashMap` debe representar correctamente su estado real.

---

# 4. LINTER

El showcase produce warnings como:

```text
Function 'async_aggregate_telemetry' is never used
Variable 'primary_sensor' is never used
Variable 'sample_array' is never used
Variable 'system_registry' is never used
Variable 'telemetry_tuple' is never used
```

aunque algunas variables y funciones sí participan en operaciones posteriores.

Auditar el análisis de usos.

El linter debe distinguir correctamente entre:

```text
declaration
use
move
borrow
read
write
consume
```

Especialmente teniendo en cuenta el sistema de ownership/borrowing.

No eliminar warnings simplemente desactivando el lint.

---

# 5. MEMORY DOMAINS

Kryo posee el concepto de:

```text
Stack
Heap
GPU
Shared
Distributed
Transfer
Persistent
```

Actualmente el showcase puede ejecutar construcciones como:

```kryo
let stack_matrix = Stack::alloc(1000);
let heap_data = Heap::alloc(2500);
let gpu_buffer = GPU::alloc(8192);
let shared_state = Shared::alloc(512);
let cluster_node = Distributed::alloc(9999);

let recovered_to_stack = Domain::transfer(heap_data);
```

Debes determinar cuáles de estas abstracciones tienen implementación real y cuáles son actualmente una representación/runtime abstraction.

Para MARCUS OS necesitamos convertir al menos:

```text
Stack
Heap
Shared
Persistent
GPU
```

en conceptos con semántica real.

Especialmente:

```text
Stack<T>
Heap<T>
Shared<T>
Persistent<T>
GPU<T>
```

deben poder tener representación adecuada a nivel de sistema operativo.

`Distributed<T>` puede permanecer inicialmente como una abstracción de networking/userspace.

---

# 6. BARE-METAL TARGET

Crear soporte para un target dedicado a MARCUS OS.

No utilizar:

```text
Windows CRT
Windows API
MSVC runtime
Windows process model
Windows filesystem
```

como dependencias del kernel.

El kernel debe poder compilarse para un target bare-metal x86_64.

Conceptualmente:

```text
Kryo
 ↓
LLVM
 ↓
x86_64 bare metal object
 ↓
linker script
 ↓
kernel binary
 ↓
bootloader
 ↓
QEMU
```

Determina qué target triple de LLVM es más apropiado y documenta la decisión.

---

# 7. FREESTANDING KRYO

Crear un modo freestanding para Kryo.

Por ejemplo:

```text
kryo build --target=x86_64-marcus-os
```

El kernel no debe depender de:

```text
malloc
printf
Windows APIs
libc
Python
C++ runtime
```

Implementar o proporcionar equivalentes mínimos del runtime cuando sea necesario.

---

# 8. KERNEL

El kernel debe ser principalmente Kryo.

Arquitectura inicial:

```text
MARCUS OS KERNEL
│
├── arch/
│   └── x86_64/
│
├── boot/
├── cpu/
├── interrupts/
├── memory/
├── scheduler/
├── process/
├── ipc/
├── filesystem/
├── networking/
├── drivers/
├── security/
└── syscall/
```

---

# 9. BOOT

Primera meta:

```text
BIOS/UEFI
   ↓
Bootloader
   ↓
Kryo Kernel
   ↓
x86_64 initialization
   ↓
Serial output
   ↓
Framebuffer
```

El kernel debe poder arrancar en QEMU.

La primera salida debe demostrar que el kernel realmente está ejecutándose.

Ejemplo:

```text
========================================
          MARCUS OS KERNEL
========================================

Architecture: x86_64
Language: Kryo
Boot: OK
CPU: OK
Memory: OK
Interrupts: OK

MARCUS OS kernel initialized.
```

No aceptar una simulación de esta salida desde Windows.

Debe provenir del kernel arrancado en QEMU.

---

# 10. MEMORY MANAGEMENT

Implementar:

```text
Physical Memory Manager
        ↓
Virtual Memory Manager
        ↓
Page Allocator
        ↓
Kernel Heap
```

Inicialmente:

* page frames;
* page tables;
* identity mapping;
* higher-half kernel si corresponde;
* kernel heap;
* allocation/deallocation;
* memory protection.

Integrar el sistema de Memory Domains de Kryo progresivamente.

---

# 11. KRYO OWNERSHIP EN EL KERNEL

Aprovechar el borrow checker de Kryo.

El objetivo es que estructuras críticas del kernel puedan beneficiarse de:

```text
Ownership
Borrowing
Lifetime analysis
Resource ownership
```

Ejemplos de recursos:

```text
Page
PageTable
Process
Thread
File
Socket
Device
Buffer
IPC Channel
```

El compilador debe impedir estados peligrosos cuando sea posible.

---

# 12. CAPACITY-AWARE TYPES

Utilizar:

```text
Array<T, N>
BoundedVec<T, N>
Bounded<T, MIN, MAX>
```

en estructuras del kernel donde los límites sean conocidos.

Especialmente:

```text
Network buffers
IPC messages
Device buffers
Page metadata
Kernel queues
Scheduler structures
```

El objetivo es que determinados límites sean comprobables durante compilación.

---

# 13. COMPILE-TIME CONTRACTS

Kryo soporta:

```kryo
fn safe_divide(numerator: i32, denominator: i32) -> i32
    requires denominator != 0
{
    return numerator / denominator;
}
```

Extender esta capacidad para expresar invariantes del kernel.

Ejemplos conceptuales:

```text
page address alignment
buffer capacity
ring buffer bounds
scheduler invariants
IPC message sizes
device constraints
```

Separar claramente:

```text
compile-time contracts
runtime assertions
debug assertions
```

No afirmar que algo fue comprobado en compile-time si realmente sólo fue comprobado durante ejecución.

---

# 14. INTERRUPTS

Implementar:

```text
GDT
IDT
Exceptions
IRQ
PIC/APIC
Timer
Keyboard
```

La mayor parte debe ser Kryo.

Assembly solamente donde sea estrictamente necesario para:

* interrupt entry;
* context switching;
* CPU-specific instructions;
* bootstrapping.

---

# 15. PROCESSES Y THREADS

Implementar progresivamente:

```text
Process
Thread
Address Space
Context
Scheduler
```

El scheduler inicialmente puede ser simple.

Priorizar:

```text
correctness > complexity
```

---

# 16. IPC

Implementar:

```text
Channels
Message Passing
Shared Memory
Handles
```

Marcus utilizará IPC.

El kernel no debe conocer detalles internos del modelo de IA.

---

# 17. SYSTEM CALL ABI

Crear una syscall ABI estable.

Categorías:

```text
process.*
thread.*
memory.*
filesystem.*
network.*
ipc.*
device.*
graphics.*
system.*
```

Marcus debe interactuar con el sistema mediante estas interfaces.

No permitir que Python acceda directamente a memoria del kernel.

---

# 18. C++ INTEROPERABILITY

C++ es secundario.

Usarlo solamente para:

* drivers;
* hardware-specific components;
* existing libraries;
* compatibility;
* bindings;
* componentes que todavía no puedan implementarse razonablemente en Kryo.

Arquitectura:

```text
Kryo Kernel
     │
     ├── Kryo native components
     │
     └── C ABI
           │
          C++
           │
        Hardware
```

Mantener el boundary pequeño.

---

# 19. PYTHON

Python pertenece exclusivamente a userspace.

Nunca introducir Python en:

```text
kernel
interrupt handlers
memory manager
scheduler
bootloader
```

Python será utilizado principalmente para Marcus.

Arquitectura:

```text
MARCUS OS
│
├── Kernel — Kryo
│
├── System services — Kryo/C++
│
└── Userspace
     │
     └── Python Runtime
            │
            └── Marcus
```

---

# 20. MARCUS

Marcus es el asistente inteligente de MARCUS OS.

Arquitectura:

```text
Marcus
│
├── Core
├── LLM
├── Planner
├── Memory
├── Context
├── Tools
├── Permissions
├── Voice
└── OS Interface
```

Marcus debe comunicarse con MARCUS OS mediante:

```text
Syscalls
IPC
OS APIs
```

Nunca mediante acceso directo al kernel.

---

# 21. MARCUS TOOLS

Marcus debe poder utilizar herramientas como:

```text
filesystem.read
filesystem.write
filesystem.search

process.list
process.launch

system.info
system.monitor

network.request

terminal.execute

developer.compile
developer.test
developer.analyze
```

Cada herramienta debe tener permisos independientes.

---

# 22. MARCUS SECURITY

No otorgar privilegios ilimitados al agente.

Crear un permission system:

```text
Marcus
 │
 ├── filesystem.read
 ├── filesystem.write
 ├── process.list
 ├── process.launch
 ├── network.access
 ├── device.access
 └── system.modify
```

Las acciones sensibles requieren autorización.

---

# 23. DEVELOPMENT MILESTONES

## M0 — Compiler Audit

Antes de tocar el kernel:

* auditar Kryo;
* corregir bugs conocidos;
* verificar HIR;
* verificar MIR;
* verificar LLVM;
* verificar ownership;
* verificar contracts;
* verificar Memory Domains;
* verificar async;
* verificar stdlib.

---

## M1 — Freestanding Target

Implementar:

```text
x86_64-marcus-os
```

Debe poder generar código sin depender de Windows.

Resultado:

```text
.kyo
 ↓
Kryo
 ↓
LLVM
 ↓
ELF/object
```

---

## M2 — Bootable Kernel

Implementar:

* bootloader;
* kernel entry;
* serial;
* framebuffer;
* GDT;
* IDT.

Resultado:

```text
QEMU
 ↓
MARCUS OS
 ↓
Kryo Kernel
```

---

## M3 — Memory

Implementar:

* physical allocator;
* paging;
* virtual memory;
* kernel heap;
* memory protection.

---

## M4 — Kernel Core

Implementar:

* interrupts;
* timer;
* processes;
* threads;
* scheduler;
* IPC.

---

## M5 — Storage

Implementar:

```text
Block Device
 ↓
VFS
 ↓
Filesystem
```

---

## M6 — Networking

Implementar progresivamente:

```text
Ethernet
ARP
IPv4
UDP
TCP
Sockets
```

---

## M7 — Userspace

Implementar:

```text
ELF loader
Process launcher
Syscalls
Userspace runtime
Init process
```

---

## M8 — Python

Crear un entorno Python userspace capaz de comunicarse con las APIs del OS.

---

## M9 — Marcus

Implementar:

```text
LLM
Memory
Planner
Tools
Permissions
OS integration
```

---

## M10 — Desktop

Implementar:

```text
Framebuffer
Compositor
Window Manager
Input
Desktop
Marcus GUI
```

---

# 24. REPOSITORY

Utilizar:

```text
marcus-os/
│
├── kernel/
│   ├── src/
│   │   ├── arch/
│   │   │   └── x86_64/
│   │   ├── boot/
│   │   ├── cpu/
│   │   ├── interrupts/
│   │   ├── memory/
│   │   ├── scheduler/
│   │   ├── process/
│   │   ├── ipc/
│   │   ├── filesystem/
│   │   ├── networking/
│   │   ├── graphics/
│   │   ├── security/
│   │   └── drivers/
│   │
│   └── kryo.toml
│
├── userspace/
│   ├── init/
│   ├── services/
│   └── applications/
│
├── marcus/
│   ├── core/
│   ├── llm/
│   ├── memory/
│   ├── planner/
│   ├── tools/
│   ├── permissions/
│   ├── voice/
│   └── main.py
│
├── cpp/
│   ├── drivers/
│   ├── bindings/
│   └── compatibility/
│
├── boot/
├── sdk/
├── tests/
├── scripts/
└── docs/
```

---

# 25. TESTING REQUIREMENTS

Cada milestone debe tener tests.

Especialmente:

```text
Compiler tests
Parser tests
Type checker tests
Borrow checker tests
Contract tests
Memory domain tests
LLVM codegen tests
ABI tests
Kernel tests
QEMU boot tests
Userspace tests
Syscall tests
```

Para cada componente documentar:

```text
Implemented
Partially implemented
Stub
Not implemented
```

Nunca marcar como `Implemented` algo que sólo imprime un resultado ficticio.

---

# 26. IMPORTANT DEVELOPMENT RULE

No intentes implementar MARCUS inmediatamente.

Primero consigue:

```text
Kryo
 ↓
Freestanding target
 ↓
LLVM
 ↓
Bootloader
 ↓
Kernel
 ↓
QEMU
```

Sólo cuando esto funcione:

```text
MARCUS OS kernel booted successfully
```

comenzar con userspace.

Después:

```text
Kernel
 ↓
Syscalls
 ↓
Userspace
 ↓
Python
 ↓
Marcus
```

---

# 27. FIRST ACTION

Tu primera acción debe ser inspeccionar el proyecto Kryo existente.

No empieces escribiendo un kernel gigante.

Primero genera un informe técnico:

```text
================================================
KRYO BARE-METAL AUDIT
================================================

Compiler architecture:
...

Current target:
...

LLVM version:
...

HIR:
...

MIR:
...

Borrow checker:
...

Memory model:
...

Runtime:
...

Standard library:
...

ABI:
...

Current limitations:
...

Bugs:
...

Required compiler changes:
...

Required kernel infrastructure:
...

Recommended implementation order:
...
```

Después corrige primero los problemas reales detectados.

Luego implementa **M0**.

Después implementa **M1**.

El primer objetivo verificable debe ser:

```text
Kryo source
     ↓
Kryo compiler
     ↓
bare-metal object
     ↓
linker
     ↓
bootable kernel
     ↓
QEMU
     ↓
MARCUS OS KERNEL INITIALIZED
```

No avances a networking, GUI, Python o Marcus hasta que la cadena anterior funcione realmente.