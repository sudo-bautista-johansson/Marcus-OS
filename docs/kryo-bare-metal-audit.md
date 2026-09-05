# Kryo Bare-Metal Audit

Auditoría basada en el compilador instalado desde `C:\Users\User\Desktop\Kryo` y en el build real de Marcus OS.

## Verificado

- Lexer, parser, resolución de nombres, type checker, borrow checker, constant propagation, dead-code elimination, HIR, MIR y LLVM codegen pasan para `kernel/src/kernel_main.kyo`.
- `kryo build --target=x86_64-marcus-os` produce un objeto ELF freestanding.
- El target usa `x86_64-pc-none-elf` y no enlaza Windows CRT, MSVC, `printf` ni filesystem de Windows.
- El ABI FFI de `String` genera `i8*`; existe regresión en `tests/llvm_backend_test.rs`.
- El kernel se prueba en QEMU, no como proceso Windows.

## ABI actual del kernel

- `String` FFI: `i8*`.
- `usize`: `i64` en LLVM x86_64.
- Estructuras de hardware: C ABI packed cuando corresponde.
- Externs de CPU, puertos, ATA, interrupciones, memoria y context switch están aislados en C/ASM.
- La coordinación del kernel y sus pruebas de secuencia se escriben en Kryo.

## Clasificación de features

| Feature | Estado | Evidencia o limitación |
| --- | --- | --- |
| Funciones/control/structs/arrays | Implementado para el slice kernel | Compila a LLVM y arranca en QEMU |
| Ownership/borrowing | Parcial | Se ejecutan las fases; falta cobertura específica de recursos kernel |
| Contracts | Parcial | Metadata/codegen existe; falta demostrar enforcement bare-metal |
| Memory Domains | Runtime abstraction | No tienen todavía semántica física completa en kernel |
| `HashMap` | Parcial/bug conocido | Requiere reproducir y corregir `len()` con regresión |
| Linter de usos | Parcial | No distingue todavía todos los estados de ownership indirecto |
| async/Futures | Parcial | No forman parte del runtime freestanding |
| stdlib | Parcial | `println` y helpers de userspace no son válidos en kernel |
| LLVM backend | Implementado para el slice usado | Sigue siendo textual y tiene constructs no soportados |
| Native backend sin LLVM | No implementado | El prompt de backend nativo es diseño, no implementación |

## Dependencias prohibidas del kernel

El build freestanding no usa `malloc`, `printf`, Python, Windows API, MSVC CRT ni C++ runtime. C sólo aparece en boundaries de hardware y estructuras ABI; ASM sólo en boot, context switch e IRQ entry.

## Riesgos abiertos

1. El target todavía es un adaptador de CLI sobre LLVM, no un backend nativo independiente.
2. El análisis E820 y el allocator tienen límites explícitos de memoria.
3. La auditoría completa de `HashMap`, linter, contracts, domains, async y stdlib debe hacerse contra el repositorio del compilador Kryo.
4. El kernel no debe marcarse como desktop hasta tener framebuffer, compositor, input y userspace.

## Comandos de evidencia

```powershell
kryo check kernel\src\kernel_main.kyo
./build/build.ps1
qemu-system-x86_64 -drive format=raw,file=build\out\marcus-os.img -display none -serial stdio -no-reboot -no-shutdown
```