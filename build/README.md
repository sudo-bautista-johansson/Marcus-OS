# Marcus OS bootstrap

Este directorio contiene el primer build reproducible del sistema. El resultado es un kernel freestanding x86_64 para BIOS/QEMU, con `kernel_main.kyo` como entrypoint del kernel. C queda limitado a la primitiva FFI `kernel_puts` para escribir en VGA y COM1; el arranque de CPU sigue usando ensamblador.

## Estado

- Bootloader BIOS: implementado.
- Kernel x86_64 freestanding: implementado.
- Entry point del kernel en Kryo: implementado.
- Salida COM1 y VGA: implementada.
- Paging identity-map de los primeros 2 MiB: implementado.
- Build Kryo -> LLVM IR -> objeto `x86_64-pc-none-elf`: implementado mediante `kryo build --target=x86_64-marcus-os`.
- QEMU boot test: pasa; imprime el mensaje de inicialización por COM1.

## Limitaciones conocidas

Kryo ahora acepta `kryo build --target=x86_64-marcus-os`. Ese target genera un objeto ELF freestanding sin CRT ni enlace Windows; el script de Marcus OS enlaza después ese objeto con el kernel C/FFI.

El backend LLVM ya corrige la representación FFI de `String`: los literales y los parámetros externos usan `i8*`, sin el puntero adicional `i8**`. El kernel usa ahora `String` directamente en la declaración Kryo de `kernel_puts`.

Construir desde PowerShell con `./build/build.ps1`. La imagen generada queda en `build/out/marcus-os.img` y se puede probar con:

```powershell
qemu-system-x86_64 -drive format=raw,file=build\out\marcus-os.img -display none -serial stdio -no-reboot -no-shutdown
```