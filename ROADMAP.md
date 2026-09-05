MARCUS OS — Development Roadmap

Goal: Build a real, bootable x86_64 operating system whose kernel is written predominantly in Kryo, with Marcus as a userspace AI assistant.

Core rule: Do not mark a component as implemented unless its behavior is real and verified. A simulated output, parser-only feature, or fake runtime abstraction does not count.

🧭 Project Vision

                         MARCUS OS
                             │
        ┌────────────────────┼────────────────────┐
        │                    │                    │
     Kernel               Userspace            Marcus
      Kryo                Kryo/C++             Python
        │                    │                    │
        └──────────── Syscalls / IPC ────────────┘
                             │
                          Hardware
                           x86_64

The development order is intentionally bottom-up:

Kryo Compiler
     ↓
Freestanding Target
     ↓
LLVM / Code Generation
     ↓
Bootloader
     ↓
Kryo Kernel
     ↓
QEMU
     ↓
Memory / Interrupts / Scheduler / IPC
     ↓
Storage / Networking
     ↓
Userspace
     ↓
Python Runtime
     ↓
Marcus
     ↓
Desktop / GUI

Phase 0 — Kryo Foundation

M0 — Compiler Audit

Objective: Know exactly what Kryo really supports before modifying it for operating-system development.

Compiler

Inspect lexer

Inspect parser

Inspect name resolution

Inspect type checker

Inspect borrow checker

Inspect constant propagation

Inspect dead-code elimination

Inspect HIR

Inspect MIR

Inspect LLVM IR generation

Inspect LLVM integration

Inspect linker integration

Document current target triple

Document current ABI

Feature Verification

For every major feature:

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
Runtime Behavior

Functions

Recursion

Control flow

Structs

Tuples

Arrays

Slices

Option

Result

HashMap

Math

async/await

Futures

Ownership

Borrowing

Bounded types

BoundedVec

requires

Memory Domains

Known Bugs

HashMap

Reproduce the incorrect len() behavior

Trace insertion

Trace storage

Trace lookup

Trace length calculation

Identify whether the bug is parser, type system, lowering, runtime, or data-structure related

Fix the actual implementation

Add regression tests

Expected behavior:

insert(...)
insert(...)
insert(...)

len() == 3

Linter

Audit use analysis

Track declarations

Track reads

Track writes

Track moves

Track borrows

Track consumes

Account for ownership semantics

Account for indirect usage

Add regression tests

Do not disable the warning system as a workaround

Memory Domains

Determine the real implementation status of:

Stack

Heap

Shared

Persistent

GPU

Transfer

Distributed

Classify each as:

Implemented
Partially implemented
Runtime abstraction
Stub
Not implemented

Contracts

Verify requires

Determine compile-time vs runtime checking

Verify generated behavior

Add tests for valid contracts

Add tests for invalid contracts

Document limitations

M0 Deliverable

Create:

docs/kryo-bare-metal-audit.md

Containing:

Compiler architecture

Current target

LLVM version

HIR status

MIR status

Borrow checker status

Memory model

Runtime dependencies

Standard library

ABI

Bugs

Limitations

Required compiler changes

Required kernel infrastructure

Recommended implementation order

Milestone complete when the compiler's real capabilities are documented and known bugs have a concrete implementation plan.

Phase 1 — Bare Metal

M1 — Freestanding Target

Objective: Make Kryo capable of generating code that does not depend on the Windows userspace environment.

Target:

x86_64-marcus-os

Remove Kernel Dependencies

Windows CRT dependency

Windows API dependency

MSVC runtime dependency

Windows process model

Windows filesystem

malloc

printf

Python

C++ runtime

Compiler

Add target definition

Add freestanding compilation mode

Define ABI

Define calling convention

Define object format

Define relocation requirements

Define panic/abort behavior

Define runtime initialization

Define minimal intrinsic support

Output

.kyo
 ↓
Kryo
 ↓
LLVM
 ↓
x86_64 bare-metal object
 ↓
linker script
 ↓
kernel binary

Verification

Compile a minimal Kryo function

Verify generated object

Verify no Windows CRT imports

Verify no forbidden runtime dependencies

Verify ABI correctness

Add compiler regression test

Milestone complete when Kryo can produce a valid freestanding x86_64 object.

Phase 2 — Boot

M2 — Bootable Kernel

Objective: Boot actual Kryo code in QEMU.

Boot

Select boot strategy

Create bootloader integration

Define kernel image format

Define kernel entry point

Pass boot information to the kernel

Initialize CPU

Initialize stack

Initialize basic serial output

Initialize framebuffer when available

CPU

GDT

Basic CPU initialization

Exception foundations

IDT foundation

Verification

Boot must happen in QEMU:

QEMU
 ↓
Bootloader
 ↓
Kryo Kernel
 ↓
x86_64 initialization
 ↓
Serial
 ↓
Framebuffer

Expected proof:

========================================
          MARCUS OS KERNEL
========================================

Architecture: x86_64
Language: Kryo
Boot: OK
CPU: OK

MARCUS OS kernel initialized.

The message must originate from the booted kernel, not from a Windows program pretending to be an OS.

Milestone complete when a real kernel compiled from Kryo boots successfully in QEMU.

Phase 3 — Memory

M3 — Memory Management

Objective: Build the foundation required for safe kernel memory management.

Physical Memory

Parse available physical memory

Page frame representation

Physical page allocator

Allocation/deallocation

Reserved regions

Kernel memory regions

Virtual Memory

Page tables

Page mapping

Page unmapping

Identity mapping

Address-space abstraction

Memory protection

Higher-half kernel if appropriate

Kernel Heap

Kernel heap region

Kernel allocator

Allocation API

Deallocation API

Out-of-memory behavior

Allocation tests

Kryo Memory Domains

Progressively give real system semantics to:

Stack<T>
Heap<T>
Shared<T>
Persistent<T>
GPU<T>

Keep:

Distributed<T>

as an initial networking/userspace abstraction.

Milestone complete when the kernel has functional physical memory, virtual memory, and heap allocation.

Phase 4 — Kernel Core

M4 — Interrupts, Processes, Threads and IPC

Interrupts

IDT

CPU exceptions

IRQ infrastructure

PIC

APIC

Timer

Keyboard

Interrupt dispatch

Interrupt-safe kernel paths

Assembly should only be used where necessary:

Interrupt entry

Context switching

CPU-specific instructions

Bootstrapping

Processes

Process structure

Address space

Process IDs

Process lifecycle

Process creation

Process termination

Threads

Thread structure

CPU context

Kernel stack

Context switching

Thread lifecycle

Scheduler

Start simple.

Priority:

Correctness > Complexity

Basic scheduler

Timer-driven scheduling

Runnable queue

Sleep/wake mechanism

Scheduler tests

IPC

Channels

Message passing

Shared memory

Handles

IPC ownership rules

Ownership

Use Kryo's ownership model for:

Page
PageTable
Process
Thread
File
Socket
Device
Buffer
IPC Channel

Ownership rules

Borrowing rules

Lifetime analysis

Resource ownership

Kernel-specific compiler tests

Milestone complete when multiple kernel execution contexts can be scheduled and communicate safely.

Phase 5 — Storage

M5 — Storage and Filesystem

Objective: Give the OS persistent storage.

Block Device
     ↓
Storage Driver
     ↓
VFS
     ↓
Filesystem
     ↓
Files / Directories

Block Layer

Block device abstraction

Read

Write

Block caching

Device errors

VFS

Files

Directories

Paths

File descriptors / handles

Permissions

Mount abstraction

Filesystem

Select initial filesystem

Implement read

Implement write

Implement create

Implement delete

Implement directory traversal

Filesystem tests

Milestone complete when MARCUS OS can boot and interact with a real filesystem.

Phase 6 — Networking

M6 — Networking Stack

Implement incrementally:

Ethernet
   ↓
ARP
   ↓
IPv4
   ↓
UDP
   ↓
TCP
   ↓
Sockets

Network Core

Network interface abstraction

Ethernet frames

ARP

IPv4

UDP

TCP

Socket API

Network buffers

Capacity-aware packet structures

Kryo Integration

Use capacity-aware types where useful:

Array<T, N>
BoundedVec<T, N>
Bounded<T, MIN, MAX>

Especially for:

Network buffers

Packet metadata

Queues

Protocol fields

Milestone complete when userspace can perform basic network communication through OS APIs.

Phase 7 — Userspace

M7 — Userspace Runtime

Objective: Establish the boundary between the kernel and applications.

ELF

ELF loader

Segment loading

Relocations as required

Entry point

User address space

Stack initialization

Process Launcher

Launch userspace process

Pass arguments

Pass environment

Process isolation

Process exit handling

Syscall ABI

Create a stable syscall ABI.

process.*
thread.*
memory.*
filesystem.*
network.*
ipc.*
device.*
graphics.*
system.*

Define syscall numbers

Define argument ABI

Define return values

Define error model

Define handle model

Add ABI tests

Milestone complete when an independent userspace program can execute through the syscall interface.

Phase 8 — Python

M8 — Python Userspace

Python is userspace only.

Never place Python inside:

Kernel
Interrupt handlers
Memory manager
Scheduler
Bootloader

Runtime

Select Python runtime strategy

Port/build runtime for MARCUS OS userspace

Implement required OS bindings

Filesystem bindings

Process bindings

IPC bindings

Network bindings

System information bindings

Permission-aware APIs

Verification

Python process starts

Python can make syscalls

Python can use OS APIs

Python cannot directly access kernel memory

Milestone complete when Python runs as a normal isolated userspace environment.

Phase 9 — Marcus

M9 — Marcus AI Assistant

Objective: Build Marcus as an OS-integrated userspace AI agent.

Architecture:

Marcus
 ├── Core
 ├── LLM
 ├── Planner
 ├── Memory
 ├── Context
 ├── Tools
 ├── Permissions
 ├── Voice
 └── OS Interface

Core

Main agent loop

Context management

Conversation state

Error handling

LLM

Local model interface

Model loading/inference layer

Streaming responses

Context limits

Model configuration

Memory

Short-term memory

Persistent memory

Search/retrieval

Memory permissions

Planner

Task decomposition

Tool selection

Execution loop

Failure recovery

Confirmation flow

Tools

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

Implement filesystem tools

Implement process tools

Implement system tools

Implement network tool

Implement terminal tool

Implement developer tools

OS Interface

Marcus communicates through:

Syscalls
   +
IPC
   +
OS APIs

Never:

Marcus → direct kernel memory access

Milestone complete when Marcus can perform useful OS tasks through controlled interfaces.

Phase 10 — Security

M9.5 — Marcus Permission System

This phase can be developed alongside M9.

Permissions

filesystem.read
filesystem.write
process.list
process.launch
network.access
device.access
system.modify

Permission registry

Per-tool permissions

Permission checks

Sensitive-action confirmation

Audit logging

Permission inheritance rules

Deny-by-default behavior

Security Boundary

Marcus
   ↓
Permission Layer
   ↓
OS API / Syscall
   ↓
Kernel

The agent must never receive unrestricted kernel privileges merely because it is the system assistant.

Phase 11 — Desktop

M10 — Graphical Desktop

Objective: Turn MARCUS OS into a usable graphical operating system.

Graphics

Framebuffer

Graphics abstraction

Rendering primitives

Surface/buffer management

GPU domain integration where applicable

Compositor

Surface composition

Window buffers

Damage tracking

Frame presentation

Window Manager

Window creation

Window destruction

Focus

Z-order

Input routing

Input

Keyboard

Mouse

Input events

Device abstraction

Desktop

Desktop shell

Application launcher

Taskbar/dock

System information

Notifications

Terminal

File manager

Settings

Marcus GUI

Marcus panel

Chat interface

Tool activity

Permission prompts

System monitoring

Voice interface

Milestone complete when MARCUS OS boots into a functional graphical desktop.

Phase 12 — C++ Interoperability

C++ remains secondary.

Use it only where appropriate:

Hardware-specific components

Drivers

Existing libraries

Compatibility layers

Bindings

Components not yet practical to implement in Kryo

Boundary:

Kryo
  ↓
C ABI
  ↓
C++
  ↓
Hardware / Existing Library

Rules:

Keep the C ABI small

Avoid C++ runtime dependencies in the kernel

Document every C++ component

Prefer Kryo when equivalent functionality is practical

Cross-Project Verification

Every milestone must have tests.

Compiler

Parser tests

Type checker tests

Borrow checker tests

Contract tests

Memory Domain tests

LLVM codegen tests

ABI tests

Kernel

Boot tests

CPU tests

Memory tests

Paging tests

Allocator tests

Interrupt tests

Scheduler tests

IPC tests

Filesystem tests

Networking tests

Syscall tests

Userspace

ELF loader tests

Process tests

Runtime tests

Python integration tests

Marcus tool tests

Permission tests

QEMU

Automated boot test

Kernel output verification

Memory initialization verification

Interrupt verification

Userspace launch verification

Syscall verification

Implementation Status Convention

Every subsystem must use exactly one of:

Status

Meaning

Implemented

Real functionality exists and has been tested

Partially implemented

Some required functionality exists

Stub

Interface exists but functionality is missing

Runtime abstraction

Behavior is represented/simulated by runtime code

Not implemented

No meaningful implementation exists

Never use Implemented for a fake result, hardcoded output, or parser-only feature.

Repository Structure

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

Recommended Development Order

The critical path is:

                    ┌─────────────────┐
                    │  Kryo Audit M0  │
                    └────────┬────────┘
                             ↓
                    ┌─────────────────┐
                    │ Freestanding M1 │
                    └────────┬────────┘
                             ↓
                    ┌─────────────────┐
                    │ Bootable M2     │
                    └────────┬────────┘
                             ↓
                    ┌─────────────────┐
                    │ Memory M3       │
                    └────────┬────────┘
                             ↓
                    ┌─────────────────┐
                    │ Kernel Core M4  │
                    └────────┬────────┘
                             ↓
                    ┌─────────────────┐
                    │ Storage M5      │
                    └────────┬────────┘
                             ↓
                    ┌─────────────────┐
                    │ Networking M6   │
                    └────────┬────────┘
                             ↓
                    ┌─────────────────┐
                    │ Userspace M7    │
                    └────────┬────────┘
                             ↓
                    ┌─────────────────┐
                    │ Python M8       │
                    └────────┬────────┘
                             ↓
                    ┌─────────────────┐
                    │ Marcus M9       │
                    └────────┬────────┘
                             ↓
                    ┌─────────────────┐
                    │ Desktop M10     │
                    └─────────────────┘

Do not skip the critical path.

In particular:

❌ Marcus before kernel
❌ Python before userspace
❌ GUI before framebuffer/compositor
❌ Networking before kernel networking infrastructure
❌ "Boot successful" without QEMU verification
❌ "Implemented" based only on syntax or simulated output

🏁 First Verifiable Objective

The first major objective is deliberately small:

Kryo source
     ↓
Kryo compiler
     ↓
Bare-metal object
     ↓
Linker
     ↓
Bootable kernel
     ↓
QEMU
     ↓
MARCUS OS KERNEL INITIALIZED

Once this chain works for real, the project has crossed the most important boundary:

Kryo is no longer only a general-purpose language prototype — it is capable of powering a real operating-system kernel.

Only after this point should development expand aggressively into userspace, networking, Python, Marcus, and the graphical desktop.

Final Definition of Done

MARCUS OS reaches its long-term goal when all of the following are true:

Kryo can compile freestanding x86_64 code

Kryo can produce the kernel binary

MARCUS OS boots on real x86_64 hardware or QEMU

Kernel is predominantly Kryo

Memory management is functional

Interrupts are functional

Processes and threads are functional

Scheduler is functional

IPC is functional

Storage is functional

Networking is functional

Userspace is isolated

Syscall ABI is stable

Python runs in userspace

Marcus runs as a userspace agent

Marcus uses controlled OS APIs

Permission system protects sensitive operations

Desktop is functional

QEMU integration is automated

Major subsystems have regression tests

Current implementation status — 2026-09-05

M0 — Compiler Audit: Partially implemented. String FFI, mutable HashMap length/replacement, async Future wrapping, conservative BoundedVec contract ranges, 128-bit contract conservatism and Domain::copy semantics are fixed and regression-tested. Full linter use-state analysis, HashMap lowering in LLVM, remaining memory-domain semantics and complete stdlib audit remain open.

M1 — Freestanding Target: Implemented. `kryo build --target=x86_64-marcus-os` produces an ELF object without Windows CRT linking.

M2 — Bootable Kernel: Partially implemented. BIOS boot, Kryo entrypoint, long mode, GDT, serial, IDT, controlled halt and a real VGA mode 13h framebuffer renderer are verified in QEMU. Higher-resolution framebuffer, keyboard event routing and compositor remain open.

M3 — Memory: Partially implemented. BIOS E820 discovery, usable-region parsing and a bounded 4 KiB physical frame allocator are verified by reserving two distinct frames and freeing them from Kryo. A bounded 512-page virtual window maps real frames, a page-backed kernel heap can allocate/free blocks through Kryo, NX/read-only PTE flags are verified, and a bounded page-table pool can clone/switch/restore a kernel address space through CR3. The allocator is bounded to 128 MiB and the table pool to 16 tables; general process address spaces, dynamic table reclamation and recoverable protection-fault dispatch remain open.

M4 — Kernel Core: Partially implemented. PIC remapping, PIT at 100 Hz, central IRQ dispatch, IRQ0 timer delivery, IRQ1 keyboard delivery, a bounded round-robin queue, a real callee-saved-register context switch, process records with cloned address spaces and a bounded IPC channel are verified in QEMU. Preemptive context switching, process lifecycle, blocking IPC and ownership-safe handles remain open.

M5 — Storage: Partially implemented. The image contains a persistent MarcusFS superblock, root directory and file sectors; ATA PIO and a one-sector block cache provide real disk read/write, while the kernel verifies VFS lookup, permissions, read, create, write, delete and a 600-byte multi-sector file in QEMU. Nested directories, general paths, cache eviction, richer permissions and full filesystem error handling remain open.

M6 — Networking: Partially implemented. The kernel now has MTU-bounded network buffers, verified Ethernet/IPv4/UDP serialization, PCI e1000 discovery, 32/64-bit BAR handling, a dedicated 16-page MMIO mapping window, e1000 CTRL/STATUS initialization and bounded TX/RX descriptor ring configuration. TCTL/COLD is corrected, TDH/TDT/TXDCTL/TCTL/ICR are captured around the doorbell, and DMA rings/buffers are placed in a dedicated aligned `.dma` section. The TX descriptor still needs QEMU DD validation; packet TX/RX interrupts, ARP wire traffic and external networking remain open.

M7 — Userspace: Not implemented. There is no `userspace/` tree, ELF loader, syscall ABI, process launcher or init process. Kernel process records exist only as an in-kernel test.

M8 — Python: Not implemented. Python must stay in userspace and therefore cannot start until M7 exists.

M9 — Marcus: Not implemented. There is no `marcus/` tree, LLM loop, tools, planner or OS interface. Marcus must talk only through syscalls/IPC after M7/M8.

M9.5 — Permissions: Not implemented. Deny-by-default permission layer for Marcus tools.

M10 — Desktop: Not implemented. The target appearance is `img/MARCUS.png` and the contract is `docs/desktop-visual-spec.md`. VGA mode 13h draws test rectangles; that is not the desktop. Compositor, window manager, 1920x1080 framebuffer, input routing and Marcus GUI remain open.

Next increment on the critical path: finish M6 TX/RX + ARP on the wire, then add M7 (syscall ABI + ELF loader + init). Do not mark M8–M10 as started until a userspace program executes through syscalls.

Implementation status is honestly documented

The End State

┌───────────────────────────────────────────────┐
│                 MARCUS OS                     │
├───────────────────────────────────────────────┤
│                  Desktop                      │
├───────────────────────────────────────────────┤
│              Applications                     │
├───────────────────────────────────────────────┤
│          Python Runtime / Marcus              │
├───────────────────────────────────────────────┤
│        Userspace Services / APIs              │
├───────────────────────────────────────────────┤
│              Syscall / IPC ABI                │
├───────────────────────────────────────────────┤
│              KRYO KERNEL                      │
│                                               │
│  Memory • Scheduler • Processes • IPC         │
│  Filesystem • Networking • Drivers            │
│  Interrupts • Security • Graphics             │
├───────────────────────────────────────────────┤
│             x86_64 Hardware                   │
└───────────────────────────────────────────────┘

MARCUS OS is not a mockup. It is a real systems project.

The roadmap therefore follows one principle:

Prove every layer before building the next one.