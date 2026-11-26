# Ajuste de imagen y recorte de bordes en PS4

[English](PS4_DISPLAY_FIT.md) | [Español](PS4_DISPLAY_FIT-es.md)

## Objetivo

Mostrar todo el escritorio de OrbisDesk en la pantalla de PS4, sin recortes en los bordes izquierdo, derecho, superior o inferior. El primer objetivo de aceptación física es el monitor de 1360×768 del usuario. La solución no debe asumir que la resolución del panel del monitor coincide con el modo HDMI activo de PS4.

## Confirmado en el código actual de `dev`

- [x] El síntoma informado es un recorte en los cuatro bordes, no solo un desplazamiento o un único lado.
- [x] El sink original fijaba `OUT_WIDTH=1280`, `OUT_HEIGHT=720` y dos buffers; ahora consulta el modo VideoOut activo y dimensiona los buffers según `width`/`height`.
- [x] `open_video()` ahora reserva y registra los buffers VideoOut con el modo activo consultado. La alternativa de CPU limpia y direcciona la salida usando anchura, altura y pitch dinámicos.
- [x] `show()` escala el escritorio dentro del rectángulo de salida dinámico, lo centra y puede estirarlo a todo el rectángulo cuando está activada la opción ESTIRAR del programa.
- [x] La posición del cursor, el indicador de actividad, la interfaz de Steam, la superposición y la composición de ventanas separadas reciben la misma geometría dinámica.
- [x] El sink de GPU acepta anchura y altura dinámicas en `pw_sink_gpu_buffers()` y crea sus imágenes, framebuffers, área de renderizado y viewport con esas dimensiones.
- [x] La declaración de OpenOrbis usa `int32_t sceVideoOutGetResolutionStatus(int32_t, OrbisVideoOutResolutionStatus *)`; el estado expone `width`, `height`, `paneWidth` y `paneHeight`. El enlazador del arranque ya incluye `-lSceVideoOut`. Aún falta compilar con el SDK fijado y probar la llamada en una PS4.

## Trabajo completado en esta investigación

- [x] Se volvieron a leer las instrucciones de la rama actual `dev` y la implementación del sink de pantalla.
- [x] Se confirmó el HEAD de la rama antes de los cambios: `f3b98b622abbeb6880b4c04c6b2363e705aac4c4`.
- [x] Se siguieron las dimensiones fijas por el registro de VideoOut, el escalado por CPU, la posición del cursor y las llamadas de superposición.
- [x] Se comprobó la declaración de la API y la estructura de estado de OpenOrbis, y se confirmó que el enlace del arranque incluye `-lSceVideoOut`.
- [x] Se añadió una consulta al inicio, después de `sceVideoOutOpen()`. Las dimensiones válidas del modo completo (320×200 a 4096×2160) dimensionan los buffers VideoOut de CPU o GPU; si falla la consulta o los valores no son plausibles, se conserva el respaldo de 1280×720. El registro muestra por separado las dimensiones completas y del panel, sin asumir que el panel sea el área segura.
- [x] No se sustituyó a ciegas el tamaño del panel 1360×768 del monitor; la consola consulta su propio modo VideoOut activo.

## Lista de implementación

- [ ] Compilar/enlazar con el toolchain OpenOrbis exacto del proyecto y confirmar que se resuelve la importación; verificar en la PS4 el retorno y las dimensiones informadas.
- [x] Registrar al inicio las dimensiones de salida activas y las dimensiones del panel; si falla la consulta, se conserva 1280×720 y el escritorio puede iniciar.
- [ ] Verificar en la PS4 que `width`/`height` se aceptan como tamaño del framebuffer registrado; determinar el significado de `paneWidth`/`paneHeight` sin asumir que son el área segura.
- [x] Sustituir las constantes de salida 1280×720 por anchura/altura validadas en tiempo de ejecución y pitch dinámico; conservar 1280×720 como respaldo si la consulta falla o devuelve dimensiones inverosímiles.
- [ ] Consolidar la geometría en una estructura explícita y verificar si los cambios de modo durante la ejecución se admiten o no.
- [x] Dimensionar la reserva de memoria directa, atributos de buffers, limpieza, escrituras de CPU y diagnósticos BMP según la geometría validada; las dimensiones se limitan antes de reservar y se comprueban los fallos de reserva/mapeo.
- [x] Pasar la misma geometría a la creación de buffers GPU, configuración de framebuffer/render pass, viewport/scissor y todas las rutas de presentación.
- [ ] Mantener independientes las dimensiones lógicas del escritorio Wine y las dimensiones físicas del buffer VideoOut.
- [ ] Añadir un margen/escala configurable de área segura si el síntoma se debe al overscan HDMI y no a un modo de framebuffer incorrecto. El comportamiento predeterminado debe mantener visibles todos los píxeles del escritorio; no recortar contenido silenciosamente.
- [x] Mantener coherentes el ajuste conservando proporción y la opción ESTIRAR por programa, incluidos los márgenes negros laterales o superior/inferior, con el tamaño de salida activo.
- [ ] Revisar cursor, indicador de actividad, FPS, interfaz Steam, superposición, ventanas separadas y mapeo del puntero respecto del rectángulo visible final del escritorio.
- [x] Conservar las rutas de respaldo por CPU y la presentación GPU/capas.
- [ ] Añadir pruebas de geometría en el host para modos 16:9 y 16:10 comunes, dimensiones impares, modos pequeños/invalidos y cálculos del área segura.
- [ ] Ejecutar las comprobaciones específicas del repositorio y revisar el diff final antes de marcar el trabajo de código como terminado.

## Pruebas físicas en PS4 — pendientes de acceso al hardware del usuario

- [ ] Registrar la resolución seleccionada en PS4 y los valores que VideoOut informa en tiempo de ejecución.
- [ ] En el monitor de 1360×768, confirmar que se ven los cuatro bordes exteriores y las cuatro esquinas del escritorio.
- [ ] Confirmar que la barra de tareas, menú Inicio, bordes de ventanas, texto e iconos no están recortados.
- [ ] Mover el puntero a las cuatro esquinas y verificar el mapeo y los límites del puntero.
- [ ] Abrir la superposición de controles y verificar que todos los bordes y elementos del menú permanecen visibles.
- [ ] Probar una ventana normal, una maximizada, un programa a pantalla completa y un programa que presente mediante Vulkan.
- [ ] Probar ESTIRAR activado y desactivado; verificar que no haya recorte involuntario ni pérdida de proporción.
- [ ] Probar tanto la alternativa de CPU como la ruta GPU si la compilación permite seleccionar ambas.
- [ ] Capturar una foto/captura auténtica de la pantalla y el log de inicio correspondiente; registrar la identidad del build y del código fuente.
- [ ] Repetir después de cambiar la resolución de salida de PS4 o conectar otra pantalla, si es posible.

## Estado actual

**Diagnóstico:** se confirmó la geometría fija 1280×720 y ahora se reemplazó por una consulta al inicio del modo VideoOut activo. Las dimensiones del modo completo y del panel se registran por separado. Esto corrige una discrepancia de framebuffer/modo de salida cuando existe, pero todavía no compensa el overscan HDMI ni demuestra que los campos del panel representen un área segura.

**Estado del código:** el dimensionamiento dinámico de salida está implementado en `dev`; siguen pendientes la validación de compilación/enlace y la aceptación física en PS4. Todavía no se afirma que la corrección esté verificada en hardware.
