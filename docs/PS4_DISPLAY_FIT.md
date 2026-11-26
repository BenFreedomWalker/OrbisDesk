# PS4 display fit and edge-clipping checklist

[English](PS4_DISPLAY_FIT.md) | [Español](PS4_DISPLAY_FIT-es.md)

## Goal

Make the OrbisDesk desktop fully visible on the PS4 display, with no clipping at the left, right, top or bottom edges. The first target for physical acceptance is the user's 1360×768 monitor. The fix must not assume that the monitor's panel resolution is the same as the PS4's active HDMI output mode.

## Confirmed from the current `dev` source

- [x] The reported symptom is clipping on all four edges, not just a single offset or one side.
- [x] The original sink hardcoded `OUT_WIDTH=1280`, `OUT_HEIGHT=720`, and two output buffers; the sink now queries the active VideoOut mode and sizes its output buffers from the reported `width`/`height`.
- [x] `open_video()` now allocates/registers VideoOut buffers using the queried active mode. The CPU fallback clears and addresses the output using the runtime width, height and pitch.
- [x] `show()` scales the desktop into the runtime output rectangle, centers it, and optionally stretches it to the full rectangle when the per-program ESTIRAR setting is enabled.
- [x] Cursor placement, busy indicator, FPS, Steam UI, overlay and separate windows respect the same visible output rectangle.
- [x] The GPU sink accepts runtime width and height in `pw_sink_gpu_buffers()` and creates its images, framebuffers, render area and viewport from those dimensions.
- [x] The OpenOrbis declaration uses `int32_t sceVideoOutGetResolutionStatus(int32_t, OrbisVideoOutResolutionStatus *)`; the status exposes `width`, `height`, `paneWidth` and `paneHeight`. The project's boot linker already links `-lSceVideoOut`. The exact pinned SDK import and target-runtime call still require a build and PS4 run.

## Work completed in this investigation

- [x] Re-read the current `dev` branch instructions and display sink implementation.
- [x] Confirmed that the work is being carried out on the existing `dev` branch; the live branch head is intentionally not frozen in this checklist.
- [x] Traced the fixed output dimensions through VideoOut registration, CPU scaling, cursor placement and overlay calls.
- [x] Checked the OpenOrbis API declaration and status structure, and confirmed the boot link includes `-lSceVideoOut`.
- [x] Added a startup query after `sceVideoOutOpen()`. Valid full-mode dimensions (320×200 through 4096×2160) now size the CPU or GPU VideoOut buffers; invalid/query-failure results retain the 1280×720 fallback. The log records both full and pane dimensions without treating pane dimensions as safe-area values.
- [x] Did not substitute the monitor's 1360×768 panel size by guess; the active VideoOut mode is queried from the console instead.
- [x] Corrected escaped newline sequences in the VideoOut startup diagnostics so each message is emitted on its own log line.
- [x] Investigated the available margin/overscan symbols: `sceVideoOutSetWindowModeMargins` is not a documented general full-screen overscan control, and `sceVideoOutSysSetOverscanRatio` is a private interface with no usable signature in the public SDK.
- [x] Confirmed `sceSystemServiceGetDisplaySafeAreaInfo` returns a `float` safe-area ratio (documented platform contract in GameMaker). Since OpenOrbis leaves the return type unspecified, the sink loads `libSceSystemService.sprx` and resolves the symbol with `sceKernelDlsym`, avoiding a static link dependency.

## Implementation checklist

- [ ] Build/link against the project's exact pinned OpenOrbis toolchain and confirm the import resolves; verify the call's runtime return and reported dimensions on target firmware.
- [x] Log the queried active output dimensions and pane dimensions at startup; query failure retains the 1280×720 fallback and does not block startup.
- [ ] Verify on the target PS4 that `width`/`height` are accepted as the registered framebuffer size; determine the meaning of `paneWidth`/`paneHeight` without assuming they are safe-area dimensions.
- [x] Replace the compile-time 1280×720 output constants with validated runtime width/height and runtime pitch; preserve a 1280×720 fallback when the query fails or returns implausible dimensions.
- [ ] Consolidate the runtime geometry into one explicit structure and verify that mode changes during a running title are either handled or explicitly unsupported.
- [x] Size direct-memory allocation, buffer attributes, clearing, CPU writes and BMP diagnostics from the validated runtime geometry; dimensions are bounded before allocation and allocation/map failures are checked.
- [x] Pass the same geometry through GPU buffer creation, framebuffer/render-pass setup, viewport/scissor and all presentation paths.
- [ ] Keep the logical Wine desktop dimensions independent from the physical VideoOut buffer dimensions.
- [x] Query the safe-area ratio at startup; if the module, symbol or value is invalid, use 1.0 and retain full-frame output.
- [x] Apply the safe rectangle to CPU/GPU scaling, cursor, busy indicator, FPS, Steam, overlay and separate windows. ESTIRAR fills the safe area, not the full output.
- [ ] Verify the ABI and behavior on actual PS4 firmware; do not call the private `sceVideoOutSysSetOverscanRatio`.
- [x] Keep aspect-preserving fit and per-program stretch behavior consistent, including letterboxing/pillarboxing, at the active output size.
- [x] Audit and constrain cursor, busy indicator, FPS, Steam UI, overlay and separate windows to the safe rectangle; physical pointer mapping validation remains pending.
- [x] Preserve CPU fallback and GPU/layered presentation behavior.
- [ ] Add host-side geometry tests for common 16:9 and 16:10 modes, odd dimensions, tiny/invalid modes, and safe-area calculations.
- [ ] Run the repository's focused checks and inspect the final diff before marking source work complete.

## Physical PS4 test checklist — pending user hardware access

- [ ] Record the PS4's selected display resolution and the values reported by VideoOut at runtime.
- [ ] On the 1360×768 monitor, confirm the desktop's four outer edges and all four corners are visible.
- [ ] Confirm taskbar, Start menu, window borders, text and desktop icons are not clipped.
- [ ] Move the pointer to all four corners and verify pointer mapping and clipping.
- [ ] Open the control overlay and verify every edge and menu item remains visible.
- [ ] Test a normal window, a maximized window, a full-screen program and a Vulkan-presenting program.
- [ ] Test ESTIRAR on and off; verify no unintended crop and no regression in aspect ratio.
- [ ] Test the CPU fallback as well as the GPU path if the build can select both.
- [ ] Capture a genuine display photo/screenshot and matching startup log; record build/source identity.
- [ ] Repeat after changing the PS4 output resolution or connecting another display, if available.

## Current status

**Diagnosis:** the sink queries the active VideoOut mode and the PS4-configured safe-area ratio. The desktop and its interfaces are fitted inside the safe rectangle; if the safe-area query fails, full-frame output is used. Acceptance on real PS4 firmware remains unverified.

**Source status:** runtime output sizing and safe-area fitting are implemented on `dev`. Compilation with the pinned SDK and physical PS4 acceptance remain pending; no hardware validation is claimed.
