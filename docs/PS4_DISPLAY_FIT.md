# PS4 display fit and edge-clipping checklist

[English](PS4_DISPLAY_FIT.md) | [Español](PS4_DISPLAY_FIT-es.md)

## Goal

Make the OrbisDesk desktop fully visible on the PS4 display, with no clipping at the left, right, top or bottom edges. The first target for physical acceptance is the user's 1360×768 monitor. The fix must not assume that the monitor's panel resolution is the same as the PS4's active HDMI output mode.

## Confirmed from the current `dev` source

- [x] The reported symptom is clipping on all four edges, not just a single offset or one side.
- [x] `platform/ps4/pw_wine_sink_ps4.c` hardcodes `OUT_WIDTH=1280`, `OUT_HEIGHT=720`, and two output buffers.
- [x] `open_video()` allocates/registers the VideoOut buffers using those fixed dimensions. The CPU fallback also clears and addresses the output using the fixed height, width and pitch.
- [x] `show()` scales the desktop into the fixed 1280×720 rectangle, centers it, and optionally stretches it to the full rectangle when the per-program ESTIRAR setting is enabled.
- [x] Cursor placement, busy indicator, Steam UI, overlay and separate-window composition are passed the same fixed output geometry.
- [x] The GPU sink already accepts output width and height in `pw_sink_gpu_buffers()`, but its output resources and viewport/framebuffer state must be audited before introducing runtime dimensions.
- [x] The checked-in OpenOrbis headers declare `sceVideoOutGetResolutionStatus()`; `OrbisVideoOutResolutionStatus` contains `width`, `height`, `paneWidth` and `paneHeight`. Availability in the exact linked SDK/runtime and the meaning of each field for this title still need verification.

## Work completed in this investigation

- [x] Re-read the current `dev` branch instructions and display sink implementation.
- [x] Confirmed the current branch head before making changes: `f3b98b622abbeb6880b4c04c6b2363e705aac4c4`.
- [x] Traced the fixed output dimensions through VideoOut registration, CPU scaling, cursor placement and overlay calls.
- [x] Checked the public OpenOrbis declarations for the VideoOut resolution-status API and structure.
- [x] Kept the implementation unchanged pending verification of the active output mode and safe-area behavior; changing 1280×720 to 1360×768 by guess would not be a reliable fix.

## Implementation checklist

- [ ] Verify the exact `sceVideoOutGetResolutionStatus` ABI, return convention, exported symbol and behavior with the project's pinned OpenOrbis toolchain and target firmware.
- [ ] Log the queried active output dimensions and pane dimensions at startup; handle query failure without preventing the desktop from starting.
- [ ] Decide which reported dimensions describe the registered framebuffer and which describe the visible output/pane. Do not assume `paneWidth`/`paneHeight` are safe-area dimensions without evidence.
- [ ] Replace compile-time output geometry with one validated runtime output-geometry structure, including width, height and pitch.
- [ ] Size direct-memory allocation, buffer attributes, clearing, CPU writes and BMP diagnostics from the validated runtime geometry, with overflow and allocation checks.
- [ ] Pass the same geometry through GPU buffer creation, framebuffer/render-pass setup, viewport/scissor and all presentation paths.
- [ ] Keep the logical Wine desktop dimensions independent from the physical VideoOut buffer dimensions.
- [ ] Add a configurable safe-area inset/scale if the symptom is HDMI overscan rather than a framebuffer-mode mismatch. Default behavior must keep all desktop pixels visible; avoid silently cropping content.
- [ ] Keep aspect-preserving fit and per-program stretch behavior consistent, including letterboxing/pillarboxing.
- [ ] Audit cursor, busy indicator, FPS, Steam UI, overlay, separate windows and input pointer-area mapping against the final visible desktop rectangle.
- [ ] Preserve CPU fallback and GPU/layered presentation behavior.
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

**Diagnosis:** fixed 1280×720 output geometry is confirmed. The user's four-edge clipping is consistent with a mismatch between the sink's full-frame output and the display's visible area, but the exact cause (active-mode mismatch, monitor overscan/safe-area scaling, or both) is not yet proven.

**Source fix:** not implemented yet. The next safe step is to establish the runtime VideoOut mode and distinguish output-mode sizing from overscan compensation. No hardware acceptance is claimed.
