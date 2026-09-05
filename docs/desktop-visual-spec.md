# MARCUS OS Desktop Visual Specification

La referencia visual es `img/MARCUS.png`. Esta especificación describe el destino de M10; no representa una interfaz ya arrancable. El desktop sólo se marcará como implementado cuando exista framebuffer, compositor, input y una prueba QEMU que pinte los píxeles desde el sistema.

## Dirección visual

- Tema oscuro de ciencia ficción sobria, con fondo casi negro azulino.
- Azul eléctrico como color de acción y foco; texto blanco frío y secundarios gris azulado.
- Bordes finos translúcidos, paneles con blur y brillo azul muy contenido.
- Tipografía geométrica legible, con números monoespaciados en terminal y métricas.
- El logo M de Marcus debe ser una señal persistente, no un adorno aislado.

## Composición

- Barra superior de 48 px: logo/nombre a la izquierda, reloj centrado y estado del sistema a la derecha.
- Rail lateral de aproximadamente 108 px: Home, Terminal, Marcus, Files, Code, System, Network y Settings.
- Centro: wallpaper de montaña, lago y torre tecnológica; logo M y título MARCUS OS centrados.
- Panel Marcus a la izquierda: estado online, visualizador de voz y conversación.
- Terminal debajo del panel Marcus: shell real del userspace, no texto estático.
- System Overview arriba a la derecha: CPU, RAM, VRAM, disco y uptime obtenidos mediante APIs del OS.
- Notifications debajo: eventos reales del sistema y del agente.
- Now Playing abajo a la derecha: servicio multimedia userspace, no parte del kernel.
- Dock inferior: accesos a Terminal, Files, Code, Marcus, System Monitor y Settings.

## Dependencias de implementación

1. M3: memoria y buffers de framebuffer.
2. M4/M7: procesos, input, syscalls y servicios userspace.
3. M10: compositor, window manager y desktop shell.
4. Marcus GUI: panel conectado por IPC a Marcus userspace.

El kernel debe exponer métricas y superficies mediante syscalls/IPC. Kryo será el lenguaje principal del compositor, window manager y servicios; C/ASM se reservará para drivers, instrucciones de CPU y boundaries FFI.